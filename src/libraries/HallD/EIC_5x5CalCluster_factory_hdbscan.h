/*
 * EIC_5x5CalCluster_factory
 *
 *  Created on: Dec 23, 2019
 *      Author: celentan
 */

#pragma once

#include <JANA/JFactoryT.h>
#include <HallD/EIC_5x5CalCluster.h>

class EIC_5x5CalCluster_factory_hdbscan: public JFactoryT<EIC_5x5CalCluster> {
public:
	EIC_5x5CalCluster_factory_hdbscan();
	virtual ~EIC_5x5CalCluster_factory_hdbscan();

	// Overloaded by user Factories
	virtual void Init();
	virtual void ChangeRun(const std::shared_ptr<const JEvent> &aEvent);
	virtual void Process(const std::shared_ptr<const JEvent> &aEvent);
	static bool compareHits(const HallDCalHit* a, const HallDCalHit* b);

private:
	//I am hard-coding the indexes here, since it will always be 1 sector, 1 layer, 332 components.
	std::map<int, double> c0, c1, c2, c3, c4; //not really true, since the components are 332, but they are not continuous from 1 to 332!
	void CorrectClusterEnergy(EIC_5x5CalCluster *cluster);
};

