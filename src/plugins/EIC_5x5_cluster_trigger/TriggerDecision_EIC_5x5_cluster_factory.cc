
#include <JANA/JLogger.h>
#include <JANA/JApplication.h>
#include <JANA/JFactoryGenerator.h>

#include <HallD/HallDCalHit.h>
#include <HallD/EIC_5x5CalCluster.h>
#include "TriggerDecision_EIC_5x5_cluster_factory.h"




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
	TAG            = "Hdbscan";
	MIN_CLUSTERS   = 1;
	MAX_CLUSTERS   = 2;
	E_THRESHOLD    = 0.0;

	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_cluster:ENABLED", ENABLED, "Set to 0 to disable the EIC_5x5_cluster trigger completely (no TriggerDecision objects will be produced).");
	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_cluster:TAG", TAG, "Factory tag to use for EIC_5x5CalCluster objects. (e.g. Hdbscan)");
	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_cluster:MIN_CLUSTERS", MIN_CLUSTERS, "Minimum number of clusters needed to make a trigger.");
	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_cluster:MAX_CLUSTERS", MAX_CLUSTERS, "Maximum number of clusters allowed to make a trigger.");
	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_cluster:E_THRESHOLD",  E_THRESHOLD,  "Minimum energy of single cluster to include in count of clusters to make a trigger.");
}

//-----------------------------------------------
// Process
//-----------------------------------------------
void TriggerDecision_EIC_5x5_cluster_factory::Process(const std::shared_ptr<const JEvent> &event) {

	if( !ENABLED ) return; // allow user to disable this via JANA config. param.
	
	// Get objects from factory
	auto clusters = event->Get<EIC_5x5CalCluster>(TAG);
	
	size_t Nclusters = 0;
	for( auto cluster : clusters ){
		if( cluster->getClusterEnergy() > E_THRESHOLD ) Nclusters++;
	}

	bool decision = (Nclusters>=MIN_CLUSTERS) && (Nclusters<=MAX_CLUSTERS);


	// Create TriggerDecision object to publish the decision
	// Argument is trigger description. It will end up in metadata file so keep it simple.
	// I think a good convention here is to just give it the tag of the factory.
	auto mTriggerDecision = new TriggerDecision( mTag ); 
	mTriggerDecision->SetDecision( decision );
	mTriggerDecision->SetID(0x03); // this will show up in 16 high order bit in TriggeredEvent::plugin_nseeds[] (lower 16 will be 0 or 1 depending on whether trigger fired)
	mData.push_back(mTriggerDecision);	
}

