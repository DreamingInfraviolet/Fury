#pragma once

class Event
{
public:

	enum Type {Close, Resize, Empty, Undefined};


	Event() : type(Empty) {}

	Type type;

	struct ResizeEvent
	{
		int width,height;
	};


	union
	{
		ResizeEvent size;
	};
};