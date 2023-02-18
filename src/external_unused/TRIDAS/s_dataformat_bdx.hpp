#ifndef PACKAGES_DATAFORMATS_S_DATAFORMAT_BDX_HPP
#define PACKAGES_DATAFORMATS_S_DATAFORMAT_BDX_HPP

#include <cstdlib>
#include <stdint.h>

namespace tridas {
namespace bdx {

const unsigned int VERSION_DATAFORMAT = 2018091800; // prev.  2016110700

// These constants are used in struct TEHeaderInfo.
// They should be all placed in different location.
// In phase2 are #define. Some of them might be static.

const unsigned int MAX_PLUGINS_NUMBER  = 8;
const unsigned int MAX_TRIGGERS_NUMBER = 5;

const unsigned int L1TOTAL_ID = 0;
const unsigned int L1Q_ID  = 1;
const unsigned int L1SC_ID = 2;
const unsigned int L1FC_ID = 3;
const unsigned int L1RT_ID = 4;

#pragma pack(push, 1)
struct DataFrameHeader {
  // First DWORD: Sync and ID
  unsigned int PMTID     :  4; // 0-15
  unsigned int EFCMID    :  5; // 0-31
  unsigned int TowerID   :  7; // 0-127
  unsigned int SyncBytes : 16;

  // Second and third DWORD: Time
  unsigned int T4ns         : 16; // 4 nanoseconds counter
  unsigned int FrameCounter : 13; // 0-7999
  unsigned int Seconds      : 17; // 0-86399
  unsigned int Days         : 10; // BCD! 0-365 (366)
  unsigned int Years        :  8; // BCD! 0-99

  // Forth DWORD: Hit info
  unsigned int NDataSamples :  7; // Number of samples
  unsigned int FifoFull     :  1; // FIFO full flag
  unsigned int Charge       : 22;
  unsigned int FragFlag     :  1; // Fragmentation flag
  unsigned int ZipFlag      :  1; // Compression flag

  DataFrameHeader()
    :
    PMTID(0),
    EFCMID(0),
    TowerID(0),
    SyncBytes(0),
    T4ns(0),
    FrameCounter(0),
    Seconds(0),
    Days(0),
    Years(0),
    NDataSamples(0),
    FifoFull(0),
    Charge(0),
    FragFlag(0),
    ZipFlag(0)
  {}
};
#pragma pack(pop)

// The following lines are cut-and-paste from the Phase 2 version of the DataFormat.
// We need to discuss about their use in TriDAS.

typedef size_t TS_t;
typedef size_t TcpuId;

// Sample D-Word
struct Samples
{
  unsigned int samp1 :16;
  unsigned int samp0 :16;
  Samples(unsigned int samp1, unsigned int samp0):
    samp1(samp1), samp0(samp0)
  {
  }
};

// =============================================
/*

 PT Header Generale
 /---- TimeSlice 1
 /-------- TriggeredEvent 1
 /------------DataFrame 1
 /------------DataFrame 2
 /------------DataFrame ...
 /------------DataFrame n
 /-------- TriggeredEvent 2
 /------------DataFrame 1
 /------------DataFrame 2
 /------------DataFrame ...
 /------------DataFrame n

 /---- TimeSlice 2
 /-------- TriggeredEvent 1
 /------------DataFrame 1
 /------------DataFrame 2
 /------------DataFrame ...
 /------------DataFrame n
 /-------- TriggeredEvent 2
 /------------DataFrame 1
 /------------DataFrame 2
 /------------DataFrame ...
 /------------DataFrame n

 ...



 */

struct PTHeader
{
  uint32_t run_number;
  uint32_t file_number;
  uint64_t run_start_time;
  uint32_t datacard_size;
  uint32_t number_of_events;
  uint32_t number_of_timeslices;
  uint32_t effective_file_size;
};

struct TimeSliceHeader {
  TS_t TS_ID;           // ID number of the TimeSlice
  uint32_t NEvents; // Number of events in the TimeSlice
  uint32_t TS_size; // size of the whole TS data (TimeSliceHeader + events payload)
};

struct TEHeaderInfo { // this is filled e.g in the TTSBuild
  uint32_t EventTag; // Tag for HeaderE = 12081972
  uint32_t EventID; //ID - to be set by the EM
  uint32_t EventL; //total length of the TE (header included)
  uint32_t nHit; // n tot. hits
  uint64_t StartTime; // Start time in 4ns
  uint32_t TSCompleted; // flag for event belonging to an (un)completed (0)1 TS

  unsigned int nseeds[MAX_TRIGGERS_NUMBER]; // array that store the number of seeds per trigger type
  unsigned int plugin_trigtype[MAX_PLUGINS_NUMBER]; // array - seed-found flags for each kind of plugin
  unsigned int plugin_nseeds[MAX_PLUGINS_NUMBER]; // array - nseeds for each kind of plugin
};

} // ns bdx
} // ns tridas

#endif
