#include "Config.h"
#include "CSPSim.h"


int main(int argc, char *argv[])
{   
	if (!openConfig("../Data/CSPSim.ini")) return 0;
	
	CSPSim app;

	app.init();
	app.run();
	app.cleanup();
	app.quit();

	return 0;
}
