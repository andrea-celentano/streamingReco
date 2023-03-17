
#pragma once

#include <JANA/JEventProcessor.h>

#include "TTree.h"


class TH1D;
class JGlobalRootLock;
class EIC_5x5Processor : public JEventProcessor {


public:

    EIC_5x5Processor();
    virtual ~EIC_5x5Processor() = default;

    void Init() override;
    void Process(const std::shared_ptr<const JEvent>& event) override;
    void Finish() override;

    static const Int_t MAX_f250hits=100;
    TTree *tf250hit;
    Int_t Nf250hits;
    Int_t f250hit_crate[MAX_f250hits];
    Int_t f250hit_slot[MAX_f250hits];
    Int_t f250hit_channel[MAX_f250hits];
    Int_t f250hit_id[MAX_f250hits];
    Float_t f250hit_charge[MAX_f250hits];  // pedestal corrected by tridas
    Float_t f250hit_time[MAX_f250hits];    // relative to earliest hit that tridas associated with event

    static const Int_t MAX_calhits=100;
    TTree *tcalhit;
    Int_t Ncalhits;
    Float_t calhit_charge[MAX_calhits];
    Float_t calhit_time[MAX_calhits];
    Int_t calhit_iX[MAX_calhits];        // From translation table
    Int_t calhit_iY[MAX_calhits];        // From translation table

    static const Int_t MAX_caltracks=100;
    TTree *tcaltrack;
    Int_t Ncaltracks;
    Float_t caltrack_tstart[MAX_caltracks];
    Float_t caltrack_tend[MAX_caltracks];
    Int_t caltrack_Nhits[MAX_caltracks];

    static const Int_t MAX_calclusters=10;
    TTree *tcalcluster;
    Int_t Ncalclusters;
    Int_t calcluster_good[MAX_calclusters];
    Int_t calcluster_size[MAX_calclusters];
    Float_t calcluster_energy[MAX_calclusters];
    Float_t calcluster_fullenergy[MAX_calclusters];
    Float_t calcluster_seedenergy[MAX_calclusters];
    Float_t calcluster_time[MAX_calclusters];
    Float_t calcluster_X[MAX_calclusters];
    Float_t calcluster_Y[MAX_calclusters];
    Float_t calcluster_Z[MAX_calclusters];
    Float_t calcluster_XX[MAX_calclusters];
    Float_t calcluster_YY[MAX_calclusters];


    static const Int_t MAX_triggerwords=100;
    TTree *ttrigger;
    Int_t Ntriggerwords;
    UInt_t triggerwords[MAX_triggerwords];  // From TridasEvent::triggerwords (i.e. recorded by tridas during acquistion)
    UShort_t cosmic_trigger_decision;       // From decision calculated when this plugin is run (i.e. after acquisition)
    UShort_t cosmic_trigger_id;             //                                     |
    UShort_t beam_trigger_decision;         //                                     |
    UShort_t beam_trigger_id;               //                                     |
    UShort_t cluster_trigger_decision;      //                                     |
    UShort_t cluster_trigger_id;            //                                     V

    
    TFile *fout=nullptr;
    

    std::shared_ptr<JGlobalRootLock> m_root_lock;
};


