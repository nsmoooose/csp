#ifndef CON_consolecommands_H
#define CON_consolecommands_H

#include "CON_console.h"

#ifdef __cplusplus
extern "C" {
#endif

void    CON_AddDefaultCommand(void (*CommandCallback)(ConsoleInformation *console, char *Parameters));

#ifdef __cplusplus
};
#endif

#endif
