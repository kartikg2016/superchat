#ifndef OSDDS_IO_H
#define OSDDS_IO_H 2016

#include "ddsincludes.h"
#include "constants.h"

using namespace DDS;
using namespace SuperChat;

typedef std::vector<user> user_list_t;
typedef std::vector<message> message_list_t;
typedef std::vector<chatroom> chatroom_list_t;

class chatroom_data
{
public:
	chatroom_data (char *topic_name);
	~chatroom_data ();
	void send ( chatroom messageInstance );
	void recv ( chatroom_list_t* chatroomList );

private:
	DDSEntityManager em;
	chatroomDataWriter_var Writer;
	chatroomDataReader_var Reader;
};

class message_data
{
public:
	message_data (char *topic_name);
	~message_data ();
	void send ( message messageInstance );
	void recv ( message_list_t* messageList );

private:
	DDSEntityManager em;
	messageDataWriter_var Writer;
	messageDataReader_var Reader;
};

class user_data
{
public:
	user_data (char *topic_name);
	~user_data ();
	void send ( user userInstance );
	void recv ( user_list_t* userList );

private:
	DDSEntityManager em;
	userDataWriter_var Writer;
	userDataReader_var Reader;
};

#endif