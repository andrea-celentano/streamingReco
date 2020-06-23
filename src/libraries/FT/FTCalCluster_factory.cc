/*
 * faWaveboardClusterfactory.cc
 *
 *  Created on: Dec 23, 2019
 *      Author: celentan
 */

#include "FTCalCluster_factory.h"
#include "JANA/JEvent.h"
#include "FT/FTCalHit.h"
#include "FT/FTCalCluster.h"
#include <vector>
#include <iostream>
#include <functional>

//Parametri clustering:
double time_min = -1;
double time_max = 30;

//this will set the hits in DESCENDING order wrt energy
bool FTCalCluster_factory::compareHits(const FTCalHit* a, const FTCalHit* b) {
	return (a->getHitEnergy() > b->getHitEnergy());
}

FTCalCluster_factory::FTCalCluster_factory() {
	// TODO Auto-generated constructor stub

}

FTCalCluster_factory::~FTCalCluster_factory() {
	// TODO Auto-generated destructor stub
}

void FTCalCluster_factory::Init() {

}
void FTCalCluster_factory::ChangeRun(const std::shared_ptr<const JEvent> &aEvent) {

}
void FTCalCluster_factory::Process(const std::shared_ptr<const JEvent> &aEvent) {

	//Loop su gli hits di un evento
	std::vector<const FTCalHit*> hits = aEvent->Get<const FTCalHit>();

	std::sort(hits.begin(), hits.end(), FTCalCluster_factory::compareHits);
	std::vector<FTCalCluster*> clusters;

//	cout << "Hits size " << hits.size() << endl;
//	for (auto hit : hits) {
//		cout << "Hit Energy " << hit->getHitEnergy() << endl;
//	}
//	cout << "Hits end! " << endl << endl;
//	auto eventNumber = aEvent->GetEventNumber();
//	if (eventNumber == 333)
//		cin.get();
//	cout << "Event Number " << endl;
	for (auto hit : hits) {
		bool flag = false;
		if (flag == false) {
			for (int j = 0; j < clusters.size(); j++) {
				FTCalCluster* cluster = clusters[j];
				if (cluster->containsHit(hit, time_min, time_max)) {
					cluster->push_hit(hit);
					flag = true;
					break;
				}
			}
		}
		if (flag == false) {
			FTCalCluster *cluster = new FTCalCluster(clusters.size());
			cluster->push_hit(hit);
			clusters.push_back(cluster);
		}
	}

	for (int i = 0; i < clusters.size(); i++) {
		//Idea: since this factory is responsible for creating the FTCalClusters,
		//we do once the calculation of ALL quantities of interest here,
		//then the "get" methods just return the computed values.
		clusters[i]->computeCluster();
		//std::cout <<"Is good cluster? " <<clusters[i]->isGoodCluster() <<" cluster size is " <<clusters[i]->getClusterSize() <<std::endl;
		if (clusters[i]->isGoodCluster() == true) {
			mData.push_back(clusters[i]);
		} else {
			delete clusters[i];
		}
	}
}
