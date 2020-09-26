#ifndef _Command_h
#define _Command_h

#include "../shell.h"
#include "../CustomIO/CustomIO.h"

// ProcessCommand process command passed
// Args:
//  - tokensHolder contains tokenized command to be processed
void ProcessCommand(TokensHolder tokensHolder);

#endif