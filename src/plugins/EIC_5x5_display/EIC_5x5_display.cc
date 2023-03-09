
#include <JANA/JEventProcessor.h>
#include <JANA/Services/JGlobalRootLock.h>
#include <TH1D.h>
#include <TFile.h>
#include <TH2.h>

#include <HallD/HallDCalHit.h>
#include <EIC_5x5_cosmics_trigger/EIC5x5CalTrack.h>
#include <Trigger/TriggerDecision.h>

class EIC_5x5_displayProcessor: public JEventProcessor {
private:
    std::string m_tracking_alg = "genfit";
    std::shared_ptr<JGlobalRootLock> m_lock;
    TFile* dest_file;
    TDirectory* dest_dir; // Virtual subfolder inside dest_file used for this specific processor
    
    /// Declare histograms here
    TH1D* h1d_pt_reco;

public:
    EIC_5x5_displayProcessor() {
        SetTypeName(NAME_OF_THIS); // Provide JANA with this class's name
    }
    
    void Init() override {
        auto app = GetApplication();
        m_lock = app->GetService<JGlobalRootLock>();

        /// Set parameters to control which JFactories you use
        app->SetDefaultParameter("tracking_alg", m_tracking_alg);

        /// Set up histograms
        m_lock->acquire_write_lock();
        dest_file = new TFile("EIC_5x5_display.root", "recreate");  /// TODO: Acquire dest_file via either a JService or a JParameter
        dest_dir = dest_file->mkdir("EIC_5x5_display"); // Create a subdir inside dest_file for these results
        dest_file->cd();
        m_lock->release_lock();
    }

    void Process(const std::shared_ptr<const JEvent>& event) override {

        // auto fa250hits       = event->Get<fa250VTPMode7Hit>();
        auto calhits         = event->Get<HallDCalHit>();
        // auto caltracks       = event->Get<EIC5x5CalTrack>();

        // const TridasEvent     *tridasEvent    = nullptr;
        const TriggerDecision *cosmic_trigger = nullptr;
        // const TriggerDecision *beam_trigger   = nullptr;
        // try{ tridasEvent     = event->GetSingle<TridasEvent>();                        } catch(...) {}  // optional
        try{ cosmic_trigger  = event->GetSingle<TriggerDecision>("EIC5x5Cal_cosmics"); } catch(...) {}  // optional
        // try{ beam_trigger    = event->GetSingle<TriggerDecision>("HallDCal_beam");     } catch(...) {}  // optional

        if( !cosmic_trigger ) return;
        if( cosmic_trigger->GetDecision() == 0) return;


        m_lock->acquire_write_lock();

        auto save_dir = gDirectory;
        dest_dir->cd();
        
        // Create a 2D historgram representing single event
        char hname[256];
        sprintf(hname, "event%04d", event->GetEventNumber() );
        auto h = new TH2F(hname, hname, 5, 0.5, 5.5, 5, 0.5, 5.5);
        for( auto hit : calhits) h->Fill( hit->getHitIX()+1, hit->getHitIY()+1, hit->getHitEnergy());

        save_dir->cd();
        m_lock->release_lock();
    }

    void Finish() override {
        // TODO: If we did not create this file then we should not delete it
        dest_file->Write();
        delete dest_file;
        dest_file = nullptr;
    }
};
    
extern "C" {
    void InitPlugin(JApplication *app) {
        InitJANAPlugin(app);
        app->Add(new EIC_5x5_displayProcessor);
    }
}
    
