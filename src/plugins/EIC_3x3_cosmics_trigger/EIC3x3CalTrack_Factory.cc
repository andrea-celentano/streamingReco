//
// This factory is used to identify tracks within a time frame/slice.
// For each track it identifies, a EIC3x3CalTrack object is created.
// Tracks are identified by looking for known hit patterns in the
// calorimeter blocks that are within CALTRACK:COINICIDENCE_WINDOW
// ns of each other. The default time window is 100 ns.
//
//


#include "EIC3x3CalTrack_Factory.h"

#include <JANA/JEvent.h>

//-----------------------------------
// Init
//-----------------------------------
void EIC3x3CalTrack_Factory::Init() {

}

//-----------------------------------
// ChangeRun
//-----------------------------------
void EIC3x3CalTrack_Factory::ChangeRun(const std::shared_ptr<const JEvent> &event) {

	// Set the default value for the coincidence window and 
	// declare it asa parameter so it can be changed at run
	// time.
	t_window = 100.0;
	GetApplication()->GetJParameterManager()->SetDefaultParameter("CALTRACK:COINICIDENCE_WINDOW", t_window, "Coincidence window width in ns for identifying cosmic tracks in calorimeter");
}

//-----------------------------------
// Process
//-----------------------------------
void EIC3x3CalTrack_Factory::Process(const std::shared_ptr<const JEvent> &event) {

	// We need to identify cosmic tracks in the calorimeter
	// by both hit pattern and time. We do this by first ordering
	// the hits by time and then looking for sequences within
	// t_window of each other that fit one of the predefined patterns.

	// Get list of hits
	auto hits = event->Get<HallDCalHit>();

	if(hits.size()>=3) _DBG_<<"hits.size()=" << hits.size() << std::endl;

	// Sort hits in order of time
	auto mylambda = [](const HallDCalHit *a, const HallDCalHit *b ){return a->getHitTime() < b->getHitTime();};
	std::sort( hits.begin(), hits.end(), mylambda);

	// Loop over hits. For each, find list of hits within
	// the t_window ns following it.
	for( uint32_t i=0; (i+2)< hits.size(); i++){
	
		// time of first (earliest) hit in this sequence
		auto t_start = hits[i]->getHitTime();
		
		// find latest hit still within t_window of t_start
		auto j = i+2; // we need minimum of 3 hits
		for( ; j<hits.size(); j++){
			auto delta_t = hits[j]->getHitTime() - t_start;
			if( delta_t > t_window ) break;
		}
		
		// At this point j indexes the hit just after the
		// last one to fall within t_window of t_start hit.
		// Clear the pixel grid and fill it using the hits
		// from i to j.
		
		// Initialize pixel grid
		bool pixels[GRID_HEIGHT][GRID_WIDTH];
		for (int i=0; i<GRID_HEIGHT; ++i) {
			for (int j=0; j<GRID_WIDTH; ++j) {
				pixels[i][j] = false;
			}
		}
		
		// Update pixel grid with hit information
		for(int ihit=i; ihit<j; ihit++) {
			auto row = hits[ihit]->getHitIY();
			auto col = hits[ihit]->getHitIX();
			assert(row >= 0);
			assert(col >= 0);
			assert(row < 3);
			assert(col < 3);
			pixels[row][col] = true;
		}
		
		// Check if hits match a track pattern
		if (match_track_pattern(pixels) != -1) {

			// Found a track! Publish outputs
			Insert( new EIC3x3CalTrack(hits.begin()+i, hits.begin()+j) );
		}

	} // end loop over i
}

//-----------------------------------
// match_track_pattern
//-----------------------------------
int EIC3x3CalTrack_Factory::match_track_pattern(bool pixel_grid[GRID_HEIGHT][GRID_WIDTH]) {
	//    +-+-+-+
	//    |1|2|3|
	//    +-+-+-+
	//    |4|5|6|
	//    +-+-+-+
	//    |7|8|9|
	//    +-+-+-+

	// Convert the pixel grid into a bitset so that we can quickly compare all acceptable patterns
	uint32_t bitset = 0;
	for (uint32_t row=0; row<GRID_HEIGHT; row++) {
		for (uint32_t col=0; col<GRID_WIDTH; col++) {
			if(pixel_grid[row][col]){
				bitset |= ((uint32_t)1 << (row*3 + col));
			}
		}
	}

	// Patterns are all known ahead of time
	const uint32_t patterns[13] = {0b100100100, 0b010010010, 0b001001001,  // Verticals
	                               0b100010001, 0b001010100, // Diagonals
	                               0b100010010, 0b010100100, 0b010001001, 0b001010010,
	                               0b001010001, 0b100010100, 0b010100010, 0b010001010};

	// Return true on the first matching pattern we find
	for (uint32_t i=0; i<13; ++i) {
		if ((bitset & patterns[i]) != patterns[i]) {
			return i;
		}
	}
	return -1;
}

