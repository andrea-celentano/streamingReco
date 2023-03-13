
#include "EIC_5x5Processor.h"

#include <HallD/HallDCalHit.h>
#include <EIC_5x5_cosmics_trigger/EIC5x5CalTrack.h>
#include <Trigger/TriggerDecision.h>
#include <DAQ/TridasEvent.h>
#include <JANA/JLogger.h>
#include <JANA/Services/JGlobalRootLock.h>
#include <JANA/JApplication.h>
#include <DAQ/fa250VTPMode7Hit.h>

#include <TFile.h>
#include <TH1D.h>
#include <TFolder.h>
#include <TROOT.h>

EIC_5x5Processor::EIC_5x5Processor() {
    SetTypeName(NAME_OF_THIS); // Provide JANA with this class's name
}

void EIC_5x5Processor::Init() {

    // Open TFiles, set up TTree branches, etc

    LOG << "EIC_5x5Processor::Init" << LOG_END;
	m_root_lock = japp->GetService<JGlobalRootLock>();
    m_root_lock->acquire_write_lock();
	
    // To make things a little easier, we want to default to creating a
    // root file if one is not already open. The user may also explicilty 
    // tell us not to create a new file here by setting this to an
    // empty string. They may also tell us to open one here regardless
    // by setting this to a nonempty string.
    bool fileAlreadyOpen = gDirectory->GetFile() != nullptr;
    std::string outFileName = fileAlreadyOpen ? "":"EIC_5x5.root";
    japp->SetDefaultParameter("EIC_5x5:output_file", outFileName, "root file name for EIC_5x5 plugin. (n.b. if not set and file open by another plugin, this will just create a EIC_5x5 directory there to write to)");

    // Optionally open output file
	if (! outFileName.empty() ) {
		std::cout << "JEventProcessor_EIC_5x5: output file name: " << this << " " << outFileName << std::endl;
		fout = new TFile(outFileName.c_str(), "recreate");
    }else{
        // If not opening our own output root file then create a
        // directory for all of our root objects
        gDirectory->mkdir("EIC_5x5")->cd();
    }


	tf250hit = new TTree("f250hit","fa250VTPMode7Hit");
	tf250hit->Branch("Nhits",   &Nf250hits,       "Nhits/I");
	tf250hit->Branch("crate",   &f250hit_crate,   "crate[Nhits]/i");
	tf250hit->Branch("slot",    &f250hit_slot,    "slot[Nhits]/i");
	tf250hit->Branch("channel", &f250hit_channel, "channel[Nhits]/i");
	tf250hit->Branch("id",      &f250hit_id,      "id[Nhits]/i");
    tf250hit->Branch("charge",  &f250hit_charge,  "charge[Nhits]/F");
    tf250hit->Branch("time",    &f250hit_time,    "time[Nhits]/F");
 
    tcalhit = new TTree("calhit","HallDCalHit");
	tcalhit->Branch("Nhits",    &Ncalhits,        "Nhits/I");
	tcalhit->Branch("charge",   &calhit_charge,   "charge[Nhits]/F");
	tcalhit->Branch("time",     &calhit_time,     "time[Nhits]/F");
	tcalhit->Branch("iX",       &calhit_iX,       "iX[Nhits]/I");
	tcalhit->Branch("iY",       &calhit_iY,       "iY[Nhits]/I");

    tcaltrack = new TTree("caltrack","EIC5x5Caltrack");
	tcaltrack->Branch("Ntracks", &Ncaltracks,        "Ntracks/I");
	tcaltrack->Branch("tstart",  &caltrack_tstart,   "tstart[Ntracks]/F");
	tcaltrack->Branch("tend",    &caltrack_tend,     "tend[Ntracks]/F");
	tcaltrack->Branch("Nhits",   &caltrack_Nhits,    "Nhits[Ntracks]/I");

    ttrigger = new TTree("trigger","Tridas and JANA Triggers");
	ttrigger->Branch("Ntriggerwords",           &Ntriggerwords,           "Ntriggerwords/I");
	ttrigger->Branch("triggerwords",            &triggerwords,            "triggerwords[Ntriggerwords]/i");
	ttrigger->Branch("cosmic_trigger_decision", &cosmic_trigger_decision, "cosmic_trigger_decision/s");
	ttrigger->Branch("cosmic_trigger_id",       &cosmic_trigger_id,       "cosmic_trigger_id/s");
	ttrigger->Branch("beam_trigger_decision",   &beam_trigger_decision,   "beam_trigger_decision/s");
	ttrigger->Branch("beam_trigger_id",         &beam_trigger_id,         "beam_trigger_id/s");


	gDirectory->cd();
	m_root_lock->release_lock();

}

void EIC_5x5Processor::Process(const std::shared_ptr<const JEvent> &event) {
    // LOG << "EIC_5x5Processor::Process, Event #" << event->GetEventNumber() << LOG_END;
    
    auto fa250hits       = event->Get<fa250VTPMode7Hit>();
    auto calhits         = event->Get<HallDCalHit>();
    auto caltracks       = event->Get<EIC5x5CalTrack>();

    const TridasEvent     *tridasEvent    = nullptr;
    const TriggerDecision *cosmic_trigger = nullptr;
    const TriggerDecision *beam_trigger   = nullptr;
    try{ tridasEvent     = event->GetSingle<TridasEvent>();                        } catch(...) {}  // optional
    try{ cosmic_trigger  = event->GetSingle<TriggerDecision>("EIC5x5Cal_cosmics"); } catch(...) {}  // optional
    try{ beam_trigger    = event->GetSingle<TriggerDecision>("EIC5x5Cal_beam");    } catch(...) {}  // optional

    /// Lock mutex
    m_root_lock->acquire_write_lock();
    
    Nf250hits = 0;
    for(auto fa250hit : fa250hits){
        f250hit_crate[Nf250hits]   = fa250hit->m_channel.crate;
        f250hit_slot[Nf250hits]    = fa250hit->m_channel.slot;
        f250hit_channel[Nf250hits] = fa250hit->m_channel.channel;
        f250hit_id[Nf250hits]      = fa250hit->m_channel.channel; // this is due to channels 0-8 being mapped to channels 0-8 (See TranslationTable::ReadTranslationTableEIC2023)
        f250hit_charge[Nf250hits]  = fa250hit->m_charge;
        f250hit_time[Nf250hits]    = fa250hit->m_time.count();
        if( ++Nf250hits >= MAX_f250hits ) break;
    }
    tf250hit->Fill();

    Ncalhits = 0;
    for(auto calhit : calhits){
        calhit_charge[Ncalhits]    = calhit->getHitEnergy();
        calhit_time[Ncalhits]      = calhit->getHitTime();
        calhit_iX[Ncalhits]        = calhit->getHitIX();
        calhit_iY[Ncalhits]        = calhit->getHitIY();
        if( ++Ncalhits >= MAX_calhits ) break;

    }
    tcalhit->Fill();

    Ncaltracks = 0;
    for(auto caltrack : caltracks){
        caltrack_tstart[Ncaltracks]  = caltrack->getTrackStartTime();
        caltrack_tend[Ncaltracks]    = caltrack->getTrackEndTime();
        caltrack_Nhits[Ncaltracks]   = caltrack->GetNumHits();
        if( ++Ncaltracks >= MAX_caltracks ) break;

    }
    tcaltrack->Fill();

    // Here we are being forgiving if any of the trigger objects are not present.
    Ntriggerwords = 0;
    cosmic_trigger_decision = 0xFFFF;
    cosmic_trigger_id = 0xFFFF;
    beam_trigger_decision = 0xFFFF;
    beam_trigger_id = 0xFFFF;
    if( tridasEvent ){
        for( auto w : tridasEvent->triggerWords ) {
            triggerwords[Ntriggerwords] = w;
             if( ++Ntriggerwords >= MAX_triggerwords ) break;
        }
    }
    if( cosmic_trigger ){
        cosmic_trigger_decision = cosmic_trigger->GetDecision();
        cosmic_trigger_id       = cosmic_trigger->GetID();
    }
    if( beam_trigger ){
        beam_trigger_decision = beam_trigger->GetDecision();
        beam_trigger_id       = beam_trigger->GetID();
    }
    ttrigger->Fill();

    m_root_lock->release_lock();
}

void EIC_5x5Processor::Finish() {
    // Close any resources
    LOG << "EIC_5x5Processor::Finish" << LOG_END;

    if( fout != nullptr ){
	    m_root_lock->acquire_write_lock();
        fout->Write();
        fout->Close();
        delete fout;
        fout = nullptr;
        m_root_lock->release_lock();
    }
}

