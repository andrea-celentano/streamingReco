

#include <JANA/JApplication.h>
#include <JANA/JFactoryGenerator.h>

#include "EIC_3x3Processor.h"

extern "C" {
void InitPlugin(JApplication* app) {

    // This code is executed when the plugin is attached.
    // It should always call InitJANAPlugin(app) first, and then do one or more of:
    //   - Read configuration parameters
    //   - Register JFactoryGenerators
    //   - Register JEventProcessors
    //   - Register JEventSourceGenerators (or JEventSources directly)
    //   - Register JServices

    InitJANAPlugin(app);

    LOG << "Loading EIC_3x3" << LOG_END;
    app->Add(new EIC_3x3Processor);
    // Add any additional components as needed
}
}

