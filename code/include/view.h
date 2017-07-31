#ifndef VIEW_H
#define VIEW_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <panel.h>
#include <constants.h>

#include "model.h"

//Global Data
namespace GUI_DATA
{
	enum class Window { Login, Chatroom, Settings };
}

extern std::mutex model_mutex;
extern Model chat_building;

using namespace std;
using namespace GUI_DATA;

class View 
{
private:
	const int BSP = KEY_LEFT; // Backspace 
	const int ExitFKey = KEY_F(6); // F6 Function Key for Cancel

public:
	//CHANGE: Constructor; basically gives you reference to the model that controller is using so any changes you make to it here will be reflected in the model used by controller class.
	View() : current_window(Window::Login) {}; //this assigns model in this class to the model in controller when an instance of this class is made in controller.

	int current_menu_index; // global attribute for current chat room from menu
	bool show_highlight_index = false;
	vector<char> message_buffer; //global attribute for message the user types

	Window current_window;

	WINDOW* MakeWindow(int Height, int Width, int Yposition, int Xposition, string Title)
	{
		//Create the Window
		WINDOW* returnWindow = newwin(Height, Width, Yposition, Xposition);

		//Draw Borders around the window
		box(returnWindow, 0, 0);

		//Print the title if there is one
		if (Title != "")
		{
			mvwchgat(returnWindow, 1, 1, Width - 2, A_NORMAL, 1, NULL);
			wattron(returnWindow, A_BOLD | COLOR_PAIR(1));
			mvwprintw(returnWindow, 1, Width / 2 - Title.length() / 2, Title.c_str());
			wattroff(returnWindow, A_BOLD | COLOR_PAIR(1));
		}

		refresh();

		return returnWindow;
	}

	bool IsNumberOrLetter(char input)
	{
		if (input >= '0' && input <= '9')
			return true;
		if (input >= 'A' && input <= 'Z')
			return true;
		if (input >= 'a' && input <= 'z')
			return true;
		return false;
	}

	bool IsNumberOrLetterOrSymbol(char input)
	{
		if (input >= ' ' && input <= '~')
			return true;
		return false;
	}

	//- - - - - - - - - - - SETTINGS WINDOW - - - - - - - - - - -

	void Settings_TopBar()
	{
		//Make the window
		WINDOW *window = MakeWindow(LINES, COLS, 0, 0, "");

		//Print the text inside top bar window
		wattron(window, COLOR_PAIR(8));
		mvwprintw(window, 1, 1, "ENTER - Save & Return\tLEFT ARROW - Backspace\tF4 - Change UserNick\tF5 - Change Chatroom Name\tF6 - Cancel & Return");
		wattroff(window, COLOR_PAIR(8));

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void Settings_ChangeUserName(string new_nick)
	{
		//Make the window
		int window_width = COLS / 2;

		WINDOW *window = MakeWindow(10, window_width - 1, 3, 1, "Change User Nick");

		//Print Columns "Current UserName" and "New Username"
		wattron(window, A_BOLD);
		mvwprintw(window, 2, window_width / 2 - 8, "Current User Nick");
		mvwprintw(window, 6, window_width / 2 - 6, "New User Nick");
		wattroff(window, A_BOLD);

		//Print the Current Username
		wattron(window, COLOR_PAIR(9));
		model_mutex.lock();
		string current_user_name = chat_building.users[0].getNickName(); //CHANGE: access through model
		model_mutex.unlock();
		mvwprintw(window, 3, window_width / 2 - current_user_name.length() / 2, current_user_name.c_str()); //CHANGE: access through model
		wattroff(window, COLOR_PAIR(9));

		//Input box for New Username
		mvwchgat(window, 7, window_width / 2 - 10, 20, A_NORMAL, 4, NULL);
		mvwprintw(window, 8, window_width / 2 - 8, "%d characters only", MAX_USER_NICK_SIZE);

		wattron(window, COLOR_PAIR(4));
		mvwprintw(window, 7, window_width / 2 - new_nick.length() / 2, new_nick.c_str());
		wattroff(window, COLOR_PAIR(4));

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void Settings_ChangeChatroomName(string new_chatroom_name)
	{
		//Make the window
		int window_width = COLS / 2;

		WINDOW *window = MakeWindow(10, window_width - 1, 3, window_width, "Change Chatroom Name");

		//Print Columns "Current UserName" and "New Username"
		wattron(window, A_BOLD);
		mvwprintw(window, 2, window_width / 2 - 10, "Current Chatroom Name");
		mvwprintw(window, 6, window_width / 2 - 8, "New Chatroom Name");
		wattroff(window, A_BOLD);

		//Print the Current Username
		wattron(window, COLOR_PAIR(9));
		model_mutex.lock();
		string current_user_chatroom_name = chat_building.calculateCurrentChatRoomName(); //CHANGE: access through model
		model_mutex.unlock();
		mvwprintw(window, 3, window_width / 2 - current_user_chatroom_name.length() / 2, current_user_chatroom_name.c_str()); //CHANGE: access through model
		wattroff(window, COLOR_PAIR(9));

		//Input box for New Chatroom name
		mvwchgat(window, 7, window_width / 2 - MAX_CHATROOM_NAME_SIZE / 2, MAX_CHATROOM_NAME_SIZE, A_NORMAL, 4, NULL);
		mvwprintw(window, 8, window_width / 2 - 7, "%d characters only", MAX_CHATROOM_NAME_SIZE);

		wattron(window, COLOR_PAIR(4));
		mvwprintw(window, 7, window_width / 2 - new_chatroom_name.length() / 2, new_chatroom_name.c_str());
		wattroff(window, COLOR_PAIR(4));

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void Settings_AllUsers()
	{
		//Make the window
		int window_height = LINES - 14;
		WINDOW *window = MakeWindow(window_height, COLS - 2, 13, 1, "All Users");

		//Create the Columns "Users", "Status", "Chatroom", "Time Online"
		wattron(window, A_BOLD);
		mvwprintw(window, 2, 1, "%-8s\t\t\t%-8s\t\t\t%-25s\t\t\t%s", "User", "Status", "Chatroom", "Time Online");
		wattroff(window, A_BOLD);

		//Print the User Profiles
		model_mutex.lock();
		for (unsigned int i = 0; i < chat_building.users.size(); i++) //CHANGE: access through model
		{
			User temp_user = chat_building.users[i];  //CHANGE: access through model
			//Print User Name
			mvwprintw(window, 3 + i, 1, "%-8s\t\t\t", temp_user.getNickName().c_str());  //CHANGE: access through model
			if (temp_user.getStatus() == Online)
			{
				//Print Online Status
				wattron(window, COLOR_PAIR(2));
				wprintw(window, "%-8s\t\t\t", "Online");
				wattroff(window, COLOR_PAIR(2));
				//Print ChatroomName and Time Online
				wprintw(window, "%-25s\t\t\t%s", chat_building.chat_rooms[temp_user.getChatRoomIndex()].getName().c_str(), temp_user.timeToString().c_str()); //CHANGE: access through model and added time functionality
			}
			else
			{
				//Print Offline Status
				wattron(window, COLOR_PAIR(3));
				wprintw(window, "%-8s\t\t\t", "Offline");
				wattroff(window, COLOR_PAIR(3));
				//Print Blanks for ChatroomName and Time Online Fields
				wprintw(window, "%-25s\t\t\t%s", "----------", "----------");
			}
		}
		model_mutex.unlock();

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void Settings_Draw()
	{
		model_mutex.lock();
		string tempUserNick = chat_building.users[0].getNickName(), tempChatroomName = chat_building.calculateCurrentChatRoomName();
		vector<char> new_user_nick(tempUserNick.begin(), tempUserNick.end()), new_chatroom_name(tempChatroomName.begin(), tempChatroomName.end());  //CHANGE: access through model
		model_mutex.unlock();

		Settings_TopBar();
		Settings_ChangeUserName("New Nick");
		Settings_ChangeChatroomName("New Chatroom Name");
		Settings_AllUsers();

		int SaveAndReturn = ENTER; //Enter key
		int ChangeUserNickFKey = KEY_F(4);
		int ChangeChatroomNameFKey = KEY_F(5);

		int window_char, input;
		input = getch();
		while ((window_char = input))
		{
			//Change the User's Name
			if (window_char == ChangeUserNickFKey)
			{
				new_user_nick.clear();
				Settings_ChangeUserName(string(new_user_nick.data(), new_user_nick.size()));

				input = getch();
				while (input != SaveAndReturn && input != ChangeUserNickFKey && input != ChangeChatroomNameFKey && input != ExitFKey)
				{
					if (IsNumberOrLetter(input) && new_user_nick.size() < MAX_USER_NICK_SIZE)
					{
						new_user_nick.push_back(input);	
					}
					else if (input == BSP && new_user_nick.size() > 0)
					{
						new_user_nick.pop_back();
					}

					Settings_ChangeUserName(string(new_user_nick.data(), new_user_nick.size()));
					input = getch();
				}
			}
			//Change the Chatroom Name
			else if (window_char == ChangeChatroomNameFKey)
			{
				new_chatroom_name.clear();
				Settings_ChangeChatroomName(string(new_chatroom_name.data(), new_chatroom_name.size()));

				input = getch();
				while (input != SaveAndReturn && input != ChangeUserNickFKey && input != ChangeChatroomNameFKey && input != ExitFKey)
				{
					if (IsNumberOrLetter(input) && new_chatroom_name.size() < MAX_CHATROOM_NAME_SIZE)
					{
						new_chatroom_name.push_back(input);
					}
					else if (input == BSP && new_chatroom_name.size() > 0)
					{
						new_chatroom_name.pop_back();
					}
					
					Settings_ChangeChatroomName(string(new_chatroom_name.data(), new_chatroom_name.size()));
					input = getch();
				}
			}
			else if (window_char == ExitFKey)
			{
				current_window = Window::Chatroom;
				break;
			}
			else if (window_char == SaveAndReturn) //CHANGE: changed logic so we don't send chatroom info to other computers if name hasn't changed
			{
				current_window = Window::Chatroom;
				model_mutex.lock();

				//update username even if it didnt change because it isn't too slower than checking. don't publish manually since it will be included in heartbeat
				if (new_user_nick.size() > 0 && chat_building.users[0].getNickName() != string(new_user_nick.data(), new_user_nick.size())) //if user name actually changed
				{
					string alert_string = "*** " + chat_building.users[0].getNickName() + " CHANGED NAME TO \"" + string(new_user_nick.data(), new_user_nick.size()) + "\" ***";
					Message alert_message = Message(chat_building.users[0], alert_string);
					chat_building.message_outbox.push_back(alert_message);

					chat_building.users[0].setName(string(new_user_nick.data(), new_user_nick.size()));
				}

				ChatRoom& current_chat_room = chat_building.chat_rooms[chat_building.users[0].getChatRoomIndex()];
				//update and publish chat room name if user actually changed chat room name
				if (current_chat_room.getName() != string(new_chatroom_name.data(), new_chatroom_name.size()))
				{
					//Check if the Chatroom is not Public and is renameable, if it is not then you can rename it
					if (current_chat_room.getChatRoomIndex() != 0 && current_chat_room.isRenameable)
					{
						string alert_string = "*** CHANGED " + current_chat_room.getName() + " TO \"" + string(new_chatroom_name.data(), new_chatroom_name.size()) + "\" ***";
						Message alert_message = Message(chat_building.users[0], alert_string);
						chat_building.message_outbox.push_back(alert_message);

						current_chat_room.setName(string(new_chatroom_name.data(), new_chatroom_name.size()));
						chat_building.chat_room_outbox.push_back(current_chat_room);
					}
				}
				model_mutex.unlock();
				break;
			}
			else
				input = getch();
		}
	}

	//- - - - - - - - - - - CHAT MESSAGE WINDOW - - - - - - - - - - -

	void ChatMessage_TopBar()
	{
		//Make the window
		WINDOW *window = MakeWindow(LINES, COLS, 0, 0, "");

		//Print the text inside top bar window
		wattron(window, COLOR_PAIR(11));
		mvwprintw(window, 1, 1, "ENTER - Send Message\tLEFT ARROW - Backspace\tF4 - Chatrooms Menu\tF5 - Settings\tF6 - Logout & Exit");
		wattroff(window, COLOR_PAIR(11));

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void ChatMessage_Chatrooms(int SelectedIndex)
	{
		int chatHeight = LINES / 2, chatWidth = 40;
		string roomNames[10];
		int roomStats[10];

		//Create the Window
		WINDOW *window = MakeWindow(chatHeight - 1, chatWidth, 3, 1, "Chatrooms");

		//Display the Chatroom Footer
		mvwprintw(window, chatHeight - 3, chatWidth / 2 - 12, "Enter - Switch Classrooms");
		model_mutex.lock();
		//CHANGE: Moved functionality for getting info into model
		for (int i = 0; i < 10; i++)
		{
			roomNames[i] = chat_building.chat_rooms[i].getName();
			roomStats[i] = chat_building.calculateNumUsersInChatRoom(i);
		}

		for (unsigned long i = 0; i < 10; i++)
		{
			mvwprintw(window, 2 + i, 1, "%s", roomNames[i].c_str());
			//if there is more than one user in the chatroom
			if (roomStats[i] > 1)
			{	
				mvwprintw(window, 2 + i, 1, "%s", roomNames[i].c_str());
				mvwprintw(window, 2 + i, 30, "%d users", roomStats[i]);
			}
			//if there is only one user in the chatroom
			else if (roomStats[i] == 1)
			{
				mvwprintw(window, 2 + i, 1, "%s", roomNames[i].c_str());
				mvwprintw(window, 2 + i, 30, "%d user", roomStats[i]);
			}
			//there are no users in the chatroom
			else
			{
				mvwprintw(window, 2 + i, 1, "%s", roomNames[i].c_str());	
				mvwchgat(window, 2 + i, 1, chatWidth - 2, A_NORMAL, 8, NULL);
				wattron(window, A_NORMAL | COLOR_PAIR(8));
				mvwprintw(window, 2 + i, 30, "0 users");
				wattroff(window, A_NORMAL | COLOR_PAIR(8));
			}

			//highlight the selected user
			if ((unsigned)SelectedIndex == i && show_highlight_index)
			{
				mvwprintw(window, 2 + i, 1, "%s", roomNames[i].c_str());
				mvwchgat(window, 2 + i, 1, chatWidth - 2, A_NORMAL, 9, NULL);
			}
			//bold the chatroom the user is in
			else if (chat_building.users[0].getChatRoomIndex() == i)
			{
				mvwprintw(window, 2 + i, 1, "%s", roomNames[i].c_str());
				mvwchgat(window, 2 + i, 1, chatWidth - 2, A_NORMAL, 2, NULL);
			}

		}

		model_mutex.unlock();

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void ChatMessage_Users()
	{
		//Create the Window
		int userHeight = LINES / 2, userWidth = 40;
		model_mutex.lock();
		string header = chat_building.calculateCurrentChatRoomName() + " Users"; //CHANGE: access through model
		WINDOW* window = MakeWindow(userHeight - 2, userWidth, (userHeight + 2), 1, header);

		//Only show the current users in the chatroom
		vector<User> usersInSameChatroom = chat_building.getUsersInChatRoom(chat_building.users[0].getChatRoomIndex()); //CHANGE: moved functionality into model
		for (unsigned int i = 0; i < usersInSameChatroom.size(); i++)
		{
			//Print the User's Name and time in chatroom
			mvwprintw(window, 2 + i, 2, usersInSameChatroom[i].getNickName().c_str()); //CHANGE: .name to .getNickName()
			mvwprintw(window, 2 + i, 30, usersInSameChatroom[i].timeChatRoomToString().c_str());
		}

		//Print the text inside the Users Window
		mvwprintw(window, (userHeight - 4), userWidth / 2 - 7, "F5 - All Users");

		model_mutex.unlock();

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void ChatMessage_ChatHistory()
	{
		int winWidth = COLS - 42;
		int winHeight = LINES - 13;

		//Initialize the Window
		model_mutex.lock();
		WINDOW *window = MakeWindow(winHeight, winWidth, 3, 41, chat_building.calculateCurrentChatRoomName()); //CHANGE:

		ChatRoom& current_chat_room = chat_building.chat_rooms[chat_building.users[0].getChatRoomIndex()]; //CHANGE:

		//Print the Chat History
		for (unsigned int i = 0; i < current_chat_room.message_history.size() && i < MAX_CHAT_HISTORY; i++)
		{
			mvwprintw(window, 2 * i + 2, 2, "%s:", current_chat_room.message_history[i].getAuthorNickName().c_str()); //CHANGE: access through model
			mvwprintw(window, 2 * i + 3, 5, "%s", current_chat_room.message_history[i].getContent().c_str()); //CHANGE: access through model
		}
		model_mutex.unlock();

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void ChatMessage_SendMessage(string Message)
	{
		int winHeight = 9;
		int winWidth = COLS - 42;

		//Initialize the Window
		WINDOW *window = MakeWindow(winHeight, winWidth, LINES - winHeight - 1, 41, "Send a Message");

		//Print the "Send a Message" Footer
		mvwprintw(window, winHeight - 2, 1, "Enter - Send Message");
		mvwprintw(window, winHeight - 2, winWidth - 26, "%d Characters Remaining", MESSAGE_LENGTH - Message.length());

		//Print the message
		mvwprintw(window, 2, 1, Message.c_str());

		//Refresh the Window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	int ChatMessage_Draw(long chatroom_index)
	{
		//Assign the user to the passed chatroomIndex
		model_mutex.lock();
		chat_building.users[0].setChatRoomIndex(chatroom_index); //CHANGE: access through model
		model_mutex.unlock();

		//Draw the Chatroom Windows
		ChatMessage_TopBar();
		ChatMessage_Users();
		ChatMessage_Chatrooms(-1);
		ChatMessage_ChatHistory();
		ChatMessage_SendMessage("");

		//Navigation
		current_menu_index = 0;
		int window_char, sub_char;
		int ChangeChatroomFKey = KEY_F(4);
		int SettingsFKey = KEY_F(5);

		sub_char = ENTER;
		while ((window_char = sub_char))
		{
			//Change the Chatroom
			if (window_char == ChangeChatroomFKey)
			{
				show_highlight_index = true;
				model_mutex.lock();
				current_menu_index = chat_building.users[0].getChatRoomIndex(); // jump to current chat room on list
				model_mutex.unlock();
				//Draw initial
				ChatMessage_Chatrooms(current_menu_index);

				//Get keyboard for up, down, or other function keys
				do
				{
					sub_char = getch();
					switch (sub_char)
					{
						case KEY_UP:
							if (current_menu_index >= 1)
								current_menu_index--;
							else
								current_menu_index = NUM_CHATROOMS - 1;
							ChatMessage_Chatrooms(current_menu_index);
							break;

						case KEY_DOWN:
							if (current_menu_index == NUM_CHATROOMS - 1)
								current_menu_index = 0;
							else current_menu_index++;
							ChatMessage_Chatrooms(current_menu_index);
							break;

						case ENTER:
							//Change user to this current_menu_index of chatroom
							show_highlight_index = false;
							return current_menu_index;

						//If the user types something unexpected, then take them to the 'Send a Message' window
						default:
							sub_char = ENTER;
							break;
					}
				} while (sub_char == KEY_UP || sub_char == KEY_DOWN);
				show_highlight_index = false;
			}

			//Go to the 'Send Message' Window
			else if (window_char == ENTER)
			{
				//Clear the screen again
				string spaces;
				for (int i = 0; i < MESSAGE_LENGTH; i++) spaces += " ";

				//Clear the message and initialize the characters
				message_buffer.clear();
				sub_char = getch();
				while (sub_char != ChangeChatroomFKey && sub_char != SettingsFKey && sub_char != ExitFKey)
				{
					if (IsNumberOrLetterOrSymbol(sub_char) && message_buffer.size() < MESSAGE_LENGTH)
					{
						message_buffer.push_back(sub_char);
					}
					else if (sub_char == ENTER)
					{
						if (message_buffer.size() > 0)
						{	
							//if message is a command
							if(message_buffer.at(0) == '/' && ( (message_buffer.at(1) == 'm' && message_buffer.at(2) == 'u' && message_buffer.at(3) == 't' && message_buffer.at(4) == 'e' && message_buffer.at(5) == ' ' )  
								|| (message_buffer.at(1) == 'u' && message_buffer.at(2) == 'n' && message_buffer.at(3) == 'm' && message_buffer.at(4) == 'u' && message_buffer.at(5) == 't' && message_buffer.at(6) == 'e' && message_buffer.at(7) == ' ' ) ) )
							{
								string content = "", command = "", command_arg = "";
								for(char c : message_buffer) content += c;
								unsigned int i = 0;

								//get command until space character
								for(char c : content)
								{
									i++;
									if(c == ' ') break;
									else command += c;
								}
								//get command arg after space to end of string
								for( ; i < content.length(); i++)
								{
									command_arg += content[i];
								}

								model_mutex.lock();
								unsigned long uuid_ban = chat_building.findUserUUID(command_arg, chat_building.getUsersInChatRoom(chat_building.users[0].getChatRoomIndex()));
								if(uuid_ban != 0)
								{
									if(command.compare("/mute") == 0)
									{
										chat_building.addToBlacklist(uuid_ban);
										string mute_string = "*** MUTED " + command_arg + " ***";
										Message mute_message = Message(chat_building.users[0], mute_string);
										chat_building.message_outbox.push_back(mute_message);
									}

									else if(command.compare("/unmute") == 0)
									{
										chat_building.removeFromBlacklist(uuid_ban);
										string mute_string = "*** UNMUTED " + command_arg + " ***";
										Message mute_message = Message(chat_building.users[0], mute_string);
										chat_building.message_outbox.push_back(mute_message);
									}
								}
								message_buffer.clear();
								model_mutex.unlock();
							}

							else
							{
							model_mutex.lock();
							Message newMessage = Message(chat_building.users[0], string(message_buffer.data(), message_buffer.size()));
							chat_building.message_outbox.push_back(newMessage);

							message_buffer.clear();
							model_mutex.unlock();
							}

							//Redraw the Chatmessage History
							ChatMessage_ChatHistory();
						}
					}
					else if (sub_char == BSP && message_buffer.size() > 0)
					{
						message_buffer.pop_back();
					}

					ChatMessage_SendMessage(string(message_buffer.data(), message_buffer.size()));
					sub_char = getch();
				}
			}

			//Go to Settings Window
			else if (window_char == SettingsFKey)
			{
				current_window = Window::Settings;
				Settings_Draw();
				model_mutex.lock();
				unsigned long return_index = chat_building.users[0].getChatRoomIndex();
				model_mutex.unlock();
				return return_index;
			}

			//Go to the Logout Window
			else if (window_char == ExitFKey)
			{
				current_window = Window::Login;
				model_mutex.lock();
				chat_building.logged_in = false;
				model_mutex.unlock();
				return -1;
			}

			//Get a new key from the user
			else
			{
				sub_char = getch();
			}
		}

		refresh();
		return 0;
	}

	//- - - - - - - - - - - START WINDOW - - - - - - - - - - -

	void StartScreen_TopBorder()
	{
		//Make the window
		WINDOW* window = MakeWindow(3, COLS, 0, 0, "");

		//Print the text inside the window
		wattron(window, COLOR_PAIR(9));
		mvwprintw(window, 1, 1, "ENTER - Login \t\t LEFT ARROW - Backspace \t F6 - Exit ");
		wattroff(window, COLOR_PAIR(9));

		//Refresh the window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void StartScreen_Username(string userName)
	{
		int textbox_y = LINES/2 - 10;
		int textbox_x = COLS/2;

		//Make the window
		WINDOW* window = MakeWindow(LINES - 3, COLS, 3, 0, "User Nick");

		//Stuff  
		wattron(window, COLOR_PAIR(11));
		mvwprintw(window,LINES-10,1,"SuperChat v1.01");
		wattroff(window, COLOR_PAIR(11));

		wattron(window, COLOR_PAIR(8));
		mvwprintw(window,LINES-9,1,"Last Updated - 04/23/2017");
		wattroff(window, COLOR_PAIR(8));
		
		wattron(window, COLOR_PAIR(2));
		mvwprintw(window,LINES-8,1,"Customer - Jimmie Bud Davis");
		wattroff(window, COLOR_PAIR(2));

		wattron(window, COLOR_PAIR(3));
		mvwprintw(window,LINES-7,1,"Development Team - Joe Cloud, Brandon Chase, Robert Brady, Kartik Gupta and Pavanaj Biyani");
		wattroff(window, COLOR_PAIR(3));

		wattron(window, COLOR_PAIR(10));
		mvwprintw(window,LINES-6,1,"Development Tools - OpenSplice and NCurses");
		wattroff(window, COLOR_PAIR(10));

		wattron(window, COLOR_PAIR(9));
		mvwprintw(window,LINES-5,1,"Platform - Linux Based OS");
		wattroff(window, COLOR_PAIR(9));

		//Print the limits
		mvwprintw(window, textbox_y+1, textbox_x - 8, "8 Character Limit");
		mvwchgat(window, textbox_y, textbox_x - 10, 20, A_NORMAL, 4, NULL);

		//print user name
		wattron(window, COLOR_PAIR(4));
		mvwprintw(window, textbox_y, textbox_x - 10, userName.c_str());
		wattroff(window, COLOR_PAIR(4));

		//Refresh the window
		wrefresh(window);
		//Delete Window
		delwin(window);
	}

	void StartScreen_Draw()
	{
		//string user_nick = "";
		vector<char> user_nick2;
		int input_char;
		long chat_room_index = 0;

		StartScreen_TopBorder();
		StartScreen_Username(string(user_nick2.data(), user_nick2.size()));

		//Draw the Login Window
		input_char = getch();
		while (input_char != ExitFKey)
		{
			if (IsNumberOrLetter(input_char) && user_nick2.size() < MAX_USER_NICK_SIZE)
			{
				user_nick2.push_back(input_char);
				//StartScreen_TopBorder();
				StartScreen_Username(string(user_nick2.data(), user_nick2.size()));
			}
			else if (input_char == ENTER) // enter key
			{
				//Create User
				model_mutex.lock();
				User temp_user = User::loadUser(string(user_nick2.data(), user_nick2.size()));
				temp_user.saveUser();
				chat_building.users.push_back(temp_user);
				chat_building.logged_in = true;
				model_mutex.unlock();
				current_window = Window::Chatroom;
				do
				{
					//Send the user to the public chatroom
					chat_room_index = ChatMessage_Draw(chat_room_index);
					if (chat_room_index == -1)
					{
						input_char = ExitFKey; // Exit and Log Out
						return;				
					}
					else if (chat_room_index < -1 && chat_room_index > 9)
					{
						chat_room_index = 0; // Public Chatroom		
					}		
				} while (chat_room_index != -1);
				StartScreen_Username("");
			}
			else if (input_char == BSP && user_nick2.size() > 0)
			{
				user_nick2.pop_back();
				StartScreen_Username(string(user_nick2.data(), user_nick2.size()));
			}

			input_char = getch();
		}
	}


	//- - - - - - - - - - - MAIN - - - - - - - - - - -
	void RefreshGUI()
	{
		if (current_window == Window::Settings) // Refresh Windows on Settings Screen
		{
			Settings_AllUsers();
		}

		else if (current_window == Window::Chatroom) // Refresh Windows on Chat Message Screen
		{
			ChatMessage_TopBar();

			ChatMessage_ChatHistory();

			ChatMessage_Users();

			ChatMessage_Chatrooms(current_menu_index);

			ChatMessage_SendMessage(string(message_buffer.data(), message_buffer.size()));
		}
	}

	void StartGUI()
	{
		//initialize ncurses
		initscr();
		start_color();
		cbreak();
		keypad(stdscr, TRUE);
		noecho();

		//Create the Color Pairs - Global Throughout Program
		init_pair(1, COLOR_BLACK, COLOR_WHITE);	 //Inverse
		init_pair(2, COLOR_GREEN, COLOR_BLACK); //Online
		init_pair(3, COLOR_RED, COLOR_BLACK);   //Offline
		init_pair(4, COLOR_WHITE, COLOR_BLUE);	//Textbox
		init_pair(5, COLOR_WHITE, COLOR_BLACK); //Normal
		init_pair(7, COLOR_WHITE, COLOR_YELLOW); //Element of Interest
		init_pair(8, COLOR_YELLOW, COLOR_BLACK); //Inactive chatroom
		init_pair(9, COLOR_BLUE, COLOR_BLACK); // Item Selected form list
		init_pair(10, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(11, COLOR_CYAN, COLOR_BLACK);
		init_pair(12, COLOR_BLUE, COLOR_WHITE); //selected Index

		StartScreen_Draw(); //Show the Login Screen

		model_mutex.lock();
		chat_building.is_running = false;
		model_mutex.unlock();
		endwin(); // clean exit
	}
};
#endif
