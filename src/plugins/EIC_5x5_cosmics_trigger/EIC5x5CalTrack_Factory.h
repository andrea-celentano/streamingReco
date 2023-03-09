
#ifndef _EIC5x5CalTrack_Factory_h_
#define _EIC5x5CalTrack_Factory_h_

#include <JANA/JFactoryT.h>

#include "EIC5x5CalTrack.h"

class EIC5x5CalTrack_Factory : public JFactoryT<EIC5x5CalTrack> {

    enum{
        DIRECTION_ANY,
        DIRECTION_LEFT,
        DIRECTION_RIGHT
    };

    // Insert any member variables here
	static const size_t GRID_HEIGHT = 5;
	static const size_t GRID_WIDTH  = 5;
    
    size_t MIN_HITS_PER_TRACK;
	
	double t_window; // (see EIC5x5CalTrack_Factory::ChangeRun)

    std::set<uint32_t> patterns;

public:
    // EIC5x5CalTrack_Factory() : JFactoryT<EIC5x5CalTrack>(NAME_OF_THIS) {};
    void Init() override;
    void ChangeRun(const std::shared_ptr<const JEvent> &event) override;
    void Process(const std::shared_ptr<const JEvent> &event) override;

    void add_next_row(  int dir, uint32_t pattern, uint32_t irow, int32_t icol );
	int match_track_pattern(bool pixel_grid[GRID_HEIGHT][GRID_WIDTH]);
};

#endif // _EIC5x5CalTrack_Factory_h_
