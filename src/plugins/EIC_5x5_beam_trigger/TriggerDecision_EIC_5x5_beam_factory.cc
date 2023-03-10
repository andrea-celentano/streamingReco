
#include <JANA/JLogger.h>
#include <JANA/JApplication.h>
#include <JANA/JFactoryGenerator.h>

#include <HallD/HallDCalHit.h>
// #include <DAQ/faWaveboardHit.h>
#include "TriggerDecision_EIC_5x5_beam_factory.h"
// #include <addRecoFactoriesGenerators.h>



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

    LOG << "Loading EIC_5x5_cosmics_trigger" << LOG_END;
    app->Add( new JFactoryGeneratorT<TriggerDecision_EIC_5x5_beam_factory>() );

    // addRecoFactoriesGenerators(app);
}
}

//-----------------------------------------------
// TriggerDecision_EIC_5x5_cosmics_factory (constructor)
//-----------------------------------------------
TriggerDecision_EIC_5x5_beam_factory::TriggerDecision_EIC_5x5_beam_factory() {
	mTag="EIC5x5Cal_beam";
}

//-----------------------------------------------
// Init
//-----------------------------------------------
void TriggerDecision_EIC_5x5_beam_factory::Init() {

	ENABLED        = true;
	MIN_BLOCKS     = 2;
	ESUM_THRESHOLD = 0.0; 

	T_WINDOW = 100.0; // MS time window around seed to exclude uncorrelated hits

	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_beam:ENABLED", ENABLED, "Set to 0 to disable the EIC_5x5_beam trigger completely (no TriggerDecision objects will be produced).");
	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_beam:MIN_BLOCKS", MIN_BLOCKS, "Minimum number of calorimeter blocks hit to make a trigger.");
	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_beam:ESUM_THRESHOLD", ESUM_THRESHOLD, "Minimum sum of HallDCalhit energy values to make a trigger.");

	mApp->SetDefaultParameter("TRIGGER:EIC_5x5_beam:T_WINDOW", T_WINDOW, "Duration of time window centered on most energetic hit for identifying beam clusters");
}

//-----------------------------------------------
// Process
//-----------------------------------------------
void TriggerDecision_EIC_5x5_beam_factory::Process(const std::shared_ptr<const JEvent> &event) {

	if( !ENABLED ) return; // allow user to disable this via JANA config. param.
	
	// Get track objects from factory
	auto blks = event->Get<HallDCalHit>();
	
	float Eseed = 0, tseed;
	for(auto hit : blks){
		if(hit->getHitEnergy()>Eseed){
			Eseed = hit->getHitEnergy();
			tseed = hit->getHitTime();
		}
	}
	
	int Nblks = 0;
	int Ncluster[5][5]; // look for hit numer in terms of channels and not hits
	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++) {
			Ncluster[i][j]=0;
		}
	}	
	float Esum = 0.0;

	for(auto hit : blks) {
		if(fabs(hit->getHitTime()-tseed)<T_WINDOW){
			Esum += hit->getHitEnergy();
			Ncluster[hit->getHitIX()][hit->getHitIY()] = 1;
		}	
	}
	
	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++){
			Nblks += Ncluster[i][j];
		}
	}		

	bool decision = (Nblks>=MIN_BLOCKS) && (Esum>=ESUM_THRESHOLD);

	// Create TriggerDecision object to publish the decision
	// Argument is trigger description. It will end up in metadata file so keep it simple.
	// I think a good convention here is to just give it the tag of the factory.
	auto mTriggerDecision = new TriggerDecision( mTag ); 
	mTriggerDecision->SetDecision( decision );
	mTriggerDecision->SetID(0x02); // this will show up in 16 high order bit in TriggeredEvent::plugin_nseeds[] (lower 16 will be 0 or 1 depending on whether trigger fired)
	mData.push_back(mTriggerDecision);	
}



