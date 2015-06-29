#pragma once
#include "message.h"
class EventMaster
{
public:
	EventMaster(){}

	bool initialise(){return true;}

	bool sendWindowMessage(WindowMessage* message);
};