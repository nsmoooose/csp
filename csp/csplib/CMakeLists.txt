# makefile for csplib
SET(LIB_NAME csplib)

ADD_DEFINITIONS(-DCSPLIB_EXPORTS)

IF (WIN32)
	ADD_DEFINITIONS(-D_USRDLL -D_DLL)
ENDIF (WIN32)

SET (SOURCE_AND_HEADER_PATH ${csp_SOURCE_DIR}/csp/csplib)
				   
ADD_LIBRARY(${LIB_NAME} SHARED 
	${SOURCE_AND_HEADER_PATH}/data/Archive.h
	${SOURCE_AND_HEADER_PATH}/data/Archive.cpp
	${SOURCE_AND_HEADER_PATH}/data/BaseType.h
	${SOURCE_AND_HEADER_PATH}/data/DataArchive.h
	${SOURCE_AND_HEADER_PATH}/data/DataArchive.cpp
	${SOURCE_AND_HEADER_PATH}/data/DataManager.h
	${SOURCE_AND_HEADER_PATH}/data/DataManager.cpp
	${SOURCE_AND_HEADER_PATH}/data/Date.h
	${SOURCE_AND_HEADER_PATH}/data/Date.cpp
	${SOURCE_AND_HEADER_PATH}/data/Enum.h
	${SOURCE_AND_HEADER_PATH}/data/Enum.cpp
	${SOURCE_AND_HEADER_PATH}/data/External.h
	${SOURCE_AND_HEADER_PATH}/data/External.cpp
	${SOURCE_AND_HEADER_PATH}/data/GeoPos.h
	${SOURCE_AND_HEADER_PATH}/data/GeoPos.cpp
	${SOURCE_AND_HEADER_PATH}/data/InterfaceProxy.h
	${SOURCE_AND_HEADER_PATH}/data/InterfaceProxy.cpp
	${SOURCE_AND_HEADER_PATH}/data/InterfaceRegistry.h
	${SOURCE_AND_HEADER_PATH}/data/InterfaceRegistry.cpp
	${SOURCE_AND_HEADER_PATH}/data/Key.h
	${SOURCE_AND_HEADER_PATH}/data/Key.cpp
	${SOURCE_AND_HEADER_PATH}/data/LUT.h
	${SOURCE_AND_HEADER_PATH}/data/LUT.cpp
	${SOURCE_AND_HEADER_PATH}/data/Link.h
	${SOURCE_AND_HEADER_PATH}/data/Link.cpp
	${SOURCE_AND_HEADER_PATH}/data/Matrix3.h
	${SOURCE_AND_HEADER_PATH}/data/Matrix3.cpp
	${SOURCE_AND_HEADER_PATH}/data/Object.h
	${SOURCE_AND_HEADER_PATH}/data/Object.cpp
	${SOURCE_AND_HEADER_PATH}/data/ObjectInterface.h
	${SOURCE_AND_HEADER_PATH}/data/Path.h
	${SOURCE_AND_HEADER_PATH}/data/Path.cpp
	${SOURCE_AND_HEADER_PATH}/data/Quat.h
	${SOURCE_AND_HEADER_PATH}/data/Quat.cpp
	${SOURCE_AND_HEADER_PATH}/data/Real.h
	${SOURCE_AND_HEADER_PATH}/data/Real.cpp
	${SOURCE_AND_HEADER_PATH}/data/TypeAdapter.h
	${SOURCE_AND_HEADER_PATH}/data/TypeAdapter.cpp
	${SOURCE_AND_HEADER_PATH}/data/Types.h
	${SOURCE_AND_HEADER_PATH}/data/Vector3.h
	${SOURCE_AND_HEADER_PATH}/data/Vector3.cpp
	
	${SOURCE_AND_HEADER_PATH}/net/Callbacks.h
	${SOURCE_AND_HEADER_PATH}/net/ClientServer.h
	${SOURCE_AND_HEADER_PATH}/net/ClientServer.cpp
	${SOURCE_AND_HEADER_PATH}/net/DispatchCache.h
	${SOURCE_AND_HEADER_PATH}/net/ClientServerMessages.h
	${SOURCE_AND_HEADER_PATH}/net/ClientServerMessages.cpp
	${SOURCE_AND_HEADER_PATH}/net/DispatchHandler.cpp
	${SOURCE_AND_HEADER_PATH}/net/DispatchHandler.h
	${SOURCE_AND_HEADER_PATH}/net/DispatchManager.cpp
	${SOURCE_AND_HEADER_PATH}/net/DispatchManager.h
	${SOURCE_AND_HEADER_PATH}/net/DispatchTarget.cpp
	${SOURCE_AND_HEADER_PATH}/net/DispatchTarget.h
	${SOURCE_AND_HEADER_PATH}/net/HandlerSet.h
	${SOURCE_AND_HEADER_PATH}/net/Median.h
	${SOURCE_AND_HEADER_PATH}/net/MessageHandler.h
	${SOURCE_AND_HEADER_PATH}/net/MessageQueue.h
	${SOURCE_AND_HEADER_PATH}/net/NetBase.h
	${SOURCE_AND_HEADER_PATH}/net/NetRandom.cpp
	${SOURCE_AND_HEADER_PATH}/net/NetRandom.h
	${SOURCE_AND_HEADER_PATH}/net/NetworkInterface.cpp
	${SOURCE_AND_HEADER_PATH}/net/NetworkInterface.h
	${SOURCE_AND_HEADER_PATH}/net/NetworkMessage.h
	${SOURCE_AND_HEADER_PATH}/net/NetworkNode.cpp
	${SOURCE_AND_HEADER_PATH}/net/NetworkNode.h
	${SOURCE_AND_HEADER_PATH}/net/PacketDecoder.h
	${SOURCE_AND_HEADER_PATH}/net/PacketHandler.h
	${SOURCE_AND_HEADER_PATH}/net/PacketQueue.h
	${SOURCE_AND_HEADER_PATH}/net/PacketSource.h
	${SOURCE_AND_HEADER_PATH}/net/PeerInfo.cpp
	${SOURCE_AND_HEADER_PATH}/net/PeerInfo.h
	${SOURCE_AND_HEADER_PATH}/net/RecordCodec.cpp
	${SOURCE_AND_HEADER_PATH}/net/RecordCodec.h
	${SOURCE_AND_HEADER_PATH}/net/ReliablePacket.h
	${SOURCE_AND_HEADER_PATH}/net/RoutingHandler.cpp
	${SOURCE_AND_HEADER_PATH}/net/RoutingHandler.h
	${SOURCE_AND_HEADER_PATH}/net/Sockets.h
	${SOURCE_AND_HEADER_PATH}/net/StopWatch.h
	${SOURCE_AND_HEADER_PATH}/net/TaggedRecord.h
	${SOURCE_AND_HEADER_PATH}/net/TaggedRecordRegistry.cpp
	${SOURCE_AND_HEADER_PATH}/net/TaggedRecordRegistry.h

	${SOURCE_AND_HEADER_PATH}/numeric/NumericalMethod.cpp
	${SOURCE_AND_HEADER_PATH}/numeric/NumericalMethod.h
	${SOURCE_AND_HEADER_PATH}/numeric/Vector.h
	${SOURCE_AND_HEADER_PATH}/numeric/VectorField.h

	${SOURCE_AND_HEADER_PATH}/spatial/QuadTree.cpp
	${SOURCE_AND_HEADER_PATH}/spatial/QuadTree.h

	${SOURCE_AND_HEADER_PATH}/thread/AtomicCounter.h
	${SOURCE_AND_HEADER_PATH}/thread/Synchronization.h
	${SOURCE_AND_HEADER_PATH}/thread/Thread.h
	${SOURCE_AND_HEADER_PATH}/thread/ThreadQueue.h
	${SOURCE_AND_HEADER_PATH}/thread/ThreadUtil.h

	${SOURCE_AND_HEADER_PATH}/util/Cache.h
	${SOURCE_AND_HEADER_PATH}/util/Callback.h
	${SOURCE_AND_HEADER_PATH}/util/CallbackDecl.h
	${SOURCE_AND_HEADER_PATH}/util/CircularBuffer.h
	${SOURCE_AND_HEADER_PATH}/util/Composite.h
	${SOURCE_AND_HEADER_PATH}/util/Config.h
	${SOURCE_AND_HEADER_PATH}/util/Conversions.h
	${SOURCE_AND_HEADER_PATH}/util/Dispatch.h
	${SOURCE_AND_HEADER_PATH}/util/Endian.h
	${SOURCE_AND_HEADER_PATH}/util/Exception.cpp
	${SOURCE_AND_HEADER_PATH}/util/Exception.h
	${SOURCE_AND_HEADER_PATH}/util/Export.h
	${SOURCE_AND_HEADER_PATH}/util/FileUtility.cpp
	${SOURCE_AND_HEADER_PATH}/util/FileUtility.h
	${SOURCE_AND_HEADER_PATH}/util/HashUtility.cpp
	${SOURCE_AND_HEADER_PATH}/util/HashUtility.h
	${SOURCE_AND_HEADER_PATH}/util/LocalUpdate.cpp
	${SOURCE_AND_HEADER_PATH}/util/LocalUpdate.h
	${SOURCE_AND_HEADER_PATH}/util/Log.h
	${SOURCE_AND_HEADER_PATH}/util/LogConstants.h
	${SOURCE_AND_HEADER_PATH}/util/LogStream.cpp
	${SOURCE_AND_HEADER_PATH}/util/LogStream.h
	${SOURCE_AND_HEADER_PATH}/util/Math.h
	${SOURCE_AND_HEADER_PATH}/util/MessageTypes.h
	${SOURCE_AND_HEADER_PATH}/util/Modules.cpp
	${SOURCE_AND_HEADER_PATH}/util/Modules.h
	${SOURCE_AND_HEADER_PATH}/util/Namespace.h
	${SOURCE_AND_HEADER_PATH}/util/Noise.cpp
	${SOURCE_AND_HEADER_PATH}/util/Noise.h
	${SOURCE_AND_HEADER_PATH}/util/Properties.h
	${SOURCE_AND_HEADER_PATH}/util/Random.cpp
	${SOURCE_AND_HEADER_PATH}/util/Random.h
	${SOURCE_AND_HEADER_PATH}/util/Ref.h
	${SOURCE_AND_HEADER_PATH}/util/Referenced.h
	${SOURCE_AND_HEADER_PATH}/util/ScopedPointer.h
	${SOURCE_AND_HEADER_PATH}/util/Signal.h
	${SOURCE_AND_HEADER_PATH}/util/SignalFwd.h
	${SOURCE_AND_HEADER_PATH}/util/SimpleConfig.cpp
	${SOURCE_AND_HEADER_PATH}/util/SimpleConfig.h
	${SOURCE_AND_HEADER_PATH}/util/Singleton.h
	${SOURCE_AND_HEADER_PATH}/util/StringTools.cpp
	${SOURCE_AND_HEADER_PATH}/util/StringTools.h
	${SOURCE_AND_HEADER_PATH}/util/SynchronousUpdate.cpp
	${SOURCE_AND_HEADER_PATH}/util/SynchronousUpdate.h
	${SOURCE_AND_HEADER_PATH}/util/Testing.cpp
	${SOURCE_AND_HEADER_PATH}/util/Testing.h
	${SOURCE_AND_HEADER_PATH}/util/TimeStamp.h
	${SOURCE_AND_HEADER_PATH}/util/Timing.cpp
	${SOURCE_AND_HEADER_PATH}/util/Timing.h
	${SOURCE_AND_HEADER_PATH}/util/Trace.cpp
	${SOURCE_AND_HEADER_PATH}/util/Trace.h
	${SOURCE_AND_HEADER_PATH}/util/Uniform.h
	${SOURCE_AND_HEADER_PATH}/util/Verify.h
	${SOURCE_AND_HEADER_PATH}/util/Version.cpp
	${SOURCE_AND_HEADER_PATH}/util/Version.h
	${SOURCE_AND_HEADER_PATH}/util/WeakReferenced.cpp
	${SOURCE_AND_HEADER_PATH}/util/WeakReferenced.h
	${SOURCE_AND_HEADER_PATH}/util/hash_map.h
	${SOURCE_AND_HEADER_PATH}/util/osg.h

	${SOURCE_AND_HEADER_PATH}/xml/XmlParser.cpp
	${SOURCE_AND_HEADER_PATH}/xml/XmlParser.h
)

TARGET_LINK_LIBRARIES(${LIB_NAME} ccgnu2.lib
								  sigc-2.0.lib
					 )

#SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../../bin/")


# ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
				   # PRE_BUILD
				   # COMMAND ${PYTHON_EXECUTABLE} ${csp_SOURCE_DIR}/csp/tools/trc/trc.py --source=ClientServerMessages.cpp --header=ClientServerMessages.h ClientServerMessages.net
				   # WORKING_DIRECTORY ${csp_SOURCE_DIR}/csp/csplib/net
				   # COMMENT "create ClientServerMessages.h/.cpp from ClientServerMessages.net"
				  # )

# We want all dlls and libs of this project in the same directory. Visual Studio puts project output files
# into individual directories per configuration (e.h. "debug" and "release"), so we have to copy the files
# from there into our own output dir in a separate step. CAUTION: The copy command uses hard-coded environment 
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
						   --source=${csp_SOURCE_DIR}/csp/csplib/net/ClientServerMessages.cpp 
						   --header=${csp_SOURCE_DIR}/csp/csplib/net/ClientServerMessages.h 
						   ${csp_SOURCE_DIR}/csp/csplib/net/ClientServerMessages.net
				  )

# csplib needs a config file created by csplib_config, so make sure csplib_config exists
ADD_DEPENDENCIES(csplib csplib_config)
