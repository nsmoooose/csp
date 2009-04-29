#include <csp/modules/demeter/Export.h>
#include <csp/modules/demeter/DemeterTerrain.h>

using namespace csp;

extern "C" {
void DEMETER_EXPORT initModule() {
	{ static DemeterTerrain::__csp_interface_proxy instance; }
}
}
