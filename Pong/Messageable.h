#pragma once

// data types passed with Messageable::message() can vary
// depending on which class receives the message. See comments
// within each classes implementation of message() for clarification
enum class MessageId {
	NONE = -1,
	SET_Y_VELOCITY,
	HANDLE_COLLISION,
	PADDLE_BOUNCE,
	X_OUT_OF_SCREEN_BOUNDS,
	Y_OUT_OF_SCREEN_BOUNDS,
	UPDATE_SCORE,
	COUNT
};

// My intention with this class was to use it like a Java interface
class Messageable {
public:
	virtual ~Messageable() = default;
	// @param reply is to be used as an OUT parameter
	virtual bool message(const MessageId messageId, 
						 const void* data = nullptr, 
						 void* reply = nullptr) = 0;
};