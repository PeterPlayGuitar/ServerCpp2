#pragma once
#include "WS2tcpip.h"
#include "iostream"
#include "thread"
#include "debug.h"
#include "Account.h"
#include "tuple"

#define say std::cout <<
#define string std::string
#define thread std::thread
#define tie std::tie
#define tuple std::tuple
#define make_tuple std::make_tuple

#pragma comment(lib,"ws2_32.lib")

#define HELPING_FUNCTION
#define PROCESSING_REQUEST_FUNCTION

typedef char Error;
#define NICE 69
#define SERVER_OK 1
#define SERVER_NOT_OK 2
#define CANT_CONNECT_TO_SERVER 3
#define CANT_BIND 4
#define CANT_INIT_LISTENING 5
#define CANT_CREATE_ACCOUNT 6
#define CANT_START_WSA 7
#define CANT_INIT_SOCKET 8
#define CLIENT_ERROR 9
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
#define THAT_CLIENT_DOES_NOT_EXISTS 20
#define EMPTY_MESSAGE 21
//when server executes client's requests there comes success or not

typedef int ServerCommandToClient;
#define SERVER_OK 1
#define SERVER_NOT_OK 2
#define SERVER_STRING 3
#define SERVER_LIST 4

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

class CurrentConnection
{
public:
	SOCKET socket;
	Account* account;
	bool isConnected;

	HELPING_FUNCTION void init(SOCKET _socket, Account *_account, bool _isConnected);
};

class Server
{
	static const size_t maxNumberOfAccounts = 64;
	static const size_t maxNumberOFConnections = FD_SETSIZE;

	Error lastError;

	SOCKET listening;
	fd_set setOfSockets;
	Account accounts[maxNumberOfAccounts];
	CurrentConnection connections[maxNumberOFConnections];

	Error servingClient(CurrentConnection &client, Package packageToSend, Package packageToRecieve);
	PROCESSING_REQUEST_FUNCTION Error createNewAccount(char *incomingBytes);
	PROCESSING_REQUEST_FUNCTION Error logUserIn(char *incomingBytes, CurrentConnection &client);
	PROCESSING_REQUEST_FUNCTION Error addFriend(Package dataPackage, Account* account);
	PROCESSING_REQUEST_FUNCTION Error fillPackageWithListOfFriendsForClient(char *bytesOFPackageToFill, Account* account);
	PROCESSING_REQUEST_FUNCTION Error sendMessageToFriend(char *incommingBytes, Account *sender);
	PROCESSING_REQUEST_FUNCTION Error getMessages(char *bytesOFPackageToFill, Account* account);
	HELPING_FUNCTION void fillBytesOutString(string str, char *bytes); 
	HELPING_FUNCTION int getIdByUsernameOfAccounts(string username);
public:
	Server();
	~Server();

	void startServerLoop();

	Error getLastError();
};
