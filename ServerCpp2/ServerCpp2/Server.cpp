#include "Server.h"

Server::Server()
{
	lastError = NICE;

	FD_ZERO(&setOfSockets);

	//my account
	accounts[0].makeAccount("peter", "creeper");
}

void Server::startServerLoop()
{
	WSADATA wsData;
	if (WSAStartup(WINSOCK_VERSION, &wsData) != 0)
	{
		lastError = CANT_START_WSA;
		return;
	}

	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == SOCKET_ERROR || listening == INVALID_SOCKET)
	{
		lastError = CANT_INIT_SOCKET;
		return;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	int iResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
	if (iResult == SOCKET_ERROR)
	{
		lastError = CANT_BIND;
		return;
	}

	if (listen(listening, SOMAXCONN) == SOCKET_ERROR)
	{
		lastError = CANT_INIT_LISTENING;
		return;
	}

	//getting ready to loop
	for (int i = 0; i < maxNumberOFConnections; i++)
		connections[i].isConnected = false;

	FD_ZERO(&setOfSockets);
	FD_SET(listening, &setOfSockets);

	Package packageToRecieve = new char[sizeOfPackage];
	Package packageToSend = new char[sizeOfPackage];

	//start looping
	while (true)
	{
		//we need copy because operations with fd_set always break the structure, that's why we always copy it before use since we need the original master structure
		fd_set copyOfSet = setOfSockets;

		// we look into all sockets listened and select ones which are currently sending data or something like that idk its its so btw but we get number of sockets we need to process thats it
		int sockCount = select(0, &copyOfSet, nullptr, nullptr, nullptr);

		//we start from 1 because 0 index is listening socket which is supposed to accept and we placed it into thread above
		for (int i = 0; i < sockCount; i++)
		{
			SOCKET sock = copyOfSet.fd_array[i];
			if (sock == listening) // get new client
			{
				SOCKET newClient = accept(listening, nullptr, nullptr);
				if (newClient == INVALID_SOCKET)
				{
					say "Server> Listening has invalid socket (which is normal if you shut down the server)\n";
					continue;
				}

				//this tmpForLoop is needed to check if it ran through the if branch at least once
				bool tmpForLoop = false;
				//this for loop is needed to set new connection
				for (int i = 0; i < maxNumberOFConnections; i++)
					if (connections[i].isConnected == false)
					{
						tmpForLoop = true;
						connections[i].init(newClient, nullptr, true);
						FD_SET(newClient, &setOfSockets);
						say "Server> Client " << newClient << " has been connected to the server\n";
						break;
					}
				if (!tmpForLoop)
					say "Server> Client who tried to connect has no space to be processed\n";
			}
			else // process needed client
			{
				//find the connection by socket
				int indexOfConnection = 0;
				for (; indexOfConnection < maxNumberOFConnections; indexOfConnection++)
					if (connections[indexOfConnection].socket == sock)
					{
						connections[indexOfConnection].isConnected = true;
						//it was "false" before
						//connections[indexOfConnection].isConnected = false;
						break;
					}
				//serving clients
				if (servingClient(connections[indexOfConnection], packageToSend, packageToRecieve) == CLIENT_ERROR)
				{
					//deleting client
					connections[indexOfConnection].isConnected = false;

					closesocket(sock);
					FD_CLR(sock, &setOfSockets);
				}
			}
		}
	}
}

Error Server::servingClient(CurrentConnection &client, Package packageToSend, Package packageToRecieve)
{
	// recieving package
	ZeroMemory(packageToRecieve, sizeOfPackage);
	int bytesRecieved = recv(client.socket, packageToRecieve, sizeOfPackage, MSG_WAITALL);

	//cehck if errors and quit the session if so
	if (bytesRecieved == SOCKET_ERROR)
	{
		say "Server> The user with socket " << client.socket << " has been disconnected, err: bytesRecieved == SOCKET_ERROR\n";
		return CLIENT_ERROR;
	}
	if (bytesRecieved == 0)
	{
		say "Server> The user with socket " << client.socket << " has been disconnected, err: bytesRecieved == 0\n";
		return CLIENT_ERROR;
	}

	//making package to send
	ZeroMemory(packageToSend, sizeOfPackage);
	switch (packageToRecieve[0])//check the command from client
	{
	case NO_SINGAL:
		say "*";
		break;
	case IM_ALIVE:
		packageToSend[0] = SERVER_OK;
		packageToSend[1] = NICE;
		break;
	case REGISTER:
		packageToSend[0] = SERVER_STRING;
		if (client.account == nullptr)
		{
			Error error = createNewAccount(packageToRecieve + 1);
			packageToSend[1] = error;
			if (error == NICE)
				fillBytesOutString("Done, my nigga", packageToSend + 2);
			if (error == CANT_CREATE_ACCOUNT)
				fillBytesOutString("Can't create an/the account", packageToSend + 2);
			if (error == THERE_IS_USER_WITH_THAT_USERNAME)
				fillBytesOutString("The user with that username already exists, lol", packageToSend + 2);
		}
		else
		{
			packageToSend[1] = YOU_ALREADY_HAVE_ACCOUNT;
			fillBytesOutString("You don't need new account if you are already logged in, we don't do it here :\\", packageToSend + 2);
		}
		break;
	case LOG_IN:
		packageToSend[0] = SERVER_STRING;
		if (client.account == nullptr)
		{
			Error error = logUserIn(packageToRecieve + 1, client);
			packageToSend[1] = error;
			if (error == NICE)
				fillBytesOutString("Welcome, " + client.account->getUsername(), packageToSend + 2);
			if (error == WRONG_PASSWORD_OR_USERNAME)
				fillBytesOutString("Wrong password or username", packageToSend + 2);
		}
		else
		{
			packageToSend[1] = YOU_ALREADY_LOG_IN;
			fillBytesOutString("You are already logged in", packageToSend + 2);
		}
		break;
	case HELLO_SERVER:
		packageToSend[0] = SERVER_STRING;
		packageToSend[1] = NICE;
		if (client.account == nullptr)
			fillBytesOutString("Hey, welcome to 69 chat!\nYou can create your account and then log in\nHave fun!", packageToSend + 2);
		else
			fillBytesOutString("Hey, it's 69 chat, oh yeah, check how your friends are doing! (if you got one lol)", packageToSend + 2);
		break;
	case ADD_FRIEND:
		packageToSend[0] = SERVER_STRING;
		if (client.account != nullptr)
		{
			Error error = addFriend(packageToRecieve + 1, client.account);
			packageToSend[1] = error;
			if (error == NICE)
				fillBytesOutString("The friend has been added, enjoy talking!", packageToSend + 2);
			if (error == CANT_FIND_FRIEND)
				fillBytesOutString("Can't find that friend", packageToSend + 2);
			if (error == YOU_ALREADY_HAVE_THAT_FRIEND)
				fillBytesOutString("You have already added that friend", packageToSend + 2);
			if (error == FRIEND_IS_YOU)
				fillBytesOutString("That friend is you, so you can't add him", packageToSend + 2);
		}
		else
		{
			packageToSend[1] = YOU_ARE_NOT_LOG_IN;
			fillBytesOutString("You first need account, then you can find friends", packageToSend + 2);
		}
		break;
	case GET_LIST_OF_FRIENDS:
		if (client.account == nullptr)
		{
			packageToSend[0] = SERVER_STRING;
			packageToSend[1] = YOU_ARE_NOT_LOG_IN;
			fillBytesOutString("You gotta log in", packageToSend + 2);
		}
		else
		{
			Error error = fillPackageWithListOfFriendsForClient(packageToSend + 2, client.account);
			packageToSend[1] = error;
			if (error == NICE)
				packageToSend[0] = SERVER_LIST;
			if (error == HAVE_NO_FRIENDS)
			{
				packageToSend[0] = SERVER_STRING;
				fillBytesOutString("Umm... you actually got no friends... lol", packageToSend + 2);
			}
		}
		break;
	case SEND_MESSAGE:
		if (client.account != nullptr)
		{
			Error error = sendMessageToFriend(packageToRecieve + 1, client.account);
			packageToSend[1] = error;
			if (error == NICE)
				packageToSend[0] = SERVER_OK;
			if (error == THAT_CLIENT_DOES_NOT_EXISTS)
			{
				packageToSend[0] = SERVER_STRING;
				fillBytesOutString("That account does not exist", packageToSend + 2);
			}
			if (error == EMPTY_MESSAGE)
			{
				packageToSend[0] = SERVER_STRING;
				fillBytesOutString("The message is empty", packageToSend + 2);
			}
		}
		else
		{
			packageToSend[0] = SERVER_STRING;
			packageToSend[1] = YOU_ARE_NOT_LOG_IN;
			fillBytesOutString("You gotta log in", packageToSend + 2);
		}
		break;
	case UPDATE:
		if (client.account != nullptr)
		{
			Error error = getMessages(packageToSend + 2, client.account);
			packageToSend[1] = error;
			if (error == NICE)
				packageToSend[0] = SERVER_STRING;
		}
		else
		{
			packageToSend[0] = SERVER_STRING;
			packageToSend[1] = YOU_ARE_NOT_LOG_IN;
			fillBytesOutString("You gotta log in", packageToSend + 2);
		}
		break;
	default:
		packageToSend[0] = SERVER_STRING;
		packageToSend[1] = UNKNOWN_COMMAND;
		fillBytesOutString("I honestly don't know this command, sorrey c:", packageToSend + 2);
		break;
	}

	send(client.socket, packageToSend, sizeOfPackage, 0);

	return NICE;
}

PROCESSING_REQUEST_FUNCTION Error Server::createNewAccount(char *incomingBytes)
{
	for (int i = 0; i < maxNumberOfAccounts; i++)
		if (accounts[i].getIsEmpty())
		{
			int x = 0;

			string username = "";
			for (; x < sizeOfPackage - sizeof(CommandToServer); x++)
			{
				if (incomingBytes[x] == SPLIT_SYMBOL || incomingBytes[x] == 0)
				{
					x++;
					break;
				}
				username += incomingBytes[x];
			}

			if (username.size() == 0) return CANT_CREATE_ACCOUNT;
			for (int j = 0; j < maxNumberOfAccounts; j++)
			{
				if (j == i) continue;
				if (accounts[j].getIsEmpty()) continue;
				if (accounts[j].getUsername() == username)
					return THERE_IS_USER_WITH_THAT_USERNAME;
			}

			string password = "";
			for (; x < sizeOfPackage - sizeof(CommandToServer); x++)
			{
				if (incomingBytes[x] == SPLIT_SYMBOL || incomingBytes[x] == '\0')
				{
					x++;
					break;
				}
				password += incomingBytes[x];
			}

			if (password.size() == 0) return CANT_CREATE_ACCOUNT;

			accounts[i].makeAccount(username, password);

			say "Registered: " << accounts[i].getUsername() << " " << accounts[i].getPassword() << '\n';

			return NICE;
		}

	return CANT_CREATE_ACCOUNT;
}

PROCESSING_REQUEST_FUNCTION Error Server::logUserIn(char *incomingBytes, CurrentConnection &client)
{
	int x = 0;

	string username = "";
	for (; x < sizeOfPackage - sizeof(CommandToServer); x++)
	{
		if (incomingBytes[x] == SPLIT_SYMBOL || incomingBytes[x] == 0)
		{
			x++;
			break;
		}
		username += incomingBytes[x];
	}

	string password = "";
	for (; x < sizeOfPackage - sizeof(CommandToServer); x++)
	{
		if (incomingBytes[x] == SPLIT_SYMBOL || incomingBytes[x] == '\0')
		{
			x++;
			break;
		}
		password += incomingBytes[x];
	}

	for (int i = 0; i < maxNumberOfAccounts; i++)
	{
		if (accounts[i].getIsEmpty()) continue;
		if (accounts[i].getUsername() == username)
		{
			if (accounts[i].getPassword() == password)
			{
				client.account = accounts + i;
				return NICE;
			}
			else
				return WRONG_PASSWORD_OR_USERNAME;
		}
	}

	return WRONG_PASSWORD_OR_USERNAME;
}

PROCESSING_REQUEST_FUNCTION Error Server::addFriend(Package dataPackage, Account* account)
{
	string friendName = dataPackage;
	int idOfFriend = getIdByUsernameOfAccounts(friendName);
	if (idOfFriend != -1)
	{
		if (account == &accounts[idOfFriend])
			return FRIEND_IS_YOU;
		if (account->hasFriend(&accounts[idOfFriend]))
			return YOU_ALREADY_HAVE_THAT_FRIEND;

		account->addFriend(&accounts[idOfFriend]);

		return NICE;
	}

	return CANT_FIND_FRIEND;
}

PROCESSING_REQUEST_FUNCTION Error Server::fillPackageWithListOfFriendsForClient(char *bytesOFPackageToFill, Account * account)
{
	auto listOfFriends = account->getListOFFriends();
	if (listOfFriends.size() == 0)
		return HAVE_NO_FRIENDS;

	int fillingPosition = 0;
	for (Account *acc : listOfFriends)
	{
		fillBytesOutString(acc->getUsername(), bytesOFPackageToFill + fillingPosition);
		fillingPosition += acc->getUsername().size();
		bytesOFPackageToFill[fillingPosition] = SPLIT_SYMBOL;
		fillingPosition += 1;
	}

	return NICE;
}

PROCESSING_REQUEST_FUNCTION Error Server::sendMessageToFriend(char *incomingBytes, Account *sender)
{
	int x = 0;
	string friendName = "";
	//decode friend name
	for (; x < sizeOfPackage - 1; x++)
	{
		if (incomingBytes[x] == DOUBLE_DOT || incomingBytes[x] == 0)
			break;
		friendName += incomingBytes[x];
	}

	//lets find account
	Account *friendAccount = nullptr;
	for (int i = 0; i < maxNumberOfAccounts; i++)
	{
		if (accounts[i].getIsEmpty())
			continue;
		if (friendName == accounts[i].getUsername())
			friendAccount = accounts + i;
	}

	if (friendAccount == nullptr)
		return THAT_CLIENT_DOES_NOT_EXISTS;

	++x;

	if (incomingBytes[x] == 0 && incomingBytes[x + 1] == 0)
		return EMPTY_MESSAGE;

	char messageBytes[sizeOfPackage];
	ZeroMemory(messageBytes, sizeOfPackage);
	//decode sent message
	int i = 0;
	for (; i < sizeOfPackage; i++)
	{
		if ((incomingBytes[x] == 0 && incomingBytes[x + 1] == 0) || x >= sizeOfPackage - 2)
			break;

		messageBytes[i] = incomingBytes[x];
		++x;
	}

	friendAccount->addMessage(new Account::Message(messageBytes, i, sender));

	return NICE;
}

PROCESSING_REQUEST_FUNCTION Error Server::getMessages(char * bytesOFPackageToFill, Account * account)
{
	auto messages = account->getMessages();

	for (int i = 0; i < messages.length; i++)
		bytesOFPackageToFill[i] = messages.data[i];

	return NICE;
}

HELPING_FUNCTION void Server::fillBytesOutString(string str, char *bytes)
{
	for (size_t i = 0; i < str.size(); i++)
		bytes[i] = str.at(i);

}

HELPING_FUNCTION int Server::getIdByUsernameOfAccounts(string username)
{
	for (int i = 0; i < maxNumberOfAccounts; i++)
		if (accounts[i].getUsername() == username)
			return i;
	return -1;
}

HELPING_FUNCTION void CurrentConnection::init(SOCKET _socket, Account * _account, bool _isConnected)
{
	socket = _socket;
	account = _account;
	isConnected = _isConnected;
}

Error Server::getLastError()
{
	return lastError;
}

Server::~Server()
{
	say "Server is shut down\n";

	for (size_t i = 0; i < setOfSockets.fd_count; i++)
	{
		closesocket(setOfSockets.fd_array[i]);
	}

	closesocket(listening);
	WSACleanup();
}
