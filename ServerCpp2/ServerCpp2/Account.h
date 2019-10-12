#pragma once
#include "string"
#include "vector"
#include "WS2tcpip.h"
#include "iostream"

#define string std::string
#define vector std::vector

static const size_t sizeOfPackage = 0x1000;

//special symbols in text protocol, for example split symbol
#define SPLIT_SYMBOL 1 //to split words
#define DOUBLE_DOT 2 //to make pair //example is friendName:message

class Account
{
public:
	//struct like a string
	struct BytesSet
	{
		size_t length;
		char *data;//dont forget to delete
	};
	class Message
	{
	public:
		size_t length;
		char *data;
		Account *sender;

		Message(char data[], size_t length, Account *sender);
		~Message();
	};
private:
	bool isEmpty;
	string username;
	string password;

	vector<Account*> friends;
	vector<Message*> messages;
public:
	Account();
	~Account();

	void makeAccount(string _username, string _password);
	void addFriend(Account *friendAccount);
	bool hasFriend(Account *friendAccount);
	void addMessage(Message *msg);

	bool getIsEmpty();
	string getUsername();
	string getPassword();
	vector<Account*> getListOFFriends();
	BytesSet getMessages();
};
