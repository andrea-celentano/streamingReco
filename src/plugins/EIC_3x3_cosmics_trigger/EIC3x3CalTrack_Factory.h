
#ifndef _EIC3x3CalTrack_Factory_h_
#define _EIC3x3CalTrack_Factory_h_

#include <JANA/JFactoryT.h>

#include "EIC3x3CalTrack.h"

class EIC3x3CalTrack_Factory : public JFactoryT<EIC3x3CalTrack> {

    // Insert any member variables here
	static const size_t GRID_HEIGHT = 3;
	static const size_t GRID_WIDTH  = 3;
	
	double t_window; // (see EIC3x3CalTrack_Factory::ChangeRun)

public:
    // EIC3x3CalTrack_Factory() : JFactoryT<EIC3x3CalTrack>(NAME_OF_THIS) {};
    void Init() override;
    void ChangeRun(const std::shared_ptr<const JEvent> &event) override;
    void Process(const std::shared_ptr<const JEvent> &event) override;

	static int match_track_pattern(bool pixel_grid[GRID_HEIGHT][GRID_WIDTH]);
};

#endif // _EIC3x3CalTrack_Factory_h_
