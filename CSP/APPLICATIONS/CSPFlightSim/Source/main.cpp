#include "stdinc.h"


#include "SDL.h"

#include "CSPFlightSim.h"

CSPFlightSim * g_pCSPFlightSim;

int main(int argc, char *argv[])
{

	// config file found!
	Config.ReadConfigFile("../Data/CSPFlightSim.ini");

    int level = Config.GetInteger("LoggingLevel");

    std::ofstream logfile("CSPFlightSim.log");
    csplog().setLogLevels( CSP_ALL, (cspDebugPriority)level );
    csplog().set_output (logfile);

    CSPFlightSim app;
    g_pCSPFlightSim = &app;
    app.Init();
    app.Run();
    app.Cleanup();
    app.Exit();

    csplog().set_output (cerr);
    logfile.close();


    return 0;

}