#ifndef __DEBUG_TYPES_H__
#define __DEBUG_TYPES_H__

// This file orginated from FlightGear, Modififed by Wolverine

/** \file debug_types.h
 *  Define the various logging classes and prioritiess
 */

/** 
 * Define the possible classes/categories of logging messages
 */
typedef enum {
    CSP_NONE      = 0x00000000,

    CSP_AUDIO     = 0x00000001,
    CSP_OPENGL    = 0x00000002,
    CSP_DIRECTX8  = 0x00000004,
    CSP_FILE      = 0x00000008,
    CSP_INPUT     = 0x00000010,
    CSP_MATH      = 0x00000020,
    CSP_NETWORK   = 0x00000040,
    CSP_SHARED    = 0x00000080,
    CSP_WINDOW    = 0x00000100,
    CSP_FACTORY   = 0x00000200,
    CSP_FRAMEWORK = 0x00000400,
    CSP_LOG       = 0x00000800,
    CSP_TERRAIN   = 0x00001000,
	CSP_APP       = 0x00002000,
    CSP_GEOMETRY  = 0x00004000,
    CSP_UNDEFD    = 0x00008000, // for range checking

    CSP_ALL     = 0xFFFFFFFF
} cspDebugClass;


/**
 * Define the possible logging priorities (and their order).
 */
typedef enum {
    CSP_BULK,	    // For frequent messages
    CSP_TRACE,      // For Trace messages, usually at the start of routines.
    CSP_DEBUG, 	    // Less frequent debug type messages
    CSP_INFO,        // Informatory messages
    CSP_WARN,	    // Possible impending problem
    CSP_ALERT,       // Very possible impending problem
    CSP_ERROR        // Problem (no core)
    // SG_ABORT        // Abandon ship (core)
} cspDebugPriority;


#endif

