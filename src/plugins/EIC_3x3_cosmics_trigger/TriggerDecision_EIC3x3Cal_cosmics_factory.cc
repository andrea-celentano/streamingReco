
#include <JANA/JLogger.h>
#include <JANA/JApplication.h>
#include <JANA/JFactoryGenerator.h>

#include "EIC3x3CalTrack_Factory.h"
#include "TriggerDecision_EIC3x3Cal_cosmics_factory.h"
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

    LOG << "Loading EIC3x3Cal_cosmics_trigger" << LOG_END;
    app->Add( new JFactoryGeneratorT<TriggerDecision_EIC3x3Cal_cosmics_factory>() );
    app->Add( new JFactoryGeneratorT<EIC3x3CalTrack_Factory>() );
}
}

//-----------------------------------------------
// TriggerDecision_EIC3x3Cal_cosmics_factory (constructor)
//-----------------------------------------------
TriggerDecision_EIC3x3Cal_cosmics_factory::TriggerDecision_EIC3x3Cal_cosmics_factory() {
	mTag="EIC3x3Cal_cosmics";
}

//-----------------------------------------------
// Init
//-----------------------------------------------
void TriggerDecision_EIC3x3Cal_cosmics_factory::Init() {

	ENABLED    = true;
	MIN_TRACKS = 1;
	MAX_TRACKS = 1000;

	mApp->SetDefaultParameter("TRIGGER:EIC3x3Cal_cosmics:ENABLED", ENABLED, "Set to 0 to disable the EIC3x3Cal_cosmics trigger completely (no TriggerDecision objects will be produced).");
	mApp->SetDefaultParameter("TRIGGER:EIC3x3Cal_cosmics:MIN_TRACKS", MIN_TRACKS, "Minimum number of EIC3x3CalTrack objects to trigger.");
	mApp->SetDefaultParameter("TRIGGER:EIC3x3Cal_cosmics:MAX_TRACKS", MAX_TRACKS, "Maximum number of EIC3x3CalTrack objects to trigger.");
}

//-----------------------------------------------
// Process
//-----------------------------------------------
void TriggerDecision_EIC3x3Cal_cosmics_factory::Process(const std::shared_ptr<const JEvent> &event) {

	if( !ENABLED ) return; // allow user to disable this via JANA config. param.

	// Get track objects from factory
	auto trks = event->Get<EIC3x3CalTrack>();
	int Ntrks = trks.size();

	bool decision = ((Ntrks>=MIN_TRACKS) && (Ntrks<=MAX_TRACKS));

	// Create TriggerDecision object to publish the decision
	// Argument is trigger description. It will end up in metadata file so keep it simple.
	// I think a good convention here is to just give it the tag of the factory.
	auto mTriggerDecision = new TriggerDecision( mTag ); 
	mTriggerDecision->SetDecision( decision );
	mTriggerDecision->SetID(0x01); // this will show up in 16 high order bit in TriggeredEvent::plugin_nseeds[] (lower 16 will be 0 or 1 depending on whether trigger fired)
	mData.push_back(mTriggerDecision);	
}



