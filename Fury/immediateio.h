#pragma once
#include "util.h"
#include <string>
/**Pops up an error message to the screen.*/
void Error(std::string err);

/**Pops up an warning message to the screen.*/
void Warning(std::string err);

/**Prints an information message.*/
void Inform(std::string err);

/**Prints an error message without crashing if no window functionality.*/
void ErrorSafe(std::string err); //does not require window to be open