
#include <JANA/JLogger.h>
#include <JANA/JApplication.h>
#include <JANA/JFactoryGenerator.h>

#include <HallD/HallDCalHit.h>
#include <DAQ/faWaveboardHit.h>
#include "TriggerDecision_HallDCal_beam_factory.h"
#include <addRecoFactoriesGenerators.h>



extern "C" {
void InitPlugin(JApplication* app) {

    // This code is executed when the plugin is attached.
    // It should always call InitJANAPlugin(app) first, and then do one or more of:
    //   - Read configuration parameters
    //   - Register JFactoryGenerators
    //   - Register JEventProcessors
    //   - Register JEventSourceGenerators (or JEventSources directly)
    //   - Register JServices

    InitJANAPlugin(app);

    LOG << "Loading HallDCal_cosmics_trigger" << LOG_END;
    app->Add( new JFactoryGeneratorT<TriggerDecision_HallDCal_beam_factory>() );

    addRecoFactoriesGenerators(app);
}
}

//-----------------------------------------------
// TriggerDecision_HallDCal_cosmics_factory (constructor)
//-----------------------------------------------
TriggerDecision_HallDCal_beam_factory::TriggerDecision_HallDCal_beam_factory() {
	mTag="HallDCal_beam";
}

//-----------------------------------------------
// Init
//-----------------------------------------------
void TriggerDecision_HallDCal_beam_factory::Init() {

	ENABLED       = true;
	MIN_BLOCKS    = 2;
	MIN_HODO_HITS = 0;

	mApp->SetDefaultParameter("TRIGGER:HallDCal_beam:ENABLED", ENABLED, "Set to 0 to disable the HallDCal_beam trigger completely (no TriggerDecision objects will be produced).");
	mApp->SetDefaultParameter("TRIGGER:HallDCal_beam:MIN_BLOCKS", MIN_BLOCKS, "Minimum number of calorimeter blocks hit to make a trigger.");
	mApp->SetDefaultParameter("TRIGGER:HallDCal_beam:MIN_HODO_HITS", MIN_HODO_HITS, "Minimum number of hodoscope scintillators hit (def:0  should be <=2)");
}

//-----------------------------------------------
// Process
//-----------------------------------------------
void TriggerDecision_HallDCal_beam_factory::Process(const std::shared_ptr<const JEvent> &event) {

	if( !ENABLED ) return; // allow user to disable this via JANA config. param.

	// Get track objects from factory
	auto blks = event->Get<HallDCalHit>();
	int Nblks = blks.size();

	// Try and get hodoscope scintillator hits. By the 
	// translation table for now since they will take too
	// long to figure out and implement for the beam period.
	auto faHits_waveboard = event->Get<faWaveboardHit>();
	bool scint1_hit = false;
	bool scint2_hit = false;
	for (auto faHit : faHits_waveboard) {
		auto m_channel = faHit->m_channel;
		if( m_channel.crate == 0 ){
			if( m_channel.slot == 7 ){
				if( m_channel.channel == 10 ) scint1_hit = true;
				if( m_channel.channel == 11 ) scint2_hit = true;
			}
		}
		if((int)m_channel.channel > 9)_DBG_ << "crate=" << (int)m_channel.crate << "  slot=" << (int)m_channel.slot << "  channel=" << (int)m_channel.channel << std::endl;
	}
	int num_hodo_hits = 0;
	if( scint1_hit ) num_hodo_hits++;
	if( scint2_hit ) num_hodo_hits++;

	bool decision = (Nblks>=MIN_BLOCKS) && (num_hodo_hits>=MIN_HODO_HITS);

	// Create TriggerDecision object to publish the decision
	// Argument is trigger description. It will end up in metadata file so keep it simple.
	// I think a good convention here is to just give it the tag of the factory.
	auto mTriggerDecision = new TriggerDecision( mTag ); 
	mTriggerDecision->SetDecision( decision );
	mTriggerDecision->SetID(0x02); // this will show up in 16 high order bit in TriggeredEvent::plugin_nseeds[] (lower 16 will be 0 or 1 depending on whether trigger fired)
	mData.push_back(mTriggerDecision);	
}



