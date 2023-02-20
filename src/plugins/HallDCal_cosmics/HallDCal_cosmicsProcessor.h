
#ifndef _HallDCal_cosmicsProcessor_h_
#define _HallDCal_cosmicsProcessor_h_

#include <JANA/JEventProcessor.h>

#include "TTree.h"


class TH1D;
class JGlobalRootLock;
class HallDCal_cosmicsProcessor : public JEventProcessor {


    
public:

    HallDCal_cosmicsProcessor();
    virtual ~HallDCal_cosmicsProcessor() = default;

    void Init() override;
    void Process(const std::shared_ptr<const JEvent>& event) override;
    void Finish() override;

    TH1D *hQ[9];

    TTree *tree0;
    Int_t raw_crate;
    Int_t raw_slot;
    Int_t raw_channel;
    Int_t raw_id;
    Float_t raw_charge;  // pedestal corrected by tridas
    Float_t raw_time;    // relative to earliest hit that tridas associated with event

	TTree *tree1;
	Float_t ch_1, ch_2,ch_3,ch_4,ch_5,ch_6,ch_7,ch_8,ch_9;
    Float_t* ch[9]={&ch_1, &ch_2, &ch_3, &ch_4, &ch_5, &ch_6, &ch_7, &ch_8, &ch_9}; // make it easy to access above via id
    
    TTree *tree2;
	Float_t E_1, E_2,E_3,E_4,E_5,E_6,E_7,E_8,E_9;
	Float_t dt_1, dt_2,dt_3,dt_4,dt_5,dt_6,dt_7,dt_8,dt_9;
    Float_t* E[9]={&E_1, &E_2, &E_3, &E_4, &E_5, &E_6, &E_7, &E_8, &E_9};           // make it easy to access above via id
    Float_t* dt[9]={&dt_1, &dt_2, &dt_3, &dt_4, &dt_5, &dt_6, &dt_7, &dt_8, &dt_9}; // make it easy to access above via id

    TTree *tree3;
    Int_t Nhits;
    Int_t id_all[9];
    Float_t E_all[9];
    Float_t dt_all[9];
    Float_t Esum_all;
    Float_t x_avg;
    Float_t y_avg;
    
    TFile *fout=nullptr;
    

    std::shared_ptr<JGlobalRootLock> m_root_lock;
};


#endif // _HallDCal_cosmicsProcessor_h_

