#include "message.h"

using namespace std;

//constructor for messages created locally, we calculate checksum ourself
Message::Message(User p_user, std::string p_content) : 
	author_nick_name(p_user.getNickName()), author_uuid(p_user.getUUID()), chat_room_index(p_user.getChatRoomIndex()), content(p_content)
	{
		content = string(p_content.c_str(), MESSAGE_LENGTH);
		contentToArray();
		checksum = calculateChecksum();
	}

//constructor for recevied messages when we are given a checksum
Message::Message(User p_user, unsigned long p_chat_room_index, std::string p_content, unsigned long long p_checksum) : 
	author_nick_name(p_user.getNickName()), author_uuid(p_user.getUUID()), chat_room_index(p_chat_room_index), content(p_content), checksum(p_checksum) 
	{
		content = string(p_content.c_str(), MESSAGE_LENGTH);
		contentToArray();
	}

message Message::convertToOS()
{
	message result;
	strcpy(result.message, content.c_str());
	result.uuid = author_uuid;
	result.cksum = checksum;
	result.chatroom_idx  = chat_room_index;
	return result;
}

bool Message::isCorrupted()
{
	if (checksum == 0) return false; //received checksum of 0, they didnt calculate it
	return !(calculateChecksum() == checksum);
}

void Message::contentToArray()
{
	for(int i = 0; i < 144; i++)
	{
		if(i < content.length())
		{
			content_array[i] = content[i];
		}

		else content_array[i] = '\0';
	}
}

unsigned long long Message::calculateChecksum()
{
	unsigned long long crc;
	crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (const unsigned char*) content_array, 144);
	return crc;
}

//getters
string Message::getContent()
{
	return content;
}

unsigned long long Message::getAuthorUUID()
{
	return author_uuid;
}

unsigned long Message::getChatRoomIndex()
{
	return chat_room_index;
}

unsigned long long Message::getChecksum()
{
	return checksum;
}

string Message::getAuthorNickName()
{
	return author_nick_name;
}