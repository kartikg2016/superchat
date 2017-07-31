#ifndef MODEL_H
#define MODEL_H 2016

#include <vector>
#include <list>

#include "chat_room.h"
#include "user.h"
#include "message.h"
#include "ddsincludes.h"


class Model
{
public:
	Model();

	ChatRoom chat_rooms[10];
	std::vector<User> users; //TESTING: can we have our user be first in the vector?
	std::list<unsigned long> blacklist; //vector of UUIDs 
	void addToBlacklist(unsigned long desired_uuid);
	void removeFromBlacklist(unsigned long desired_uuid);

	void updateAllChatRoomsTimeEmpty();

	bool logged_in;
	bool is_running;
	//vectors that will be accessed in OS loop of outgoing things to be published
	std::vector<ChatRoom> chat_room_outbox;
	std::vector<Message> message_outbox;

	void populateForTesting(int selected_user);
	bool isUserNew(unsigned long long uuid); //TODO: logic for checking this when get heartbeat, if new, add to users.

	void updateUsers(std::vector<SuperChat::user> p_users);
	int findUserIndex(unsigned long long uuid);
	unsigned long findUserUUID(std::string desired_name, std::vector<User> users_in_chatroom);

	void updateChatRooms(std::vector<SuperChat::chatroom> p_chat_rooms);
	void updateMessages(std::vector<SuperChat::message> p_messages);
	int calculateNumUsersInChatRoom(unsigned long desired_chatroom_index);
	std::string calculateCurrentChatRoomName();

	//getters
	std::vector<User> getUsersInChatRoom(unsigned long desired_chatroom_index);
	bool isChatRoomEmpty(unsigned long chatroom_index);
};

#endif