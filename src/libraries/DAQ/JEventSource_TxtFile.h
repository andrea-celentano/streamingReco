//
// Created by Nathan Brei on 2019-12-15.
//

#ifndef JANA2_TxtFileEventSource_H
#define JANA2_TxtFileEventSource_H

#include <JANA/JEventSource.h>
#include <JANA/Services/JEventGroupTracker.h>
#include <JANA/JEvent.h>

#include "DAQ/TridasEvent.h"
//TRIDAS headers
#include <tools/ptfile_reader.hpp>
using namespace tridas::post_trigger;

class JEventSourceTxtFile: public JEventSource {

	int curRunNumber, curEventNumber, curEventType;
	std::ifstream file;
	int idx;
public:

	JEventSourceTxtFile(std::string res_name, JApplication* app);
	virtual ~JEventSourceTxtFile(){};
	// A description of this source type must be provided as a static member
	static std::string GetDescription(void) {
		return "Event source for txt file";
	}

	void Open(void);
	void GetEvent(std::shared_ptr<JEvent>);
	//bool GetObjects(const std::shared_ptr<const JEvent>& aEvent, JFactory* aFactory);


};

//TODO: where to place eventually the following?
//   template<> double JEventSourceGeneratorT<MyType>::CheckOpenable(std::string source) { ... }

#endif //JANA2_TxtFileEventSource_H
