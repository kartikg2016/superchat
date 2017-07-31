#include "model.h"
#include "stdlib.h"

#include <algorithm>
#include <string.h>

using namespace std;
using namespace SuperChat;

Model::Model() : logged_in(false), is_running(true)
{
	chat_rooms[0] = ChatRoom(0, "PUBLIC");
	for (int i = 1; i < NUM_CHATROOMS; i++)
	{
		string name = "Chatroom #" + to_string(i);
		chat_rooms[i] = ChatRoom(i, name);
	}
}

void Model::updateUsers(vector<user> p_users)
{
	//printf("user size: %d\n", p_users.size());

	if (p_users.size() > 0)
	{
		for (user u : p_users)
		{
			User temp_user = User(u.nick, u.uuid, u.chatroom_idx);
			int index = findUserIndex(u.uuid);
			if (index == -1) //if user is new
			{
				users.push_back(temp_user);
			} 
			else
			{
				if(users[index].previous_chatroom_index != temp_user.getChatRoomIndex()) 
				{
					users[index].time_in_chatroom = 0;
				}
				users[index].setName(temp_user.getNickName()); //update even if nothing changed, update
				users[index].previous_chatroom_index = temp_user.getChatRoomIndex();
				users[index].setChatRoomIndex(temp_user.getChatRoomIndex());
				users[index].setStatus(OnlineStatus::Online);
				users[index].time_since_last_hb = -1; //trigger value to say to not update hb timer for when we go through all users and increment hb timer
			}
		}
	}

	for(User& u : users)
	{
		if(u.time_since_last_hb == -1) u.time_since_last_hb = 0; //if heartbeat was just received, set timer to 0
		else u.time_since_last_hb++;
	}
}

void Model::updateChatRooms(vector<chatroom> p_chat_rooms)
{
	if (p_chat_rooms.size() > 0)
	{
		for (chatroom cr : p_chat_rooms)
		{
			chat_rooms[cr.chatroom_idx].changeName(string(cr.chatroom_name));
		}
	}
}

void Model::updateMessages(vector<message> p_messages)
{
	if (p_messages.size() > 0)
	{
		for (message m : p_messages)
		{
			User temp_user = users[findUserIndex(m.uuid)];
			Message temp_message = Message(temp_user, m.chatroom_idx, string(m.message), m.cksum);
			if(temp_message.isCorrupted())
			{
				temp_message.content += " ~~~Message has been corrupted~~~";
			}
			//for testing
			//temp_message.content = "\"" + temp_message.content + "\"" + " calc cs: " + to_string(temp_message.calculateChecksum()) + " rec cs: " + to_string(temp_message.getChecksum());
			if(find(blacklist.begin(), blacklist.end(), m.uuid) == blacklist.end()) //if uuid isnt in blacklist, add message to chat room
			{
				chat_rooms[temp_message.getChatRoomIndex()].addMessage(temp_message);
			}
		}
	}
}

//returns index of user in users vector. If not in the vector, returns -1, so caller should push to vector
int Model::findUserIndex(unsigned long long uuid)
{
	for (int i = 0; i < (signed)users.size(); i++)
	{
		if (users[i].getUUID() == uuid)
		{
			return i;
		}
	}
	return -1;
}

string Model::calculateCurrentChatRoomName()
{
	return chat_rooms[users[0].getChatRoomIndex()].getName();
}

bool Model::isUserNew(unsigned long long uuid)
{
	for (User u : users)
	{
		if (u.getUUID() == uuid)
		{
			return false;
		}
	}
	return true;
}

void Model::updateAllChatRoomsTimeEmpty()
{
	for(ChatRoom& cr : chat_rooms)
	{
		if(calculateNumUsersInChatRoom(cr.getChatRoomIndex()) == 0)
		{
			cr.time_empty_seconds++;
			if(cr.isRenameable == false && cr.time_empty_seconds >= CHATROOM_RENAMEABLE_DURATION && cr.getChatRoomIndex() != 0) cr.isRenameable = true;
			if(cr.isRenameable)
			{
				cr.setName("________");//"________" is chosen value for renameable chatroom
			}
		}
		else 
		{
			cr.time_empty_seconds = 0;
		}
	}
}

/* Template for model after populateForTesting(-1)
0)  Name: me       ~ UUID: 0  ~ CRI: 0
1)  Name: Joe      ~ UUID: 1  ~ CRI: 0
2)  Name: Robert   ~ UUID: 2  ~ CRI: 0 
3)  Name: Rivka    ~ UUID: 3  ~ CRI: 0
4)  Name: Ramon    ~ UUID: 4  ~ CRI: 1
5)  Name: Stephani ~ UUID: 5  ~ CRI: 1
6)  Name: Jewel    ~ UUID: 6  ~ CRI: 1
7)  Name: Isaias   ~ UUID: 7  ~ CRI: 2
8)  Name: Murray   ~ UUID: 8  ~ CRI: 2
9)  Name: Darell   ~ UUID: 9  ~ CRI: 2
10) Name: Alyce    ~ UUID: 10 ~ CRI: 3
11) Name: Carylon  ~ UUID: 11 ~ CRI: 3
12) Name: Dona     ~ UUID: 12 ~ CRI: 3
13) Name: George   ~ UUID: 13 ~ CRI: 4 
14) Name: Doug     ~ UUID: 14 ~ CRI: 4
15) Name: Hannah   ~ UUID: 15 ~ CRI: 4 
*/
void Model::populateForTesting(int selected_user) //TODO
{
	//Create Fake Users
	string user_names[] = { "Joe", "Robert", "Rivka", "Ramon", "Stephani", "Jewel", "Isaias", "Murray", "Darell", "Alyce", "Carylon", "Dona", "George", "Doug", "Hannah" };
	unsigned long long uuids[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	unsigned long chat_room_indexes[] = { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4 };

	if (selected_user == -1)
	{
		User me = User("me", 0, 0);
		users.push_back(me);

		for (int i = 0; i < 15; i++)
		{
			users.push_back(User(user_names[i], uuids[i], chat_room_indexes[i]));
			users[i].time_online_seconds = i * 37;
		}

		//Create Fake Chat Messages
		string message_contents[] = { "Hello", "You Still there?", "I guess nice guys finish last." };
		for (int i = 0; i < 15; i++)
		{
			chat_rooms[users[i].getChatRoomIndex()].addMessage(Message(users[i], message_contents[0]));
			chat_rooms[users[i].getChatRoomIndex()].addMessage(Message(users[i], message_contents[1]));
			chat_rooms[users[i].getChatRoomIndex()].addMessage(Message(users[i], message_contents[2]));
		}
	}

	else
	{
		User me = User(user_names[selected_user], selected_user, 0);
		users.push_back(me);
	}
}

bool Model::isChatRoomEmpty(unsigned long chatroom_index)
{
	return getUsersInChatRoom(chatroom_index).size();
}

int Model::calculateNumUsersInChatRoom(unsigned long desired_chatroom_index)
{
	int count = 0;
	for (User& user : users)
	{
		if (user.getStatus() == OnlineStatus::Online && user.getChatRoomIndex() == desired_chatroom_index)
		{
			count++;
		}
	}

	return count;
}

//getters

vector<User> Model::getUsersInChatRoom(unsigned long desired_chatroom_index)
{
	vector<User> result;
	for (User user : users)
	{
		if (user.getStatus() == OnlineStatus::Online && user.getChatRoomIndex() == desired_chatroom_index)
		{
			result.push_back(user);
		}
	}
	return result;
}

unsigned long Model::findUserUUID(std::string desired_name, std::vector<User> users_in_chatroom)
{
	for(User u : users_in_chatroom)
	{
		if(strcmp(u.getNickName().c_str(), desired_name.c_str()) == 0) //if names are the same
		{
			return u.getUUID();
		}
	}

	return 0; //error value, not in current chatroom
}

void Model::addToBlacklist(unsigned long desired_uuid)
{
	blacklist.push_back(desired_uuid);
	blacklist.unique();
}

void Model::removeFromBlacklist(unsigned long desired_uuid)
{
	blacklist.remove(desired_uuid);
}
