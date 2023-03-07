
// This plugin provides the factories from
// the DAQ, TT, HallD libraries. It should
// be included in the plugins list for jtridas 

#include <DAQ/JFactoryGenerator_DAQ.h>
#include <TT/JFactoryGenerator_TT.h>
#include <HallD/JFactoryGenerator_HallD.h>


extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->Add(new JFactoryGenerator_DAQ());
	app->Add(new JFactoryGenerator_TT());
	app->Add(new JFactoryGenerator_HallD());
}
} // "C"

