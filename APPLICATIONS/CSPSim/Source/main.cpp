#include "Config.h"
#include "CSPSim.h"


int main(int argc, char *argv[])
{   
	if (!openConfig("../Data/CSPSim.ini")) return 0;
	
	CSPSim app;

	app.Init();
	app.Run();
	app.Cleanup();
	app.Exit();

	return 0;
}
