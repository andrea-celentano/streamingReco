
#include <JANA/JLogger.h>
#include <JANA/JApplication.h>
#include <JANA/JFactoryGenerator.h>

#include <HallD/HallDCalHit.h>
#include "TriggerDecision_EIC_5x5_cluster_factory.h"


#include <FT/kmeans/kmeans.h>
#include <FT/hdbscandir/Hdb.h>
#include <FT/hdbscandir/hdbscanAlgorithm.h>



extern "C" {
void InitPlugin(JApplication* app) {

    InitJANAPlugin(app);

    LOG << "Loading EIC_5x5_cosmics_trigger" << LOG_END;
    app->Add( new JFactoryGeneratorT<TriggerDecision_EIC_5x5_cluster_factory>() );

}
};

//-----------------------------------------------
// TriggerDecision_EIC_5x5_cosmics_factory (constructor)
//-----------------------------------------------
TriggerDecision_EIC_5x5_cluster_factory::TriggerDecision_EIC_5x5_cluster_factory() {
	mTag="EIC5x5Cal_cluster";
}

//-----------------------------------------------
// Init
//-----------------------------------------------
void TriggerDecision_EIC_5x5_cluster_factory::Init() {

	ENABLED        = true;

	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_cluster:ENABLED", ENABLED, "Set to 0 to disable the EIC_5x5_cluster trigger completely (no TriggerDecision objects will be produced).");

}

//-----------------------------------------------
// Process
//-----------------------------------------------
void TriggerDecision_EIC_5x5_cluster_factory::Process(const std::shared_ptr<const JEvent> &event) {

	if( !ENABLED ) return; // allow user to disable this via JANA config. param.
	
	// Get track objects from factory
	// auto blks = event->Get<HallDCalHit>();
	

	bool decision = false;

	// Create TriggerDecision object to publish the decision
	// Argument is trigger description. It will end up in metadata file so keep it simple.
	// I think a good convention here is to just give it the tag of the factory.
	auto mTriggerDecision = new TriggerDecision( mTag ); 
	mTriggerDecision->SetDecision( decision );
	mTriggerDecision->SetID(0x03); // this will show up in 16 high order bit in TriggeredEvent::plugin_nseeds[] (lower 16 will be 0 or 1 depending on whether trigger fired)
	mData.push_back(mTriggerDecision);	
}

