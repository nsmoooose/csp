#include "F16_FCS.cpp"


namespace f16 {

class DetailedModel: public Model {
public:

	DetailedModel(): Model("DetailedModel") {}

	void init() {
		addSystem(new FCS());
		testdata();
		bindSystems();
		MethodChannel< SigC::Signal0<void> >::CRef dualDataFailure;
		dualDataFailure = getBus()->getChannel("Systems.FCS.dualDataFailure()");
		assert(dualDataFailure.valid());
		dualDataFailure->call()();
	}

	void testdata() {
		Bus::Ref bus = getBus();
		bus->registerChannel(DataChannel<double>::newLocal("FlightControls.PitchInput", 0.0));
		bus->registerChannel(DataChannel<double>::newLocal("FlightControls.RollInput", 0.0));
		bus->registerChannel(DataChannel<double>::newLocal("FlightControls.YawInput", 0.0));
		bus->registerChannel(DataChannel<double>::newLocal("FlightControls.PitchTrim", 0.0));
		bus->registerChannel(DataChannel<double>::newLocal("FlightControls.RollTrim", 0.0));
		bus->registerChannel(DataChannel<double>::newLocal("FlightControls.YawTrim", 0.0));
		bus->registerChannel(DataChannel<double>::newLocal("AirData.Alpha", 0.0));
		bus->registerChannel(DataChannel<double>::newLocal("AirData.qBar", 1.0));
		bus->registerChannel(DataChannel<double>::newLocal("AirData.Ps", 1.0));
		bus->registerChannel(DataChannel<Vector3>::newLocal("AirData.AngularVelocity", Vector3::ZERO));
		bus->registerChannel(DataChannel<Vector3>::newLocal("AirData.Acceleration", Vector3::ZERO));
		bus->registerChannel(DataChannel<Quat>::newLocal("AirData.Orientation", Quat::IDENTITY));
		bus->registerChannel(DataChannel<bool>::newLocalPush("Configuration.WOW", false));
		bus->registerChannel(DataChannel<bool>::newLocalPush("Configuration.LockLEF", false));
		bus->registerChannel(DataChannel<bool>::newLocalPush("Configuration.AirRefuelDoor", false));
		bus->registerChannel(DataChannel<bool>::newLocalPush("Configuration.AltFlapsExtend", false));
		bus->registerChannel(DataChannel<bool>::newLocalPush("Configuration.LGHandleDown", false));
		bus->registerChannel(DataChannel<bool>::newLocalPush("Configuration.CATIII", false));
		bus->registerChannel(DataChannel<bool>::newLocalPush("Configuration.MPO", false));
	}
};

} // namespace f16

int main() {
	f16::DetailedModel m;
	m.init();
	float t = 0.0;
	for (int i = 0; i < 100; i++) {
		std::cout << t << ":\n";
		m.update(0.02);
		t += 0.02;
	}
	return 0;
}


