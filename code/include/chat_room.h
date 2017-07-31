#ifndef CHATROOM_H
#define CHATROOM_H 2016

#include <deque>
#include <string>

#include "constants.h"
#include "message.h"
#include "user.h"
#include "ddsincludes.h"

using namespace SuperChat;

class ChatRoom
{
public:
	ChatRoom();
	ChatRoom(unsigned long p_chat_room_index, std::string p_chat_room_name);

	std::deque<Message> message_history; //Using deque because it allows easy push_back, pop_front for history
	int time_empty_seconds;

	void changeName(std::string desired_name);
	void addMessage(Message message);
	chatroom convertToOS();

	bool isRenameable;

	//getters
	std::string getName();
	unsigned long getChatRoomIndex();

	//setters
	void setName(std::string desired_name);

private:
	unsigned long chat_room_index; //DISCUSION: could be int?
	std::string chat_room_name;
};

#endif
