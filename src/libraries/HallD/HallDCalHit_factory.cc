/*
 * faWaveboardHitfactory.cc
 *
 *  Created on: Dec 23, 2019
 *      Author: celentan
 */

#include "HallDCalHit_factory.h"
#include "JANA/JEvent.h"

#include "DAQ/faWaveboardHit.h"
#include "DAQ/fa250VTPMode7Hit.h"
#include "TT/TranslationTable.h"

HallDCalHit_factory::HallDCalHit_factory() {
	// TODO Auto-generated constructor stub
	m_tt = 0;

}

HallDCalHit_factory::~HallDCalHit_factory() {
	// TODO Auto-generated destructor stub
}

void HallDCalHit_factory::Init() {

}
void HallDCalHit_factory::ChangeRun(const std::shared_ptr<const JEvent> &aEvent) {

	//TODO: get the TT
	std::cout << "HallDCalHit_factory::ChangeRun run number: " << aEvent->GetRunNumber() << " " << this << " " << m_tt << " " << std::endl;
	if (m_tt == 0) {
		//std::cout << "HallDCalHit_factory::get TT" << std::endl;
		m_tt = aEvent->GetSingle<TranslationTable>();
		//	std::cout << "HallDCalHit_factory: got TT" << std::endl;
	}

}
void HallDCalHit_factory::Process(const std::shared_ptr<const JEvent> &aEvent) {

	TranslationTable::ChannelInfo m_channel;

	//Get the hits from FADC. Support bot the waveboard hit and the fa250VTPMode7 hit
	auto faHits_waveboard = aEvent->Get<faWaveboardHit>();
	auto faHits_fa250VTPMode7 = aEvent->Get<fa250VTPMode7Hit>();

	for (auto faHit : faHits_waveboard) {

		m_channel = m_tt->getChannelInfo(faHit->m_channel);

		if ((m_channel.det_sys == TranslationTable::HallDCAL)) {
			//Convert the waveboard hit. Probably will never be used, unless we will perform HallD tests with waveboard.
			//It is used to test the system with data from a waveboard

			//Create a new HallDCal Hit Object object
			auto mHallDCalHit = new HallDCalHit();

			//Assign the channel
			mHallDCalHit->m_channel = *(m_channel.HallDCAL);

			//Assign the time
			mHallDCalHit->setHitTime(4 * faHit->m_time.count());

			//Assign the energy
			double mPED = 0;
			double mQ = 0;
			const int NPED = 10;
			for (int ii = 0; ii < faHit->samples.size(); ii++) {
				if (ii < NPED)
					mPED += faHit->samples[ii];
				mQ += faHit->samples[ii];
			}

			mPED /= NPED;
			mPED *= faHit->samples.size();
			mQ = mQ - mPED;
			mHallDCalHit->setHitEnergy(mQ);

			mData.push_back(mHallDCalHit);
		}
	}

	//Here is the important part - converting the faHit from VTPMode7 to HallDCalHit
	for (auto faHit : faHits_fa250VTPMode7) {
		m_channel = m_tt->getChannelInfo(faHit->m_channel);
		// _DBG_<<"Detector Type: " << m_tt->DetectorName(m_channel.det_sys) << std::endl;
		// _DBG_<<"crate="<<(int)faHit->m_channel.crate<<" slot="<<(int)faHit->m_channel.slot<<" channel="<<(int)faHit->m_channel.channel<<std::endl;
		// _DBG_<<"crate="<<(int)m_channel.CSC.crate<<" slot="<<(int)m_channel.CSC.slot<<" channel="<<(int)m_channel.CSC.channel<<std::endl;
		if ((m_channel.det_sys == TranslationTable::EIC3x3CAL)) {

			//Create a new HallDCal Hit Object object
			auto mHallDCalHit = new HallDCalHit();

			//Assign the channel
			mHallDCalHit->m_channel = *(m_channel.HallDCAL);

			//Assign the time
			mHallDCalHit->setHitTime(4 * faHit->m_time.count());

			//Assign the energy
			//TODO: eventually apply another correction, here I just take the energy as provided by VTP
			mHallDCalHit->setHitEnergy(faHit->m_charge);

			mData.push_back(mHallDCalHit);
		}
	}

}

