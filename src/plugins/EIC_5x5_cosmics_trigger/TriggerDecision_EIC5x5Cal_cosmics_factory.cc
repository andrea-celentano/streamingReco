
#include <JANA/JLogger.h>
#include <JANA/JApplication.h>
#include <JANA/JFactoryGenerator.h>

#include "EIC5x5CalTrack_Factory.h"
#include "TriggerDecision_EIC5x5Cal_cosmics_factory.h"
#include "DAQ/TridasEvent.h"



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

    LOG << "Loading EIC5x5Cal_cosmics_trigger" << LOG_END;
    app->Add( new JFactoryGeneratorT<TriggerDecision_EIC5x5Cal_cosmics_factory>() );
    app->Add( new JFactoryGeneratorT<EIC5x5CalTrack_Factory>() );
}
}

//-----------------------------------------------
// TriggerDecision_EIC5x5Cal_cosmics_factory (constructor)
//-----------------------------------------------
TriggerDecision_EIC5x5Cal_cosmics_factory::TriggerDecision_EIC5x5Cal_cosmics_factory() {
	mTag="EIC5x5Cal_cosmics";
}

//-----------------------------------------------
// Init
//-----------------------------------------------
void TriggerDecision_EIC5x5Cal_cosmics_factory::Init() {

	ENABLED    = true;
	MIN_TRACKS = 1;
	MAX_TRACKS = 1000;
	MAX_HITS   = 8;

	mApp->SetDefaultParameter("TRIGGER:EIC5x5Cal_cosmics:ENABLED", ENABLED, "Set to 0 to disable the EIC5x5Cal_cosmics trigger completely (no TriggerDecision objects will be produced).");
	mApp->SetDefaultParameter("TRIGGER:EIC5x5Cal_cosmics:MIN_TRACKS", MIN_TRACKS, "Minimum number of EIC5x5CalTrack objects to trigger.");
	mApp->SetDefaultParameter("TRIGGER:EIC5x5Cal_cosmics:MAX_TRACKS", MAX_TRACKS, "Maximum number of EIC5x5CalTrack objects to trigger.");
	mApp->SetDefaultParameter("TRIGGER:EIC5x5Cal_cosmics:MAX_HITS", MAX_HITS, "Maximum number of HallDCalhit objects to trigger.");
}

//-----------------------------------------------
// Process
//-----------------------------------------------
void TriggerDecision_EIC5x5Cal_cosmics_factory::Process(const std::shared_ptr<const JEvent> &event) {

	if( !ENABLED ) return; // allow user to disable this via JANA config. param.

	// Check if there are more than MAX_HITS in the event. This
	// gives some ability to cut out beam events that may well 
	// fit a track pattern.
	auto hits = event->Get<HallDCalHit>();
	bool decision = hits.size() <= MAX_HITS;

	// Only check for cosmic tracks if we are not already rejecting 
	// the event due to too many hits.
	if( decision ){
		// Get track objects from factory
		auto trks = event->Get<EIC5x5CalTrack>();
		int Ntrks = trks.size();

		decision &= ((Ntrks>=MIN_TRACKS) && (Ntrks<=MAX_TRACKS));
	}

	// Create TriggerDecision object to publish the decision
	// Argument is trigger description. It will end up in metadata file so keep it simple.
	// I think a good convention here is to just give it the tag of the factory.
	auto mTriggerDecision = new TriggerDecision( mTag ); 
	mTriggerDecision->SetDecision( decision );
	mTriggerDecision->SetID(0x01); // this will show up in 16 high order bit in TriggeredEvent::plugin_nseeds[] (lower 16 will be 0 or 1 depending on whether trigger fired)
	mData.push_back(mTriggerDecision);	
}



