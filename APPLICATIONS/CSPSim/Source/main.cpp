#include "stdinc.h"
#include "SDL.h"
#include "Config.h"
#include "CSPSim.h"


int main(int argc, char *argv[])
{
	if (!openConfig("../Data/CSPSim.ini")) return 0;
        int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);

	std::ofstream logfile("CSPSim.log");
	csplog().setLogLevels(CSP_ALL, (cspDebugPriority)level);
	csplog().set_output(logfile);

	CSPSim app;

	app.Init();
	app.Run();
	app.Cleanup();
	app.Exit();

	csplog().set_output (cerr);
	logfile.close();

	return 0;
}
