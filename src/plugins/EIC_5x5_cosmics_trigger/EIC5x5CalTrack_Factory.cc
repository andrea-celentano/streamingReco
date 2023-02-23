//
// This factory is used to identify tracks within a time frame/slice.
// For each track it identifies, a EIC5x5CalTrack object is created.
// Tracks are identified by looking for known hit patterns in the
// calorimeter blocks that are within CALTRACK:COINICIDENCE_WINDOW
// ns of each other. The default time window is 100 ns.
//
//


#include "EIC5x5CalTrack_Factory.h"

#include <JANA/JEvent.h>

/// pattern = bit pattern of all previous rows
/// irow = index of row to add (1-4)
/// icol_prev = column of hit in row before the one we are adding 
void EIC5x5CalTrack_Factory::add_next_row( int dir, uint32_t pattern, uint32_t irow, int32_t icol){


	// If the column or row is outside the range, then consider
	// the track to have left the detector. Check that the pattern
	// has the minimum number of hits and if so, add the pattern.
	if( icol<0 || icol>=GRID_WIDTH || irow>=GRID_HEIGHT ){
		int Nhits = 0;
		for(int i=0; i<GRID_WIDTH*GRID_HEIGHT; i++ )
			if(pattern & (0x1<<i)) Nhits++;
		
		if(Nhits >= MIN_HITS_PER_TRACK) patterns.insert(pattern);
		// _DBG_<<"irow="<<irow<<" icol="<<icol<<" Nhits="<<Nhits<<std::endl;
		return;
	}

	pattern |= (0x1 <<(irow*GRID_WIDTH + icol));

	// Recursively call ourselves to add hits from the next row.
	// If we were called for a pattern already moving to the left
	// or right, then only check for blocks that are either straight
	// down or in that direction.  
	if( dir == DIRECTION_LEFT ){
		add_next_row( DIRECTION_LEFT, pattern, irow+1, icol-1);
		add_next_row( DIRECTION_LEFT, pattern, irow+1, icol);
	}
		if( dir == DIRECTION_ANY ){
		add_next_row( DIRECTION_LEFT, pattern, irow+1, icol-1);
		add_next_row( DIRECTION_ANY, pattern, irow+1, icol);
		add_next_row( DIRECTION_RIGHT, pattern, irow+1, icol+1);
	}
	if( dir == DIRECTION_RIGHT ){
		add_next_row( DIRECTION_RIGHT, pattern, irow+1, icol);
		add_next_row( DIRECTION_RIGHT, pattern, irow+1, icol+1);
	}

}

//-----------------------------------
// Init
//-----------------------------------
void EIC5x5CalTrack_Factory::Init() {

	// Loop over every 
	LOG<<"Building patterns for cosmic tracks in " <<GRID_WIDTH<<"x"<<GRID_HEIGHT<<" calorimeter ..." << LOG_END;
	for(uint32_t irow=0; irow<GRID_HEIGHT; irow++){
		for(int32_t icol=0; icol<GRID_WIDTH; icol++){

			// Patterns can start anywhere on top row, but after
			// that, they must start from edge.
			if( (irow>0) && (icol!=0) && (icol!=(GRID_WIDTH-1))) continue;

			// Start new track pattern
			add_next_row( DIRECTION_ANY, 0x0, irow, icol);
		}

		// no need to check for tracks starting so close to bottom
		// that they can't include minimum number of hits.
		if( irow > (GRID_HEIGHT - MIN_HITS_PER_TRACK) ) break; 
	}
	LOG<<patterns.size()<<" patterns determined." << LOG_END;
}

//-----------------------------------
// ChangeRun
//-----------------------------------
void EIC5x5CalTrack_Factory::ChangeRun(const std::shared_ptr<const JEvent> &event) {

	// Set the default value for the coincidence window and 
	// declare it asa parameter so it can be changed at run
	// time.
	t_window = 100.0;
	GetApplication()->GetJParameterManager()->SetDefaultParameter("CALTRACK:COINICIDENCE_WINDOW", t_window, "Coincidence window width in ns for identifying cosmic tracks in calorimeter");
}

//-----------------------------------
// Process
//-----------------------------------
void EIC5x5CalTrack_Factory::Process(const std::shared_ptr<const JEvent> &event) {

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
			assert(row < 5);
			assert(col < 5);
			pixels[row][col] = true;
		}
		
		// Check if hits match a track pattern
		if (match_track_pattern(pixels) != -1) {

			// Found a track! Publish outputs
			Insert( new EIC5x5CalTrack(hits.begin()+i, hits.begin()+j) );
		}

	} // end loop over i
}

//-----------------------------------
// match_track_pattern
//-----------------------------------
int EIC5x5CalTrack_Factory::match_track_pattern(bool pixel_grid[GRID_HEIGHT][GRID_WIDTH]) {
	//    +--+--+--+--+--+
	//    | 1| 2| 3| 4| 5|
	//    +--+--+--+--+--+
	//    | 6| 7| 8| 9|10|
	//    +--+--+--+--+--+
	//    |11|12|13|14|15|
	//    +--+--+--+--+--+
	//    |16|17|18|19|20|
	//    +--+--+--+--+--+
	//    |21|22|23|24|25|
	//    +--+--+--+--+--+

	// Convert the pixel grid into a bitset so that we can quickly compare all acceptable patterns
	uint32_t bitset = 0;
	for (uint32_t row=0; row<GRID_HEIGHT; row++) {
		for (uint32_t col=0; col<GRID_WIDTH; col++) {
			if(pixel_grid[row][col]){
				bitset |= ((uint32_t)1 << (row*GRID_WIDTH + col));
			}
		}
	}

	// // Patterns are all known ahead of time
	// const uint32_t patterns[13] = {0b100100100, 0b010010010, 0b001001001,  // Verticals
	//                                0b100010001, 0b001010100, // Diagonals
	//                                0b100010010, 0b010100100, 0b010001001, 0b001010010,
	//                                0b001010001, 0b100010100, 0b010100010, 0b010001010};

	// Return true on the first matching pattern we find
	uint32_t idx = 0;
	for(auto pattern : patterns) {
		if ((bitset & pattern) == pattern) return idx;
		idx++;
	}
	return -1;
}

