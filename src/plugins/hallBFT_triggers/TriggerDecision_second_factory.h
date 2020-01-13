/*
 * TriggerDecision_second_factory.h
 *
 *  Created on: Jan 13, 2020
 *      Author: celentan
 */

#ifndef SRC_PLUGINS_HALLBFT_TRIGGERS_TRIGGERDECISION_SECOND_FACTORY_H_
#define SRC_PLUGINS_HALLBFT_TRIGGERS_TRIGGERDECISION_SECOND_FACTORY_H_

#include "Trigger/TriggerDecision.h"
#include "JANA/JFactoryT.h"

class TriggerDecision_second_factory: public JFactoryT<TriggerDecision> {
public:
	TriggerDecision_second_factory();
	virtual ~TriggerDecision_second_factory();

	// Overloaded by user Factories
	virtual void Init();
	virtual void ChangeRun(const std::shared_ptr<const JEvent> &aEvent);
	virtual void Process(const std::shared_ptr<const JEvent> &aEvent);
};

#endif /* SRC_PLUGINS_HALLBFT_TRIGGERS_TRIGGERDECISION_SECOND_FACTORY_H_ */
