//
// Created by Nathan Brei on 2019-12-15.
//

#ifndef JANA2_PTFileEventSource_H
#define JANA2_PTFileEventSource_H

#include <JANA/JEventSource.h>
#include <JANA/Services/JEventGroupTracker.h>
#include <JANA/JEvent.h>

#include "DAQ/TridasEvent.h"
//TRIDAS headers
#include <tools/ptfile_reader.hpp>
#include <packages/DataFormats/s_dataformat_bdx.hpp>
using namespace tridas::post_trigger;

class JEventSourcePTFile: public JEventSource {

	int curRunNumber, curEventNumber, curEventType;
	uint32_t m_pos = 0;

	PtFileReader<sample::uncompressed> *ptReader;
	//ITerator on file, each instance is a timeslice
	PtFileReader<sample::uncompressed>::iterator it_ptReader;
	TimeSlice<sample::uncompressed> *ptTimeSlice;

	//Iterator on timeslice, each instance is an event
	TimeSlice<sample::uncompressed>::iterator it_ptTimeSlice;
	Event<sample::uncompressed> *ptEvent;

	//int currEventTimeSlice;
	int nEventsTimeSlice;
public:

	JEventSourcePTFile(std::string res_name, JApplication* app);
	virtual ~JEventSourcePTFile(){};
	// A description of this source type must be provided as a static member
	static std::string GetDescription(void) {
		return "Event source for Tridas PT file";
	}

	void Open(void);
	void GetEvent(std::shared_ptr<JEvent>);
	//bool GetObjects(const std::shared_ptr<const JEvent>& aEvent, JFactory* aFactory);


};

//TODO: where to place eventually the following?
//   template<> double JEventSourceGeneratorT<MyType>::CheckOpenable(std::string source) { ... }

#endif //JANA2_PTFileEventSource_H
