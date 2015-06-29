#pragma once

class MessageType
{
		enum MessageTypeEnum {WindowClose};
};

typedef int u32;

class BaseMessage
{
protected:
	BaseMessage(u32 destObjectId, MessageType type)
	{
		mDestObjectID=destObjectId;
		mType=type;
	}

	u32 mDestObjectID;
	MessageType mType;


public:
	u32 getDestObjectID()
	{
		return mDestObjectID;
	}

	MessageType getType()
	{
		return mType;
	}
};

class WindowMessage : public BaseMessage
{
public:
	WindowMessage(MessageType type) : BaseMessage(0, type) {}
};