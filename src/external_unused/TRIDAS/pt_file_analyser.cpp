#include "ptfile_reader.hpp"

#include <iostream>
#include <fstream>
#include <cassert>
#include <boost/program_options.hpp>

using namespace tridas::post_trigger;

int main(int argc, char* argv[]) {
	std::string cardfile;
	std::string ptfile;
	bool write_dc = false;

	ptfile = std::string(argv[1]);

	PtFileReader<sample::uncompressed> pt_reader(ptfile);

	if (write_dc) {
		std::ofstream datacard(cardfile.c_str());
		assert(datacard && "Error opening datacard file for writing.");

		datacard << pt_reader.datacard();
	}

	std::size_t nevent = 0;

	int count = 0;

	for (PtFileReader<sample::uncompressed>::iterator it = pt_reader.begin(), et = pt_reader.end(); it != et; ++it) {
		TimeSlice<sample::uncompressed> ts = *it;
		nevent += ts.nEvents();
		std::cout << "TS " << ++count << "/" << pt_reader.nTS() << '\n' << "TS ID: " << ts.id() << '\n' << "Number of events: " << ts.nEvents() << '\n';
		for (TimeSlice<sample::uncompressed>::iterator it2 = ts.begin(); it2 != ts.end(); ++it2) {
			Event<sample::uncompressed> ev = *it2;
			std::cout <<"EV "<<ev.eventTag()<<" "<<ev.id()<<" "<<ev.startTime()<<" "<<ev.nHit()<<'\n';
			for (Event<sample::uncompressed>::iterator it3 = ev.begin(); it3 != ev.end(); ++it3) {
					std::cout << "HIT1! " << '\n';;
					Hit<sample::uncompressed> hit = (*it3);	//This is the HIT
					auto abs_time = getDFHFullTime(hit.frameHeader(0));  //absolute time in 4 ns
					std::cout<<"HIT2! "<<abs_time<<'\n';
				}

		}
	}

	std::cout<<"nevents: "<<nevent<<'\n';
	assert(pt_reader.nEvents() == nevent);
}
