#include <csp/cspsim/RegisterObjectInterfaces.h>

// Objects to register.
#include <csp/cspsim/ai/AircraftAgent.h>
#include <csp/cspsim/AircraftObject.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/AnimationSequence.h>
#include <csp/cspsim/Collision.h>
#include <csp/cspsim/Controller.h>
#include <csp/cspsim/DamageModifier.h>
#include <csp/cspsim/ChannelMirror.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/Engine.h>
#include <csp/cspsim/f16/RegisterF16Objects.h>
#include <csp/cspsim/FlightControlSystem.h>
#include <csp/cspsim/FlightDynamics.h>
#include <csp/cspsim/FlightModel.h>
#include <csp/cspsim/FuelManagementSystem.h>
#include <csp/cspsim/GearAnimation.h>
#include <csp/cspsim/LandingGear.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/PhysicsModel.h>
#include <csp/cspsim/ResourceBundle.h>
#include <csp/cspsim/sound/Sample.h>
#include <csp/cspsim/StabilityFlightModel.h>
#include <csp/cspsim/Station.h>
#include <csp/cspsim/stores/DragProfile.h>
#include <csp/cspsim/stores/FuelTank.h>
#include <csp/cspsim/stores/Hardpoint.h>
#include <csp/cspsim/stores/Missile.h>
#include <csp/cspsim/stores/Projectile.h>
#include <csp/cspsim/stores/Rack.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/cspsim/stores/StoresDefinition.h>
#include <csp/cspsim/stores/StoresManagementSystem.h>
#include <csp/cspsim/System.h>
#include <csp/cspsim/systems/AircraftFlightSensors.h>
#include <csp/cspsim/systems/AircraftInputSystem.h>
#include <csp/cspsim/systems/AircraftSimpleFCS.h>
#include <csp/cspsim/SystemsModel.h>
#include <csp/cspsim/TankObject.h>
#include <csp/cspsim/TerrainObject.h>
#include <csp/cspsim/theater/CustomLayoutModel.h>
#include <csp/cspsim/theater/FeatureGroup.h>
#include <csp/cspsim/theater/FeatureGroupList.h>
#include <csp/cspsim/theater/FeatureLayout.h>
#include <csp/cspsim/theater/FeatureObjectModel.h>
#include <csp/cspsim/theater/FeatureQuad.h>
#include <csp/cspsim/theater/IsoContour.h>
#include <csp/cspsim/theater/Objective.h>
#include <csp/cspsim/theater/RandomBillboardModel.h>
#include <csp/cspsim/theater/RandomForestModel.h>
#include <csp/cspsim/Theater.h>
#include <csp/cspsim/ThrustData.h>

namespace csp {

void CSPSIM_EXPORT registerAllObjectInterfaces() {
	{ static ai::AircraftAgent::__csp_interface_proxy instance; }
	{ static AircraftObject::__csp_interface_proxy instance; }
	registerAnimationObjects();
	{ static SharedSequence::__csp_interface_proxy instance; }
	{ static TimedSequence::__csp_interface_proxy instance; }
	{ static DrivenSequence::__csp_interface_proxy instance; }
	{ static GroundCollisionDynamics::__csp_interface_proxy instance; }
	{ static ChannelMirrorSet::__csp_interface_proxy instance; }
	{ static RemoteController::__csp_interface_proxy instance; }
	{ static LocalController::__csp_interface_proxy instance; }
	{ static DamageModifier::__csp_interface_proxy instance; }
	registerChannelMirrorObjects();
	{ static DynamicObject::__csp_interface_proxy instance; }
	{ static Engine::__csp_interface_proxy instance; }
	{ static EngineDynamics::__csp_interface_proxy instance; }
	registerF16Objects();
	registerFlightControlSystemObjects();
	{ static FlightDynamics::__csp_interface_proxy instance; }
	{ static FlightModel::__csp_interface_proxy instance; }
	{ static FuelNode::__csp_interface_proxy instance; }
	{ static FuelTankNode::__csp_interface_proxy instance; }
	{ static FuelManagementSystem::__csp_interface_proxy instance; }
	{ static M2kGearAnimation::__csp_interface_proxy instance; }
	{ static GearAnimation::__csp_interface_proxy instance; }
	{ static GearSequenceAnimation::__csp_interface_proxy instance; }
	{ static LandingGear::__csp_interface_proxy instance; }
	{ static GearDynamics::__csp_interface_proxy instance; }
	{ static ObjectModel::__csp_interface_proxy instance; }
	{ static PhysicsModel::__csp_interface_proxy instance; }
	{ static ResourceBundle::__csp_interface_proxy instance; }
	{ static SoundSample::__csp_interface_proxy instance; }
	{ static StabilityFlightModel::__csp_interface_proxy instance; }
	{ static Station::__csp_interface_proxy instance; }
	{ static DragProfile::__csp_interface_proxy instance; }
	{ static FuelTankData::__csp_interface_proxy instance; }
	{ static HardpointData::__csp_interface_proxy instance; }
	{ static MissileData::__csp_interface_proxy instance; }
	{ static Projectile::__csp_interface_proxy instance; }
	{ static RackData::__csp_interface_proxy instance; }
	{ static RackMount::__csp_interface_proxy instance; }
	{ static StoreData::__csp_interface_proxy instance; }
	{ static StoreCompatibilityTable::__csp_interface_proxy instance; }
	{ static StoresDefinition::__csp_interface_proxy instance; }
	{ static StoresManagementSystem::__csp_interface_proxy instance; }
	{ static System::__csp_interface_proxy instance; }
	{ static AircraftFlightSensors::__csp_interface_proxy instance; }
	{ static AircraftInputSystem::__csp_interface_proxy instance; }
	{ static AircraftSimpleFCS::__csp_interface_proxy instance; }
	{ static SystemsModel::__csp_interface_proxy instance; }
	{ static TankObject::__csp_interface_proxy instance; }
	{ static TerrainObject::__csp_interface_proxy instance; }
	{ static FeatureModel::__csp_interface_proxy instance; }
	{ static FeatureGroupModel::__csp_interface_proxy instance; }
	{ static CustomLayoutModel::__csp_interface_proxy instance; }
	{ static FeatureGroup::__csp_interface_proxy instance; }
	{ static FeatureGroupList::__csp_interface_proxy instance; }
	{ static FeatureLayout::__csp_interface_proxy instance; }
	{ static FeatureObjectModel::__csp_interface_proxy instance; }
	{ static FeatureQuad::__csp_interface_proxy instance; }
	{ static IsoContour::__csp_interface_proxy instance; }
	{ static RectangularCurve::__csp_interface_proxy instance; }
	{ static Circle::__csp_interface_proxy instance; }
	{ static Objective::__csp_interface_proxy instance; }
	{ static RandomBillboardModel::__csp_interface_proxy instance; }
	{ static RandomForestModel::__csp_interface_proxy instance; }
	{ static Theater::__csp_interface_proxy instance; }
	{ static ThrustData::__csp_interface_proxy instance; }
}

} // namespace csp
