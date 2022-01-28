
#include "HallD/HallDCalHit.h"
#include "HallDCal_cosmicsProcessor.h"
#include <JANA/JLogger.h>
#include "JANA/Services/JGlobalRootLock.h"
#include "JANA/JApplication.h"

#include "TFile.h"
#include "TH1D.h"
#include "TFolder.h"
#include "TROOT.h"

HallDCal_cosmicsProcessor::HallDCal_cosmicsProcessor() {
    SetTypeName(NAME_OF_THIS); // Provide JANA with this class's name
}

void HallDCal_cosmicsProcessor::Init() {
    LOG << "HallDCal_cosmicsProcessor::Init" << LOG_END;
    // Open TFiles, set up TTree branches, etc


	m_root_lock = japp->GetService<JGlobalRootLock>();

	m_root_lock->acquire_write_lock();
	if (hQ[0] != NULL) {
		m_root_lock->release_lock();
		return;
	}

    // Optionally open output file
	if (japp->GetJParameterManager()->Exists("HallDCal_cosmics:output_file") == true) {
	    std::string outFileName;
		japp->GetJParameterManager()->GetParameter("HallDCal_cosmics:output_file", outFileName);
		std::cout << "JEventProcessor_HallDCal_cosmics: output file name: " << this << " " << outFileName << std::endl;
		fout = new TFile(outFileName.c_str(), "recreate");
    }else{
        // If not opening our own output root file then create a
        // directory for all of our root objects
        gDirectory->mkdir("HallDCal_cosmics")->cd();
    }

	for (int ii=0;ii<9;ii++){
		hQ[ii]=new TH1D(Form("hQ_%i",ii),Form("hQ_%i",ii),1000,0,1E5);
	}

	tree1 = new TTree("tree1","Reconst ntuple");
	tree1->Branch("ch_1",&ch_1,"ch_1/F");         tree1->Branch("ch_2",&ch_2,"ch_2/F");      tree1->Branch("ch_3",&ch_3,"ch_3/F");
	tree1->Branch("ch_4",&ch_4,"ch_4/F");         tree1->Branch("ch_5",&ch_5,"ch_5/F");      tree1->Branch("ch_6",&ch_6,"ch_6/F");
	tree1->Branch("ch_7",&ch_7,"ch_7/F");         tree1->Branch("ch_8",&ch_8,"ch_8/F");      tree1->Branch("ch_9",&ch_9,"ch_9/F");

	tree2 = new TTree("tree2","Reconst ntuple2");
	tree2->Branch("E_1",&E_1,"E_1/F");         tree2->Branch("E_2",&E_2,"E_2/F");      tree2->Branch("E_3",&E_3,"E_3/F");
	tree2->Branch("E_4",&E_4,"E_4/F");         tree2->Branch("E_5",&E_5,"E_5/F");      tree2->Branch("E_6",&E_6,"E_6/F");
	tree2->Branch("E_7",&E_7,"E_7/F");         tree2->Branch("E_8",&E_8,"E_8/F");      tree2->Branch("E_9",&E_9,"E_9/F");
	tree2->Branch("dt_1",&dt_1,"dt_1/F");      tree2->Branch("dt_2",&dt_2,"dt_2/F");   tree2->Branch("dt_3",&dt_3,"dt_3/F");
	tree2->Branch("dt_4",&dt_4,"dt_4/F");      tree2->Branch("dt_5",&dt_5,"dt_5/F");   tree2->Branch("dt_6",&dt_6,"dt_6/F");
	tree2->Branch("dt_7",&dt_7,"dt_7/F");      tree2->Branch("dt_8",&dt_8,"dt_8/F");   tree2->Branch("dt_9",&dt_9,"dt_9/F");

	tree3 = new TTree("tree3","Reconst ntuple3");
	tree3->Branch("Nhits",&Nhits,"Nhits/I");
	tree3->Branch("id",&id_all,"id[Nhits]/I");
	tree3->Branch("E",&E_all,"E[Nhits]/F");
	tree3->Branch("dt",&dt_all,"dt[Nhits]/F");
	tree3->Branch("Esum",&Esum_all,"Esum/F");
	tree3->Branch("x_avg",&x_avg,"x_avg/F");
	tree3->Branch("y_avg",&y_avg,"y_avg/F");


	gDirectory->cd();
	m_root_lock->release_lock();

}

void HallDCal_cosmicsProcessor::Process(const std::shared_ptr<const JEvent> &event) {
    LOG << "HallDCal_cosmicsProcessor::Process, Event #" << event->GetEventNumber() << LOG_END;
    
    /// Do everything we can in parallel
    /// Warning: We are only allowed to use local variables and `event` here
    //auto hits = event->Get<Hit>();

    auto hits = event->Get<HallDCalHit>();

    /// Lock mutex
    m_root_lock->acquire_write_lock();
    

    for (auto hit : hits){
    	int iX=hit->m_channel.iX;
    	int iY=hit->m_channel.iY;
    	int id=iX*3+iY;
    	double Q=hit->getHitEnergy();
        
    	hQ[id]->Fill(Q);
        
        // Look for hit in central crystal
        // DL - Energy seems highest for id==3 so assume that is central crystal
        if( id == 4 ){
            auto t_center = hit->getHitTime();

            // Pedestal subtracted integrals for hits within
            // 100ns of the time the central detector was hit
            ch_1=ch_2=ch_3=ch_4=ch_5=ch_6=ch_7=ch_8=ch_9=0;

            // Pedestal subtracted integrals for closest hit
            // in time with the central detector
            for(int i=0; i<9; i++){
                *(ch[i]) = 0.0;
                *( E[i]) = 0.0;
                *(dt[i]) = 1.0E6;
            }
            
            // Loop over all hits, recording those close in time with the
            // current central detector hit
            for (auto hit2 : hits){
            
                auto t_hit = hit2->getHitTime();

    	        int iX2=hit2->m_channel.iX;
    	        int iY2=hit2->m_channel.iY;
    	        int id2=iX2*3+iY2;
    	        Float_t Q2=hit2->getHitEnergy();
                Float_t delta_t = t_center - t_hit;
                
                // Record closest hit in time with central detector
                if( fabs(delta_t) < fabs(*dt[id2])) {
                    *( E[id2]) = Q2;         // for tree2
                    *(dt[id2]) = delta_t;    // for tree2
                    if( fabs(delta_t) > 100.0 ){
                        *( ch[id2]) = Q2;    // for tree1
                    }
                }

            }
            
            // Copy all of the closest hits in time to the variables for tree3
            Nhits = 0;
            Esum_all = 0.0;
            x_avg = 0.0;
            y_avg = 0.0;
            for(int i=0; i<9; i++){
                if( *(dt[i]) >= 1.0E6 ) continue;
                id_all[Nhits] = i+1;
                E_all[Nhits]  = *(E[i]);
                dt_all[Nhits] = *(dt[i]);
                Nhits++;
                
                Esum_all += *(E[i]);
                x_avg += *(E[i])*(Float_t)(i/3);
                y_avg += *(E[i])*(Float_t)(i%3);
            }
            if( Esum_all > 0.0 ){
                x_avg /= Esum_all;
                y_avg /= Esum_all;
            }
            
            // Fill all trees
            tree1->Fill();
            tree2->Fill();
            tree3->Fill();
        }

    }


    m_root_lock->release_lock();
}

void HallDCal_cosmicsProcessor::Finish() {
    // Close any resources
    LOG << "HallDCal_cosmicsProcessor::Finish" << LOG_END;

    if( fout != nullptr ){
	    m_root_lock->acquire_write_lock();
        fout->Write();
        fout->Close();
        delete fout;
        fout = nullptr;
        m_root_lock->release_lock();
    }
}

