#include "user.h"

#include <stdexcept>

using namespace std;

//default constructor for creating our local user at very beginning of program when nothing is know about him
User::User()
{

}

//this will likely only be used for when we receive heartbeat from new user and these are known
User::User(string p_nick_name, unsigned long long p_uuid, unsigned long p_chat_room_index) :
	uuid(p_uuid), chat_room_index(p_chat_room_index), online_status(Online), time_online_seconds(0), time_since_last_hb(0), time_in_chatroom(0), previous_chatroom_index(0)
	{
		nick_name = string(p_nick_name.c_str(), MAX_USER_NICK_SIZE);
	} //by default, a new user will be online and their duration is 0 seconds



user User::convertToOS()
{
	user result;
	strncpy(result.nick, nick_name.c_str(), MAX_USER_NICK_SIZE);
    result.uuid = uuid;
    result.chatroom_idx = chat_room_index;
    return result;
}

//setters
void User::setName(string desired_name)
{
	try
	{
		if (desired_name.length() > MAX_USER_NICK_SIZE)
		{
			throw runtime_error("Tried to set name that was too long.");
		}

		nick_name = desired_name;
	}

	catch (const std::exception& e)
	{
		//printf(e.what());
	}
}

void User::setChatRoomIndex(unsigned long index)
{
	try
	{
		if (index < 0 || index > NUM_CHATROOMS)
		{
			throw runtime_error("Tried to change to invalid chatroom index.");
		}

		chat_room_index = index;
	}

	catch (const std::exception& e)
	{
		//printf(e.what());
	}
}

void User::setStatus(OnlineStatus desired_status)
{
	online_status = desired_status;
}

//getters
//returns name without null terminated
string User::getNickName()
{
	string result;
	for(char c : nick_name)
	{
		if(c == '\0') break;
		else result += c;
	}
	return result;
}

unsigned long long User::getUUID()
{
	return uuid;
}

OnlineStatus User::getStatus()
{
	return online_status;
}

unsigned long User::getChatRoomIndex()
{
	return chat_room_index;
}

string User::timeToString()
{
	char result[10];
	int temp_time_seconds = time_online_seconds;
	int hours = temp_time_seconds / 3600;
	temp_time_seconds = temp_time_seconds % 3600;
	int minutes = temp_time_seconds / 60;
	int seconds = time_online_seconds % 60;
	sprintf(result, "%02d:%02d:%02d", hours, minutes, seconds);
	string string_result(result);
	return string_result;
}

string User::timeChatRoomToString()
{
	char result[10];
	int temp_time_seconds = time_in_chatroom;
	int hours = temp_time_seconds / 3600;
	temp_time_seconds = temp_time_seconds % 3600;
	int minutes = temp_time_seconds / 60;
	int seconds = time_in_chatroom % 60;
	sprintf(result, "%02d:%02d:%02d", hours, minutes, seconds);
	string string_result(result);
	return string_result;
}

//KARTIK
unsigned long long User::generateUUID() // Why static?
{
	boost::uuids::uuid uuidb = boost::uuids::random_generator()();
	unsigned long long result_uuid;
	memcpy ( &result_uuid, &uuidb, sizeof (result_uuid) );
	return result_uuid;
}

User User::loadUser(std::string desired_name)
{
	User found_user;
	ifstream read_User;
	string found_uuid;
	try
	{
		read_User.open(".superchat");
		found_user.nick_name = desired_name;
		if(!read_User)
		{
        	throw UserFileDoesNotExist();
    	}
    	else
    	{
    		getline(read_User, found_uuid, '~');
			std::string::size_type sz = 0;
			found_user.uuid = std::stoull(found_uuid, &sz, 0);
			read_User.close();
		}
	}
	catch (const UserFileDoesNotExist& e)
	{
		found_user.uuid = User::generateUUID(); //KEYWORD: maybe use User::
		read_User.close();
	}

	//Set other initialized variables
	found_user.nick_name = desired_name;
	found_user.online_status = OnlineStatus::Online;
	found_user.chat_room_index = 0;
	found_user.time_online_seconds = 0;
	found_user.time_since_last_hb = 0;
	found_user.time_in_chatroom = 0;
	return found_user;
}

 void User::saveUser() // Why static?
{
	ofstream write_User;
	write_User.open(".superchat");
	write_User << uuid << '~' << endl;
	write_User.close();
}