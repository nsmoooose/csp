#ifndef __CONSOLECOMMANDS_H__
#define __CONSOLECOMMANDS_H__

#include "SDL.h"
#include "CON_console.h"
#include "CON_consolecommands.h"


void ListCommands(ConsoleInformation *console, char *string);
void Resize(ConsoleInformation *console, char *string);
void Move(ConsoleInformation *console, char *string);
void AlphaChange(ConsoleInformation *console, char *alpha);
void KillProgram(ConsoleInformation *console, char *String);
void DefaultCommand(ConsoleInformation *console, char *string);


#endif
