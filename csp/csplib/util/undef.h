/** @file undef.h
 *
 *  This header should be included immediately after including external
 *  headers that may define any of the symbols below as macros.
 */

// the following symbols are used in the logging macros, so previous macro
// declarations must be deleted.  note that csplib does *not* redefine these
// macros (see LogConstants.h for details).
#ifdef DEBUG
#undef DEBUG
#endif
#ifdef INFO
#undef INFO
#endif
#ifdef WARNING
#undef WARNING
#endif
#ifdef ERROR
#undef ERROR
#endif
#ifdef FATAL
#undef FATAL
#endif

// logging categories (see LogConstants.h)
#ifdef DATA
#undef DATA
#endif
#ifdef ARCHIVE
#undef ARCHIVE
#endif
#ifdef REGISTRY
#undef REGISTRY
#endif
#ifdef THREAD
#undef THREAD
#endif
#ifdef TIME
#undef TIME
#endif
#ifdef NETWORK
#undef NETWORK
#endif
#ifdef AUDIO
#undef AUDIO
#endif
#ifdef OPENGL
#undef OPENGL
#endif
#ifdef INPUT
#undef INPUT
#endif
#ifdef NUMERIC
#undef NUMERIC
#endif
#ifdef PHYSICS
#undef PHYSICS
#endif
#ifdef TESTING
#undef TESTING
#endif
#ifdef TIMING
#undef TIMING
#endif
#ifdef HANDSHAKE
#undef HANDSHAKE
#endif
#ifdef BALANCING
#undef BALANCING
#endif
#ifdef PACKET
#undef PACKET
#endif
#ifdef PEER
#undef PEER
#endif
#ifdef MESSAGE
#undef MESSAGE
#endif
#ifdef TERRAIN
#undef TERRAIN
#endif
#ifdef SCENE
#undef SCENE
#endif
#ifdef BATTLEFIELD
#undef BATTLEFIELD
#endif
#ifdef OBJECT
#undef OBJECT
#endif
#ifdef APP
#undef APP
#endif
#ifdef VIEW
#undef VIEW
#endif
#ifdef THEATER
#undef THEATER
#endif
#ifdef NONE
#undef NONE
#endif
#ifdef ALL
#undef ALL
#endif

// some external headers define min and max as macros.  we use the standard
// inline functions (std::min and std::max) instead.
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

