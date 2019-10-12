#include "Account.h"

Account::Account()
{
	isEmpty = true;
}

Account::~Account()
{
	for (auto i : messages)
		delete i;
	messages.clear();
}

void Account::makeAccount(string _username, string _password)
{
	username = _username;
	password = _password;

	isEmpty = false;
}

void Account::addFriend(Account * friendAccount)
{
	friends.push_back(friendAccount);
}

bool Account::hasFriend(Account *friendAccount)
{
	for (Account *i : friends)
		if (i == friendAccount)
			return true;
	return false;
}

void Account::addMessage(Message *msg)
{
	messages.push_back(msg);
}

bool Account::getIsEmpty()
{
	return isEmpty;
}

string Account::getUsername()
{
	return username;
}

string Account::getPassword()
{
	return password;
}

vector<Account*> Account::getListOFFriends()
{
	return friends;
}

Account::BytesSet Account::getMessages()
{
	size_t x = 0;
	char fillingBytes[sizeOfPackage];
	ZeroMemory(fillingBytes, sizeOfPackage);
	for (auto msg : messages)
	{
		for (int i = 0; i < msg->sender->getUsername().size(); i++)
		{
			fillingBytes[x] = msg->sender->getUsername()[i];
			x += 1;
		}
		fillingBytes[x] = DOUBLE_DOT;
		x += 1;
		for (int i = 0; i < msg->length; i++)
		{
			fillingBytes[x] = msg->data[i];
			x += 1;
		}
		fillingBytes[x] = SPLIT_SYMBOL;
		x += 1;
		fillingBytes[x] = SPLIT_SYMBOL;
		x += 1;
	}

	BytesSet bytesSet = { x,new char[x] };

	for (int i = 0; i < x; i++)
		bytesSet.data[i] = fillingBytes[i];

	for (auto i : messages)
		delete i;
	messages.clear();

	return bytesSet;
}

Account::Message::Message(char _data[], size_t _length, Account *_sender)
{
	data = new char[_length];
	for (int i = 0; i < _length; i++)
		data[i] = _data[i];
	length = _length;
	sender = _sender;
}

Account::Message::~Message()
{
	delete[] data;
}
