
#ifndef _EIC3x3Cal_cosmicsTriggerProcessor_h_
#define _EIC3x3Cal_cosmicsTriggerProcessor_h_

#include <JANA/JFactoryT.h>
#include <JANA/JEvent.h>
#include <Trigger/TriggerDecision.h>


class TriggerDecision_EIC3x3Cal_cosmics_factory : public JFactoryT<TriggerDecision> {

public:

    TriggerDecision_EIC3x3Cal_cosmics_factory();
    virtual ~TriggerDecision_EIC3x3Cal_cosmics_factory() = default;

    void Init() override;
    void Process(const std::shared_ptr<const JEvent>& event) override;

    bool ENABLED;
    int MIN_TRACKS;
    int MAX_TRACKS;
};


#endif // _EIC3x3Cal_cosmicsTriggerProcessor_h_

