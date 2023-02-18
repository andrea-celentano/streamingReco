//
// Aug. 16, 2020 David Lawrence
//
// This is a trivial plugin that simply adds the
// JANA event source for reading a PT file. The
// code for the source all exists in the directory:
//
//    src/libraires/DAQ
//
// Making it a plugin allows us to use the source 
// with any jana executable. E.g.
//
//   jana -PPLUGINS=PTFileSource file.pt
//

#include <DAQ/JEventSourcePTFileGenerator.h>
#include <DAQ/JFactoryGenerator_DAQ.h>
#include <HallD/JFactoryGenerator_HallD.h>
#include <TT/JFactoryGenerator_TT.h>
#include <FT/JFactoryGenerator_FT.h>

#include <JANA/Calibrations/JCalibrationManager.h>
#define HAVE_CCDB 1
#include <JANA/Calibrations/JCalibrationGeneratorCCDB.h>
#include <JANA/Services/JGlobalRootLock.h>


extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->Add(new JEventSourcePTFileGenerator());
	app->Add(new JFactoryGenerator_DAQ());
	app->Add(new JFactoryGenerator_HallD());
	app->Add(new JFactoryGenerator_TT());
	app->Add(new JFactoryGenerator_FT());

	app->SetParameterValue("RUNTYPE", "EIC2023"); // This is used in TT/TranslationTable.cc to determine which TT to use
	auto calib_manager = std::make_shared<JCalibrationManager>();
	calib_manager->AddCalibrationGenerator(new JCalibrationGeneratorCCDB);
	japp->ProvideService(calib_manager);
	japp->ProvideService(std::make_shared<JGlobalRootLock>());
}
} // "C"

