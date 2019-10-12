#pragma once
#include "iostream"
#include "WS2tcpip.h"
#include "string"
#pragma comment(lib, "ws2_32.lib")

#define string std::string
#define say std::cout<<

#define yes true
#define no false

#define PUBLIC 
#define FILL
#define SEND
#define HELPING_FUNCTION
#define GET

typedef int Error;
#define NICE 69
#define CANT_START_WSA 1
#define CANT_INIT_SOCKET 2
#define CANT_CONNECT_TO_SERVER 3

//this comes on 0st place of package
typedef char ServerAnswerZeroPlace;
#define SERVER_NO_RESULT 0
#define SERVER_OK 1
#define SERVER_NOT_OK 2
#define SERVER_STRING 3
#define SERVER_LIST 4

//this comes on 1st places of package
typedef char ServerAnswerFirstPlace;
#define NICE 69
#define THERE_IS_USER_WITH_THAT_USERNAME 10
#define WRONG_PASSWORD_OR_USERNAME 11
#define CANT_FIND_FRIEND 12
#define YOU_ALREADY_HAVE_THAT_FRIEND 13
#define HAVE_NO_FRIENDS 14
#define FRIEND_IS_YOU 15
#define YOU_ALREADY_HAVE_ACCOUNT 16
#define YOU_ALREADY_LOG_IN 17
#define YOU_ARE_NOT_LOG_IN 18
#define UNKNOWN_COMMAND 19

typedef char CommandToServer;
#define NO_SINGAL 0
#define IM_ALIVE 1
#define REGISTER 2
#define LOG_IN 3
#define UPDATE 4 //to get message
#define HELLO_SERVER 5
#define ADD_FRIEND 6
#define GET_LIST_OF_FRIENDS 7
#define SEND_MESSAGE 8

typedef char* Package;

//special symbols in text protocol, for example split symbol
#define SPLIT_SYMBOL 1
#define DOUBLE_DOT 2

class EasyClient
{
	static const unsigned int durationOfPauseInServingLoop = 200;

	//server stuff
	string serverAdress;
	unsigned int port;
	WSAData wsaData;
	SOCKET sock;
	bool isConnectedToServer = false;
	//it is string with answer from server liek done my nigga or some stuff liek that
	char *serverAnswerString; // length is size of a package

	//data to send and to recieve
	Package packageToSend;
	Package packageToRecieve;

	//functions
	ServerAnswerZeroPlace sendRequestAndGetAnswer();
	ServerAnswerFirstPlace unpackPackage();

	//making packages for server
	FILL void fillImAliveRequest();
	FILL void fillRegisterRequest(string name, string password);
	FILL void fillLogInRequest(string name, string password);
	FILL void fillHelloRequest();
	FILL void fillAddFriendRequest(string friendName);
	FILL void fillShowListOfFriendsRequest();
	FILL void fillGetMessagesRequest();
	FILL void fillSendMessageRequest(string friendName, System::String ^ message);
	HELPING_FUNCTION void fillBytesWithString(char *bytes, string str);
public:
	static const size_t sizeOfPackage = 0x1000;

	EasyClient();
	~EasyClient();

	PUBLIC Error connectToServer(string _serverAdress, unsigned int _port);
	PUBLIC void shutDownConnectionToServer();

	PUBLIC SEND ServerAnswerFirstPlace registerUser(string name, string password);
	PUBLIC SEND ServerAnswerFirstPlace logIn(string name, string password);
	PUBLIC SEND ServerAnswerFirstPlace sayHelloToTheServer();
	PUBLIC SEND ServerAnswerFirstPlace addFriend(string friendName);
	PUBLIC SEND ServerAnswerFirstPlace sendListOfFriendsRequest();
	PUBLIC SEND ServerAnswerFirstPlace sendMessage(string friendName, System::String ^message);
	PUBLIC SEND ServerAnswerFirstPlace update();

	GET char* getServerAnswerString();
	GET bool getIsConnectedToServer();
};

