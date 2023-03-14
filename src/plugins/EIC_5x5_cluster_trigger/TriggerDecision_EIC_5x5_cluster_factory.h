
#pragma once

#include <JANA/JFactoryT.h>
#include <JANA/JEvent.h>
#include <Trigger/TriggerDecision.h>


class TriggerDecision_EIC_5x5_cluster_factory : public JFactoryT<TriggerDecision> {

public:

    TriggerDecision_EIC_5x5_cluster_factory();
    virtual ~TriggerDecision_EIC_5x5_cluster_factory() = default;

    void Init() override;
    void Process(const std::shared_ptr<const JEvent>& event) override;

    bool ENABLED;
};


