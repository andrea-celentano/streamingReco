
#ifndef _EIC5x5Cal_cosmicsTriggerProcessor_h_
#define _EIC5x5Cal_cosmicsTriggerProcessor_h_

#include <JANA/JFactoryT.h>
#include <JANA/JEvent.h>
#include <Trigger/TriggerDecision.h>


class TriggerDecision_EIC5x5Cal_cosmics_factory : public JFactoryT<TriggerDecision> {

public:

    TriggerDecision_EIC5x5Cal_cosmics_factory();
    virtual ~TriggerDecision_EIC5x5Cal_cosmics_factory() = default;

    void Init() override;
    void Process(const std::shared_ptr<const JEvent>& event) override;

    bool ENABLED;
    int MIN_TRACKS;
    int MAX_TRACKS;
};


#endif // _EIC5x5Cal_cosmicsTriggerProcessor_h_

