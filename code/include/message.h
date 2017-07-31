#ifndef MESSAGE_H
#define MESSAGE_H 2016

#include <string>
#include <zlib.h>

#include "constants.h"
#include "user.h"
#include "ddsincludes.h"

using namespace SuperChat;

class Message
{
public:
	Message(User p_user, std::string p_content);
	Message(User p_user, unsigned long p_chat_room_index, std::string p_content, unsigned long long p_checksum);
	bool isCorrupted();

	message convertToOS();

	//getters
	std::string getContent();
	unsigned long long getAuthorUUID();
	unsigned long getChatRoomIndex();
	unsigned long long getChecksum();
	std::string getAuthorNickName();
	//TODO: .toStruct for OpenSplice

	char content_array[144];
	std::string content;
	void contentToArray();
		unsigned long long calculateChecksum();

private:
	unsigned long long author_uuid;
	unsigned long chat_room_index; 
	unsigned long long checksum; 
	std::string author_nick_name; //TODO: lookup name every time draw message instead of storing name


};

#endif