#include "JEventSource_PTFile.h"
#include "TRIDAS/s_dataformat_bdx.hpp"
#include <iostream>
#include <chrono>

JEventSourcePTFile::JEventSourcePTFile(std::string res_name, JApplication* app) :
		JEventSource(std::move(res_name), app), ptReader(0) {

}

//-------------------------------------------------------------------------
// Open
//
// The Open method is called before events are read from this source. You
// could open the file in the constructor, but it is recommended to wait
// until Open is called. This is because JEventSource objects are made at
// the beginning of the job, though not all files will be read right away.
//-------------------------------------------------------------------------
void JEventSourcePTFile::Open() {

	LOG << "Scanning input file \"" << this->GetResourceName() << "\" ..." << LOG_END;
	ptReader = new PtFileReader<sample::uncompressed>(this->GetResourceName());
	
	// Print stats about file
	LOG << "TriDAS PT File opened." << LOG_END;
	LOG << "===================================================" << LOG_END;
	LOG << "            File name: " << this->GetResourceName()  << LOG_END;
	LOG << "           Run Number: " << ptReader->runNumber()    << LOG_END;
	LOG << "          File Number: " << ptReader->fileNumber()   << LOG_END;
	LOG << "     Num. Time Slices: " << ptReader->nTS()          << LOG_END;
	LOG << "          Num. Events: " << ptReader->nEvents()      << LOG_END;
	LOG << "File Size (effective): " << ptReader->fileSize()     << LOG_END;
	LOG << "       Data Card Size: " << ptReader->datacardSize() << LOG_END;
	LOG << "===================================================" << LOG_END;

	try{
		//point to the first time slice
		_DBG__;
		it_ptReader = ptReader->begin();
		_DBG__;
		nEventsTimeSlice = (*it_ptReader).nEvents();

		//Create a new time slice
		_DBG__;
		ptTimeSlice = new TimeSlice<sample::uncompressed>(*it_ptReader);
		_DBG__;
		it_ptTimeSlice = ptTimeSlice->begin();
	}catch(...){
		LOG << "Exception while trying to grab initial time slice from PT file! This is fatal so quitting now ..." << LOG_END;
		japp->Quit(true);
		return;
	}

	//currEventTimeSlice = 0;
	LOG << "JEventSourcePTFile creator DONE: " << this << LOG_END;

}
//-------------------------------------------------------------------------
// GetEvent
//
// This method is called to read in a single "event"
//-------------------------------------------------------------------------
void JEventSourcePTFile::GetEvent(std::shared_ptr<JEvent> event) {

	// Create a JEvent object and fill in important info. It is expected that
	// the most common operation will be to read in a buffer and store a pointer
	// to it in the event object. You should not spend much effort here processing
	// the data since this method is called serially. Defer as much as possible
	// to the GetObjects method below which may be called in parallel.

	if (it_ptTimeSlice == ptTimeSlice->end()) {
		it_ptReader++;
		delete ptTimeSlice;
		//If the iterator on time slices is at the end, the source is completely read.
		if (it_ptReader == ptReader->end()) {
			std::cout << "Source done" << std::endl;
			fflush(stdout);
			throw JEventSource::RETURN_STATUS::kNO_MORE_EVENTS;
		} else {
			nEventsTimeSlice = (*it_ptReader).nEvents();
			ptTimeSlice = new TimeSlice<sample::uncompressed>(*it_ptReader);
			it_ptTimeSlice = ptTimeSlice->begin();
			//currEventTimeSlice = 0;
		}
	}

	curRunNumber = ptReader->runNumber();
	ptEvent = new Event<sample::uncompressed>(*it_ptTimeSlice++);
	m_pos += ptEvent->size(); // try and keep track of position in file so we can report progress

	//TODO: allow for a user-defined run number
	event->SetRunNumber(curRunNumber);
//	event->SetEventNumber(ptEvent->id());
	static int evtNumber=0;
	event->SetEventNumber(evtNumber++);

	// To give user better indication of how far we are into file,
	// periodically print message.
	static auto t_last = time(NULL);
	auto t_now = time(NULL);
	if( (t_now-t_last)%4 ){
		t_last = t_now;
		
		auto num = GetEventCount();
		auto len = ptReader->nEvents();
		
		// Calculate rate we are reading in MB/s
		static auto last_pos = m_pos;
		static auto t_last = std::chrono::steady_clock::now();
		auto t_now = std::chrono::steady_clock::now();
		auto t_diff = std::chrono::duration_cast<std::chrono::microseconds>(t_now-t_last).count();
		static double MB_per_sec = 0.0;
		if( t_diff>=1000000 ){
			
			auto pos = m_pos;
			auto bytes_diff = pos - last_pos;
			MB_per_sec = (double)bytes_diff/(double)t_diff;

			last_pos = pos;
			t_last = t_now;
		}
		
		double percent_done = 100.0*(double)num/(double)len;
 		char str[256];
		sprintf(str, "--- PTFileSource: %ld/%ld time slices read (%3.1f%%)  %3.1fMB (%3.3fMB/s)", num, len, percent_done, last_pos/1.0E6, MB_per_sec);
		
		LOG << str << LOG_END; 
	}

//	event.SetEventTS(ptTimeSlice->id());
//	event->Insert(ptEvent);

	TridasEvent *tridasEvent = new TridasEvent();

	//add the trigger word
	tridasEvent->triggerWords.push_back(tridas::bdx::MAX_TRIGGERS_NUMBER); //currently 5
	for (int ii = 0; ii < tridas::bdx::MAX_TRIGGERS_NUMBER; ii++) {
		tridasEvent->triggerWords.push_back(ptEvent->nseeds(ii));
	}
	tridasEvent->triggerWords.push_back(tridas::bdx::MAX_PLUGINS_NUMBER); //currently 8
	for (int ii = 0; ii < tridas::bdx::MAX_PLUGINS_NUMBER; ii++) {
		tridasEvent->triggerWords.push_back(ptEvent->plugin_nseeds(ii));
	}
	for (int ii = 0; ii < tridas::bdx::MAX_PLUGINS_NUMBER; ii++) {
		tridasEvent->triggerWords.push_back(ptEvent->plugin_trigtype(ii));
	}



	for (Event<sample::uncompressed>::iterator it = ptEvent->begin(); it != ptEvent->end(); ++it) {
		Hit<sample::uncompressed> hit = (*it);	//This is the HIT

		fadcHit fhit;

		fhit.crate = hit.frameHeader(0).TowerID;
		fhit.slot = hit.frameHeader(0).EFCMID;
		fhit.channel = hit.frameHeader(0).PMTID;

		//add the time
		T5nsec t(hit.frameHeader(0).T5ns);
		fhit.time = t;

		//add the charge
		fhit.charge = 0;
		for (int q = 0; q < hit.nFrames(); q++)
			fhit.charge += hit.frameHeader(q).Charge;

		//add the samples
		for (auto it_ptSample = hit.begin(); it_ptSample != hit.end(); it_ptSample++) {
			fhit.data.push_back(*it_ptSample);
		}

		/*	At the moment (2020), TriDAS can be feed by three hardware sources:

		 - Waveboard (V1)
		 - Waveboard (V2)
		 - fa250 stream trough VTP

		 In the future, there may be more than one hit type per event, if the readout architecture is mixed.
		 Ideally, one would recognize the hit type from the hit itself, and have one JANA2 factory per hit - hits may be later processed differently.
		 One can judge this from the crate/slot/channel combination, but this is setup-dependent.
		 For the moment, this is not supported. Hence, for the moment I differentiate between waveboard and fa250 by considering that the fa250 has no samples, only time and charge.
		 */
		if (fhit.data.size() == 0)
			fhit.type = fadcHit_TYPE::FA250VTPMODE7;
		else
			fhit.type = fadcHit_TYPE::WAVEBOARD;

		tridasEvent->hits.push_back(fhit);
	}
	event->Insert(tridasEvent);

	delete ptEvent;
	ptEvent = 0;
}

/*
 bool JEventSourcePTFile::GetObjects(const std::shared_ptr<const JEvent>& aEvent, JFactory* aFactory) {

 //I organized this as follows.
 //From the ptEvent, in principle I can extract the faWaveboardHits and have the JEventSourcePTFile providing these directly.
 //In the online part (JEventSource_Tridas), I don't do this.
 //The online factory, interacting with the TriDAS, provides directly the TridasEvent,
 //and then there are different factories providing the fadc hits (faWaveboardHit_factory and fa250VTPMode7Hit_factory).
 //

 }
 */
