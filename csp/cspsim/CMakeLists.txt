# makefile for cspsim
SET(LIB_NAME cspsim)

ADD_DEFINITIONS(-DCSPSIM_EXPORTS -D__CSPSIM_EXE__)

SET (SOURCE_AND_HEADER_PATH ${csp_SOURCE_DIR}/csp/cspsim)

MESSAGE("SOURCE_AND_HEADER_PATH: ${SOURCE_AND_HEADER_PATH}")

CONFIGURE_FILE(${SOURCE_AND_HEADER_PATH}/battlefield/BattlefieldMessages.cpp.in ${SOURCE_AND_HEADER_PATH}/battlefield/BattlefieldMessages.cpp)
CONFIGURE_FILE(${SOURCE_AND_HEADER_PATH}/battlefield/BattlefieldMessages.h.in ${SOURCE_AND_HEADER_PATH}/battlefield/BattlefieldMessages.h)

CONFIGURE_FILE(${SOURCE_AND_HEADER_PATH}/ObjectUpdate.cpp.in ${SOURCE_AND_HEADER_PATH}/ObjectUpdate.cpp)
CONFIGURE_FILE(${SOURCE_AND_HEADER_PATH}/ObjectUpdate.h.in ${SOURCE_AND_HEADER_PATH}/ObjectUpdate.h)

ADD_LIBRARY(${LIB_NAME} SHARED 
			${SOURCE_AND_HEADER_PATH}/battlefield/Battlefield.cpp
			${SOURCE_AND_HEADER_PATH}/battlefield/Battlefield.h
			${SOURCE_AND_HEADER_PATH}/battlefield/BattlefieldMessages.h
			${SOURCE_AND_HEADER_PATH}/battlefield/BattlefieldMessages.cpp
			${SOURCE_AND_HEADER_PATH}/battlefield/SimObject.cpp
			${SOURCE_AND_HEADER_PATH}/battlefield/SimObject.h

			${SOURCE_AND_HEADER_PATH}/Config.cpp
			${SOURCE_AND_HEADER_PATH}/Config.h
			${SOURCE_AND_HEADER_PATH}/config/Configuration.cpp
			${SOURCE_AND_HEADER_PATH}/config/Configuration.h
			${SOURCE_AND_HEADER_PATH}/config/Display.cpp
			${SOURCE_AND_HEADER_PATH}/config/Display.h
			${SOURCE_AND_HEADER_PATH}/config/UserInterface.cpp
			${SOURCE_AND_HEADER_PATH}/config/UserInterface.h

			${SOURCE_AND_HEADER_PATH}/sky/Colorspace.cpp
			${SOURCE_AND_HEADER_PATH}/sky/Colorspace.h
			${SOURCE_AND_HEADER_PATH}/sky/Magnitude.cpp
			${SOURCE_AND_HEADER_PATH}/sky/Magnitude.h
			${SOURCE_AND_HEADER_PATH}/sky/OrbitalBody.cpp
			${SOURCE_AND_HEADER_PATH}/sky/OrbitalBody.h
			${SOURCE_AND_HEADER_PATH}/sky/OrbitalBodyImposter.cpp
			${SOURCE_AND_HEADER_PATH}/sky/OrbitalBodyImposter.h
			${SOURCE_AND_HEADER_PATH}/sky/OrbitalBodyModel.cpp
			${SOURCE_AND_HEADER_PATH}/sky/OrbitalBodyModel.h
			${SOURCE_AND_HEADER_PATH}/sky/PhaseShader.cpp
			${SOURCE_AND_HEADER_PATH}/sky/PhaseShader.h
			${SOURCE_AND_HEADER_PATH}/sky/Sky.cpp
			${SOURCE_AND_HEADER_PATH}/sky/Sky.h
			${SOURCE_AND_HEADER_PATH}/sky/SkyDome.cpp
			${SOURCE_AND_HEADER_PATH}/sky/SkyDome.h
			${SOURCE_AND_HEADER_PATH}/sky/SkyShader.cpp
			${SOURCE_AND_HEADER_PATH}/sky/SkyShader.h
			${SOURCE_AND_HEADER_PATH}/sky/SolarSystem.cpp
			${SOURCE_AND_HEADER_PATH}/sky/SolarSystem.h
			${SOURCE_AND_HEADER_PATH}/sky/StarDome.cpp
			${SOURCE_AND_HEADER_PATH}/sky/StarDome.h
			${SOURCE_AND_HEADER_PATH}/sky/Stars.h

			${SOURCE_AND_HEADER_PATH}/ai/AircraftAgent.cpp
			${SOURCE_AND_HEADER_PATH}/ai/AircraftAgent.h
			${SOURCE_AND_HEADER_PATH}/ai/AircraftControl.cpp
			${SOURCE_AND_HEADER_PATH}/ai/AircraftControl.h
			${SOURCE_AND_HEADER_PATH}/ai/AircraftMission.cpp
			${SOURCE_AND_HEADER_PATH}/ai/AircraftMission.h
			${SOURCE_AND_HEADER_PATH}/ai/AircraftTask.cpp
			${SOURCE_AND_HEADER_PATH}/ai/AircraftTask.h
			${SOURCE_AND_HEADER_PATH}/ai/DiveRecovery.cpp
			${SOURCE_AND_HEADER_PATH}/ai/DiveRecovery.h
			${SOURCE_AND_HEADER_PATH}/ai/LandingTask.cpp
			${SOURCE_AND_HEADER_PATH}/ai/LandingTask.h
			${SOURCE_AND_HEADER_PATH}/ai/TakeoffTask.cpp
			${SOURCE_AND_HEADER_PATH}/ai/TakeoffTask.h
			${SOURCE_AND_HEADER_PATH}/ai/PID.h
			${SOURCE_AND_HEADER_PATH}/ai/StateMachine.cpp
			${SOURCE_AND_HEADER_PATH}/ai/StateMachine.h
			${SOURCE_AND_HEADER_PATH}/ai/Task.cpp
			${SOURCE_AND_HEADER_PATH}/ai/Task.h

			${SOURCE_AND_HEADER_PATH}/AircraftObject.cpp
			${SOURCE_AND_HEADER_PATH}/AircraftObject.h
			${SOURCE_AND_HEADER_PATH}/Animation.cpp
			${SOURCE_AND_HEADER_PATH}/Animation.h
			${SOURCE_AND_HEADER_PATH}/AnimationSequence.cpp
			${SOURCE_AND_HEADER_PATH}/AnimationSequence.h
			${SOURCE_AND_HEADER_PATH}/BaseDynamics.cpp
			${SOURCE_AND_HEADER_PATH}/BaseDynamics.h
			${SOURCE_AND_HEADER_PATH}/BaseScreen.cpp
			${SOURCE_AND_HEADER_PATH}/BaseScreen.h
			${SOURCE_AND_HEADER_PATH}/Bus.cpp
			${SOURCE_AND_HEADER_PATH}/Bus.h
			${SOURCE_AND_HEADER_PATH}/CSPSim.cpp
			${SOURCE_AND_HEADER_PATH}/CSPSim.h
			${SOURCE_AND_HEADER_PATH}/CSPViewer.cpp
			${SOURCE_AND_HEADER_PATH}/CSPViewer.h
			${SOURCE_AND_HEADER_PATH}/ChannelMirror.h
			${SOURCE_AND_HEADER_PATH}/Channels.cpp
				#	'ChunkLodTerrain.cpp
				#	'ChunkLodTerrain.h
			${SOURCE_AND_HEADER_PATH}/CockpitInterface.cpp
			${SOURCE_AND_HEADER_PATH}/CockpitInterface.h
			${SOURCE_AND_HEADER_PATH}/Collision.cpp
			${SOURCE_AND_HEADER_PATH}/Collision.h
			${SOURCE_AND_HEADER_PATH}/ConditionsChannels.h
			${SOURCE_AND_HEADER_PATH}/ControlInputsChannels.h
			${SOURCE_AND_HEADER_PATH}/ControlNode.h
			${SOURCE_AND_HEADER_PATH}/ControlSurfacesChannels.h
			${SOURCE_AND_HEADER_PATH}/Controller.cpp
			${SOURCE_AND_HEADER_PATH}/Controller.h
			${SOURCE_AND_HEADER_PATH}/DamageModifier.cpp
			${SOURCE_AND_HEADER_PATH}/DamageModifier.h
			${SOURCE_AND_HEADER_PATH}/DataRecorder.cpp
			${SOURCE_AND_HEADER_PATH}/DataRecorder.h
			${SOURCE_AND_HEADER_PATH}/DoubleChannelMirror.cpp
			${SOURCE_AND_HEADER_PATH}/DynamicObject.cpp
			${SOURCE_AND_HEADER_PATH}/DynamicObject.h
			${SOURCE_AND_HEADER_PATH}/Engine.cpp
			${SOURCE_AND_HEADER_PATH}/Engine.h
			${SOURCE_AND_HEADER_PATH}/Exception.cpp
			${SOURCE_AND_HEADER_PATH}/Exception.h
			${SOURCE_AND_HEADER_PATH}/Filters.h
			${SOURCE_AND_HEADER_PATH}/FlightControlSystem.cpp
			${SOURCE_AND_HEADER_PATH}/FlightControlSystem.h
			${SOURCE_AND_HEADER_PATH}/FlightDynamics.cpp
			${SOURCE_AND_HEADER_PATH}/FlightDynamics.h
			${SOURCE_AND_HEADER_PATH}/FlightDynamicsChannels.h
			${SOURCE_AND_HEADER_PATH}/FlightModel.cpp
			${SOURCE_AND_HEADER_PATH}/FlightModel.h
			${SOURCE_AND_HEADER_PATH}/FuelManagementSystem.cpp
			${SOURCE_AND_HEADER_PATH}/FuelManagementSystem.h
			${SOURCE_AND_HEADER_PATH}/GameScreen.cpp
			${SOURCE_AND_HEADER_PATH}/GameScreen.h
			${SOURCE_AND_HEADER_PATH}/GearAnimation.cpp
			${SOURCE_AND_HEADER_PATH}/GearAnimation.h
			${SOURCE_AND_HEADER_PATH}/KineticsChannels.h
			${SOURCE_AND_HEADER_PATH}/LandingGear.cpp
			${SOURCE_AND_HEADER_PATH}/LandingGear.h
			${SOURCE_AND_HEADER_PATH}/LandingGearChannels.h
			${SOURCE_AND_HEADER_PATH}/LogoScreen.cpp
			${SOURCE_AND_HEADER_PATH}/LogoScreen.h
			${SOURCE_AND_HEADER_PATH}/MenuScreen.cpp
			${SOURCE_AND_HEADER_PATH}/MenuScreen.h
			#${SOURCE_AND_HEADER_PATH}/Module.cpp
			#${SOURCE_AND_HEADER_PATH}/Module.h
			${SOURCE_AND_HEADER_PATH}/NavigationChannels.h
			${SOURCE_AND_HEADER_PATH}/ObjectModel.cpp
			${SOURCE_AND_HEADER_PATH}/ObjectModel.h
			${SOURCE_AND_HEADER_PATH}/ObjectUpdate.cpp
			${SOURCE_AND_HEADER_PATH}/ObjectUpdate.h
			${SOURCE_AND_HEADER_PATH}/PhysicsModel.cpp
			${SOURCE_AND_HEADER_PATH}/PhysicsModel.h
			${SOURCE_AND_HEADER_PATH}/Profile.h
			${SOURCE_AND_HEADER_PATH}/Projection.cpp
			${SOURCE_AND_HEADER_PATH}/Projection.h
			${SOURCE_AND_HEADER_PATH}/RegisterObjectInterfaces.cpp
			${SOURCE_AND_HEADER_PATH}/RegisterObjectInterfaces.h
			${SOURCE_AND_HEADER_PATH}/ResourceBundle.cpp
			${SOURCE_AND_HEADER_PATH}/ResourceBundle.h
			${SOURCE_AND_HEADER_PATH}/SceneConstants.h
			${SOURCE_AND_HEADER_PATH}/SceneModel.cpp
			${SOURCE_AND_HEADER_PATH}/SceneModel.h
			${SOURCE_AND_HEADER_PATH}/ScreenInfo.cpp
			${SOURCE_AND_HEADER_PATH}/ScreenInfo.h
			${SOURCE_AND_HEADER_PATH}/ScreenInfoNode.cpp
			${SOURCE_AND_HEADER_PATH}/ScreenInfoNode.h
			${SOURCE_AND_HEADER_PATH}/ScreenInfoManager.cpp
			${SOURCE_AND_HEADER_PATH}/ScreenInfoManager.h
			${SOURCE_AND_HEADER_PATH}/SDLViewer.cpp
			${SOURCE_AND_HEADER_PATH}/SDLViewer.h
			${SOURCE_AND_HEADER_PATH}/SDLEventHandler.cpp
			${SOURCE_AND_HEADER_PATH}/SDLEventHandler.h
			${SOURCE_AND_HEADER_PATH}/SDLGraphicsWindow.cpp
			${SOURCE_AND_HEADER_PATH}/SDLGraphicsWindow.h
			${SOURCE_AND_HEADER_PATH}/Shader.cpp
			${SOURCE_AND_HEADER_PATH}/Shader.h
			${SOURCE_AND_HEADER_PATH}/SimpleSceneManager.cpp
			${SOURCE_AND_HEADER_PATH}/SimpleSceneManager.h
					#'Sky.cpp
					#'Sky.h
			${SOURCE_AND_HEADER_PATH}/SmokeEffects.cpp
			${SOURCE_AND_HEADER_PATH}/SmokeEffects.h
			${SOURCE_AND_HEADER_PATH}/StabilityFlightModel.cpp
			${SOURCE_AND_HEADER_PATH}/StabilityFlightModel.h
			${SOURCE_AND_HEADER_PATH}/StateStack.h
			${SOURCE_AND_HEADER_PATH}/Station.cpp
			${SOURCE_AND_HEADER_PATH}/Station.h
			${SOURCE_AND_HEADER_PATH}/Steerpoint.h
			${SOURCE_AND_HEADER_PATH}/System.cpp
			${SOURCE_AND_HEADER_PATH}/System.h
			${SOURCE_AND_HEADER_PATH}/SystemsModel.cpp
			${SOURCE_AND_HEADER_PATH}/SystemsModel.h
			${SOURCE_AND_HEADER_PATH}/TankObject.cpp
			${SOURCE_AND_HEADER_PATH}/TankObject.h
			${SOURCE_AND_HEADER_PATH}/TerrainObject.cpp
			${SOURCE_AND_HEADER_PATH}/TerrainObject.h
			${SOURCE_AND_HEADER_PATH}/Theater.cpp
			${SOURCE_AND_HEADER_PATH}/Theater.h
			${SOURCE_AND_HEADER_PATH}/ThrustData.cpp
			${SOURCE_AND_HEADER_PATH}/ThrustData.h
			${SOURCE_AND_HEADER_PATH}/VirtualScene.cpp
			${SOURCE_AND_HEADER_PATH}/VirtualScene.h

			${SOURCE_AND_HEADER_PATH}/battlefield/LocalBattlefield.cpp
			${SOURCE_AND_HEADER_PATH}/battlefield/LocalBattlefield.h
			${SOURCE_AND_HEADER_PATH}/battlefield/SceneManager.cpp
			${SOURCE_AND_HEADER_PATH}/battlefield/SceneManager.h

			${SOURCE_AND_HEADER_PATH}/f16/AlphaNumericDisplay.cpp
			${SOURCE_AND_HEADER_PATH}/f16/AlphaNumericDisplay.h
			${SOURCE_AND_HEADER_PATH}/f16/Constants.cpp
			${SOURCE_AND_HEADER_PATH}/f16/Constants.h
			${SOURCE_AND_HEADER_PATH}/f16/DataEntry.h
			${SOURCE_AND_HEADER_PATH}/f16/DataEntryDisplay.cpp
			${SOURCE_AND_HEADER_PATH}/f16/DataEntryDisplay.h
			${SOURCE_AND_HEADER_PATH}/f16/DataEntryPage.cpp
			${SOURCE_AND_HEADER_PATH}/f16/DataEntryPage.h
			${SOURCE_AND_HEADER_PATH}/f16/F16Channels.cpp
			${SOURCE_AND_HEADER_PATH}/f16/F16Channels.h
			${SOURCE_AND_HEADER_PATH}/f16/F16Engine.cpp
			${SOURCE_AND_HEADER_PATH}/f16/F16Engine.h
			${SOURCE_AND_HEADER_PATH}/f16/F16FlightModel.cpp
			${SOURCE_AND_HEADER_PATH}/f16/F16FlightModel.h
			${SOURCE_AND_HEADER_PATH}/f16/F16GearDynamics.cpp
			${SOURCE_AND_HEADER_PATH}/f16/F16GearDynamics.h
			${SOURCE_AND_HEADER_PATH}/f16/F16HUD.cpp
			${SOURCE_AND_HEADER_PATH}/f16/F16HUD.h
			${SOURCE_AND_HEADER_PATH}/f16/F16INS.cpp
			${SOURCE_AND_HEADER_PATH}/f16/F16INS.h
				#	'f16/F16InputSystem.cpp
				#	'f16/F16InputSystem.h
			${SOURCE_AND_HEADER_PATH}/f16/F16System.cpp
			${SOURCE_AND_HEADER_PATH}/f16/F16System.h
			${SOURCE_AND_HEADER_PATH}/f16/FLCS.cpp
			${SOURCE_AND_HEADER_PATH}/f16/FuelSystem.cpp
			${SOURCE_AND_HEADER_PATH}/f16/FuelSystem.h
			${SOURCE_AND_HEADER_PATH}/f16/GroundAvoidanceAdvisory.cpp
			${SOURCE_AND_HEADER_PATH}/f16/GroundAvoidanceAdvisory.h
			${SOURCE_AND_HEADER_PATH}/f16/JetFuelStarter.cpp
			${SOURCE_AND_HEADER_PATH}/f16/JetFuelStarter.h
			${SOURCE_AND_HEADER_PATH}/f16/MasterModes.cpp
			${SOURCE_AND_HEADER_PATH}/f16/MasterModes.h
			${SOURCE_AND_HEADER_PATH}/f16/MultiFunctionDisplay.cpp
			${SOURCE_AND_HEADER_PATH}/f16/MultiFunctionDisplay.h
			${SOURCE_AND_HEADER_PATH}/f16/NavigationSystem.cpp
			${SOURCE_AND_HEADER_PATH}/f16/NavigationSystem.h
			${SOURCE_AND_HEADER_PATH}/f16/PageALOW.h
			${SOURCE_AND_HEADER_PATH}/f16/PageCNI.cpp
			${SOURCE_AND_HEADER_PATH}/f16/PageCNI.h
			${SOURCE_AND_HEADER_PATH}/f16/PageLIST.cpp
			${SOURCE_AND_HEADER_PATH}/f16/PageLIST.h
			${SOURCE_AND_HEADER_PATH}/f16/PageSTPT.h
			${SOURCE_AND_HEADER_PATH}/f16/PanelHUD.cpp
			${SOURCE_AND_HEADER_PATH}/f16/PanelHUD.h
			${SOURCE_AND_HEADER_PATH}/f16/Radio.cpp
			${SOURCE_AND_HEADER_PATH}/f16/Radio.h
			${SOURCE_AND_HEADER_PATH}/f16/RegisterF16Objects.cpp
			${SOURCE_AND_HEADER_PATH}/f16/RegisterF16Objects.h
			${SOURCE_AND_HEADER_PATH}/f16/SpecialFonts.cpp
			${SOURCE_AND_HEADER_PATH}/f16/SpecialFonts.h
			${SOURCE_AND_HEADER_PATH}/f16/StoresManagementPages.cpp
			${SOURCE_AND_HEADER_PATH}/f16/UpFrontControls.cpp
			${SOURCE_AND_HEADER_PATH}/f16/UpFrontControls.h
			${SOURCE_AND_HEADER_PATH}/f16/VoiceMessageUnit.cpp
			${SOURCE_AND_HEADER_PATH}/f16/VoiceMessageUnit.h

			${SOURCE_AND_HEADER_PATH}/glDiagnostics.cpp
			${SOURCE_AND_HEADER_PATH}/glDiagnostics.h

			${SOURCE_AND_HEADER_PATH}/hud/Display.cpp
			${SOURCE_AND_HEADER_PATH}/hud/Display.h
			${SOURCE_AND_HEADER_PATH}/hud/DisplayTools.cpp
			${SOURCE_AND_HEADER_PATH}/hud/DisplayTools.h
			${SOURCE_AND_HEADER_PATH}/hud/HUD.cpp
			${SOURCE_AND_HEADER_PATH}/hud/HUD.h
			${SOURCE_AND_HEADER_PATH}/hud/PitchLadder.cpp
			${SOURCE_AND_HEADER_PATH}/hud/PitchLadder.h
			${SOURCE_AND_HEADER_PATH}/hud/Tape.cpp
			${SOURCE_AND_HEADER_PATH}/hud/Tape.h
			${SOURCE_AND_HEADER_PATH}/hud/Text.cpp
			${SOURCE_AND_HEADER_PATH}/hud/Text.h

			${SOURCE_AND_HEADER_PATH}/input/EventMapIndex.cpp
			${SOURCE_AND_HEADER_PATH}/input/EventMapIndex.h
			${SOURCE_AND_HEADER_PATH}/input/EventMapping.cpp
			${SOURCE_AND_HEADER_PATH}/input/EventMapping.h
			${SOURCE_AND_HEADER_PATH}/input/MapEvent.h
			${SOURCE_AND_HEADER_PATH}/input/RawEvent.h
			${SOURCE_AND_HEADER_PATH}/input/HID.cpp
			${SOURCE_AND_HEADER_PATH}/input/HID.h
			${SOURCE_AND_HEADER_PATH}/input/InputEventChannel.h
			${SOURCE_AND_HEADER_PATH}/input/InputInterface.cpp
			${SOURCE_AND_HEADER_PATH}/input/InputInterface.h
			${SOURCE_AND_HEADER_PATH}/input/InputInterfaceWfAdapter.cpp
			${SOURCE_AND_HEADER_PATH}/input/InputInterfaceWfAdapter.h

			${SOURCE_AND_HEADER_PATH}/sound/Loader.cpp
			${SOURCE_AND_HEADER_PATH}/sound/OggLoader.cpp
			${SOURCE_AND_HEADER_PATH}/sound/Sample.cpp
			${SOURCE_AND_HEADER_PATH}/sound/SoundEffect.cpp
			${SOURCE_AND_HEADER_PATH}/sound/SoundEffect.h
			${SOURCE_AND_HEADER_PATH}/sound/SoundEngine.cpp
			${SOURCE_AND_HEADER_PATH}/sound/SoundEngine.h
			${SOURCE_AND_HEADER_PATH}/sound/SoundModel.cpp
			${SOURCE_AND_HEADER_PATH}/sound/SoundModel.h

			${SOURCE_AND_HEADER_PATH}/stores/DragProfile.cpp
			${SOURCE_AND_HEADER_PATH}/stores/DragProfile.h
			${SOURCE_AND_HEADER_PATH}/stores/FuelTank.cpp
			${SOURCE_AND_HEADER_PATH}/stores/FuelTank.h
			${SOURCE_AND_HEADER_PATH}/stores/Hardpoint.cpp
			${SOURCE_AND_HEADER_PATH}/stores/Hardpoint.h
			${SOURCE_AND_HEADER_PATH}/stores/Missile.cpp
			${SOURCE_AND_HEADER_PATH}/stores/Missile.h
			${SOURCE_AND_HEADER_PATH}/stores/Projectile.cpp
			${SOURCE_AND_HEADER_PATH}/stores/Projectile.h
			${SOURCE_AND_HEADER_PATH}/stores/Rack.cpp
			${SOURCE_AND_HEADER_PATH}/stores/Rack.h
			${SOURCE_AND_HEADER_PATH}/stores/Stores.cpp
			${SOURCE_AND_HEADER_PATH}/stores/Stores.h
			${SOURCE_AND_HEADER_PATH}/stores/StoresDatabase.cpp
			${SOURCE_AND_HEADER_PATH}/stores/StoresDatabase.h
			${SOURCE_AND_HEADER_PATH}/stores/StoresDefinition.cpp
			${SOURCE_AND_HEADER_PATH}/stores/StoresDefinition.h
			${SOURCE_AND_HEADER_PATH}/stores/StoresDynamics.h
			${SOURCE_AND_HEADER_PATH}/stores/StoresManagementSystem.cpp
			${SOURCE_AND_HEADER_PATH}/stores/StoresManagementSystem.h

			${SOURCE_AND_HEADER_PATH}/systems/AircraftFlightSensors.cpp
			${SOURCE_AND_HEADER_PATH}/systems/AircraftFlightSensors.h
			${SOURCE_AND_HEADER_PATH}/systems/AircraftInputSystem.cpp
			${SOURCE_AND_HEADER_PATH}/systems/AircraftInputSystem.h
			${SOURCE_AND_HEADER_PATH}/systems/AircraftSimpleFCS.cpp
			${SOURCE_AND_HEADER_PATH}/systems/AircraftSimpleFCS.h

			${SOURCE_AND_HEADER_PATH}/theater/CustomLayoutModel.cpp
			${SOURCE_AND_HEADER_PATH}/theater/CustomLayoutModel.h
			${SOURCE_AND_HEADER_PATH}/theater/ElevationCorrection.cpp
			${SOURCE_AND_HEADER_PATH}/theater/ElevationCorrection.h
			${SOURCE_AND_HEADER_PATH}/theater/Feature.cpp
			${SOURCE_AND_HEADER_PATH}/theater/Feature.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureGroup.cpp
			${SOURCE_AND_HEADER_PATH}/theater/FeatureGroup.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureGroupList.cpp
			${SOURCE_AND_HEADER_PATH}/theater/FeatureGroupList.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureGroupModel.cpp
			${SOURCE_AND_HEADER_PATH}/theater/FeatureGroupModel.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureLayout.cpp
			${SOURCE_AND_HEADER_PATH}/theater/FeatureLayout.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureModel.cpp
			${SOURCE_AND_HEADER_PATH}/theater/FeatureModel.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureObjectModel.cpp
			${SOURCE_AND_HEADER_PATH}/theater/FeatureObjectModel.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureQuad.cpp
			${SOURCE_AND_HEADER_PATH}/theater/FeatureQuad.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureSceneGroup.h
			${SOURCE_AND_HEADER_PATH}/theater/FeatureSceneModel.cpp
			${SOURCE_AND_HEADER_PATH}/theater/FeatureSceneModel.h
			${SOURCE_AND_HEADER_PATH}/theater/IsoContour.cpp
			${SOURCE_AND_HEADER_PATH}/theater/IsoContour.h
			${SOURCE_AND_HEADER_PATH}/theater/LayoutTransform.cpp
			${SOURCE_AND_HEADER_PATH}/theater/LayoutTransform.h
			${SOURCE_AND_HEADER_PATH}/theater/Objective.cpp
			${SOURCE_AND_HEADER_PATH}/theater/Objective.h
			${SOURCE_AND_HEADER_PATH}/theater/RandomBillboardModel.cpp
			${SOURCE_AND_HEADER_PATH}/theater/RandomBillboardModel.h
			${SOURCE_AND_HEADER_PATH}/theater/RandomForestModel.cpp
			${SOURCE_AND_HEADER_PATH}/theater/RandomForestModel.h

			${SOURCE_AND_HEADER_PATH}/views/CameraAgent.cpp
			${SOURCE_AND_HEADER_PATH}/views/CameraAgent.h
			${SOURCE_AND_HEADER_PATH}/views/CameraCommand.h
			${SOURCE_AND_HEADER_PATH}/views/CameraKinematics.cpp
			${SOURCE_AND_HEADER_PATH}/views/CameraKinematics.h
			${SOURCE_AND_HEADER_PATH}/views/ExternalViews.cpp
			${SOURCE_AND_HEADER_PATH}/views/ExternalViews.h
			${SOURCE_AND_HEADER_PATH}/views/InternalView.cpp
			${SOURCE_AND_HEADER_PATH}/views/InternalView.h
			${SOURCE_AND_HEADER_PATH}/views/View.cpp
			${SOURCE_AND_HEADER_PATH}/views/View.h

			${SOURCE_AND_HEADER_PATH}/weather/Atmosphere.cpp
			${SOURCE_AND_HEADER_PATH}/weather/Atmosphere.h
			${SOURCE_AND_HEADER_PATH}/weather/clouds/Cloud.cpp
			${SOURCE_AND_HEADER_PATH}/weather/clouds/Cloud.h
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudBox.cpp
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudBox.h
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudMath.cpp
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudMath.h
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudRegistry.cpp
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudRegistry.h
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudTextureFactory.cpp
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudTextureFactory.h
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudUtils.cpp
			${SOURCE_AND_HEADER_PATH}/weather/clouds/CloudUtils.h
					
			${SOURCE_AND_HEADER_PATH}/weather/clouds/visitors/RemoveRedundantCloudSpritesVisitor.cpp
			${SOURCE_AND_HEADER_PATH}/weather/clouds/visitors/RemoveRedundantCloudSpritesVisitor.h
			${SOURCE_AND_HEADER_PATH}/weather/clouds/visitors/UpdateCloudModelVisitor.cpp
			${SOURCE_AND_HEADER_PATH}/weather/clouds/visitors/UpdateCloudModelVisitor.h
			)

TARGET_LINK_LIBRARIES(${LIB_NAME} ${SOUND_LIBS}
								  ${OSG_LIBS}
								  ${OPENGL_LIBS}
								  ${PYTHON_LIBRARY}
								  SDL
								  csplib
								  cspwf
					 )
					 
# We want all dlls and libs of this project in the same directory. Visual Studio puts project output files
# into individual directories per configuration (e.h. "debug" and "release"), so we have to copy the files
# from there into our own output dir in a separate step. CAUTION: The copy command uses environment 
# variables from Visual Studio. They should work for 2008 and 2010, but maybe not for future versions
IF(MSVC_IDE)
	ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
					   POST_BUILD
					   COMMAND copy $(InputDir)$(ConfigurationName)\\*.* $(InputDir)..\\..\\bin
					   COMMENT "copy output files to common directory"
					  )
ENDIF(MSVC_IDE)

# Some source and header files are created by trc.py on the fly
# This needs to be done prior to compiling, so we use a custom target to run trc.py
ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
	PRE_BUILD
	COMMAND ${PYTHON_EXECUTABLE} 
			${csp_SOURCE_DIR}/csp/tools/trc/trc.py 
			--source=${csp_SOURCE_DIR}/csp/cspsim/battlefield/BattlefieldMessages.cpp 
			--header=${csp_SOURCE_DIR}/csp/cspsim/battlefield/BattlefieldMessages.h 
			${csp_SOURCE_DIR}/csp/cspsim/battlefield/BattlefieldMessages.net
	COMMAND ${PYTHON_EXECUTABLE} 			
			${csp_SOURCE_DIR}/csp/tools/trc/trc.py 
			--source=${csp_SOURCE_DIR}/csp/cspsim/ObjectUpdate.cpp 
			--header=${csp_SOURCE_DIR}/csp/cspsim/ObjectUpdate.h 
			${csp_SOURCE_DIR}/csp/cspsim/ObjectUpdate.net
	)
	
ADD_DEPENDENCIES(cspsim csplib)