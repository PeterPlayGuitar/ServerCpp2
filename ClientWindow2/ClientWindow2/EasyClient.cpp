#include "EasyClient.h"

EasyClient::EasyClient()
{
	packageToSend = nullptr;
	packageToRecieve = nullptr;

	serverAnswerString = new char[sizeOfPackage];
}

PUBLIC Error EasyClient::connectToServer(string _serverAdress, unsigned int _port)
{
	closesocket(sock);
	WSACleanup();

	serverAdress = _serverAdress;
	port = _port;

	int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (wsResult != 0)
		return CANT_START_WSA;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return CANT_INIT_SOCKET;

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, serverAdress.c_str(), &hint.sin_addr);

	int result = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (result == SOCKET_ERROR)
		return CANT_CONNECT_TO_SERVER;

	say "Connected to the server\n";
	isConnectedToServer = true;
	packageToSend = new char[sizeOfPackage];
	packageToRecieve = new char[sizeOfPackage];
	return NICE;
}

FILL void EasyClient::fillImAliveRequest()
{
	ZeroMemory(packageToSend, sizeOfPackage);
	packageToSend[0] = IM_ALIVE;
}

FILL void EasyClient::fillRegisterRequest(string name, string password)
{
	ZeroMemory(packageToSend, sizeOfPackage);
	packageToSend[0] = REGISTER;

	char *bytes = packageToSend + 1;

	fillBytesWithString(bytes, name);
	bytes[name.size()] = SPLIT_SYMBOL;
	fillBytesWithString(bytes + name.size() + 1, password);
}

FILL void EasyClient::fillLogInRequest(string name, string password)
{
	//this is same with filling registration so...
	fillRegisterRequest(name, password);
	packageToSend[0] = LOG_IN;
}

FILL void EasyClient::fillHelloRequest()
{
	ZeroMemory(packageToSend, sizeOfPackage);
	packageToSend[0] = HELLO_SERVER;
}

FILL void EasyClient::fillAddFriendRequest(string friendName)
{
	ZeroMemory(packageToSend, sizeOfPackage);
	packageToSend[0] = ADD_FRIEND;
	fillBytesWithString(packageToSend + 1, friendName);
}

FILL void EasyClient::fillShowListOfFriendsRequest()
{
	ZeroMemory(packageToSend, sizeOfPackage);
	packageToSend[0] = GET_LIST_OF_FRIENDS;
}

FILL void EasyClient::fillGetMessagesRequest()
{
	ZeroMemory(packageToSend, sizeOfPackage);
	packageToSend[0] = UPDATE;
}

FILL void EasyClient::fillSendMessageRequest(string friendName, System::String ^ message)
{
	ZeroMemory(packageToSend, sizeOfPackage);
	packageToSend[0] = SEND_MESSAGE;
	int x = 1;
	fillBytesWithString(packageToSend + x, friendName);
	x += friendName.size();
	packageToSend[x] = DOUBLE_DOT;
	x += 1;

	for (int i = 0; i < message->Length; i++)
	{
		wchar_t unicodeLetter = System::Convert::ToChar(message->default[i]);
		packageToSend[x] = (char)(unicodeLetter >> 8);
		packageToSend[x + 1] = (char)unicodeLetter;
		x += 2;
	}
}

ServerAnswerZeroPlace EasyClient::sendRequestAndGetAnswer()
{
	int bytesSent = send(sock, packageToSend, sizeOfPackage, 0);

	if (bytesSent == -1)
		return SERVER_NO_RESULT;

	ZeroMemory(packageToRecieve, sizeOfPackage);
	int bytesRecieved = recv(sock, packageToRecieve, sizeOfPackage, MSG_WAITALL);

	if (bytesRecieved == -1)
		return SERVER_NO_RESULT;
	if (bytesRecieved == sizeOfPackage)
		return SERVER_OK;

	return SERVER_NOT_OK;
}

ServerAnswerFirstPlace EasyClient::unpackPackage()
{
	ZeroMemory(serverAnswerString, sizeOfPackage);

	ServerAnswerFirstPlace safpTmp = packageToRecieve[1];

	switch (packageToRecieve[0])
	{
	case SERVER_NO_RESULT:
		serverAnswerString = "Server has not given result";
		break;
	case SERVER_OK:
		break;
	case SERVER_NOT_OK:
		serverAnswerString = "Unsuccessed";
		break;
	case SERVER_STRING:
		for (int i = 0; i < sizeOfPackage - 2; i++)
			serverAnswerString[i] += packageToRecieve[i + 2];
		break;
	case SERVER_LIST:
	{
		//here you can optimise
		int x = 2;
		while (packageToRecieve[x] != 0)
		{
			if (packageToRecieve[x] == SPLIT_SYMBOL)
				serverAnswerString[x - 2] = SPLIT_SYMBOL;
			else
				serverAnswerString[x - 2] = packageToRecieve[x];
			++x;
		}
		break;
	}
	default:
		serverAnswerString = "Unknows server result\n";
		break;
	}
	return safpTmp;
}

PUBLIC SEND ServerAnswerFirstPlace EasyClient::registerUser(string name, string password)
{
	if (!isConnectedToServer)
		return SERVER_NO_RESULT;
	fillRegisterRequest(name, password);
	ServerAnswerZeroPlace sazp = sendRequestAndGetAnswer();
	ServerAnswerFirstPlace safp = SERVER_NOT_OK;
	if (sazp == SERVER_OK) safp = unpackPackage();
	else if (sazp == SERVER_NO_RESULT) shutDownConnectionToServer();
	return safp;
}

PUBLIC SEND ServerAnswerFirstPlace EasyClient::logIn(string name, string password)
{
	if (!isConnectedToServer)
		return SERVER_NO_RESULT;
	fillLogInRequest(name, password);
	ServerAnswerZeroPlace sazp = sendRequestAndGetAnswer();
	ServerAnswerFirstPlace safp = SERVER_NOT_OK;
	if (sazp == SERVER_OK) safp = unpackPackage();
	else if (sazp == SERVER_NO_RESULT) shutDownConnectionToServer();
	return safp;
}

PUBLIC SEND ServerAnswerFirstPlace EasyClient::sayHelloToTheServer()
{
	if (!isConnectedToServer)
		return SERVER_NO_RESULT;
	fillHelloRequest();
	ServerAnswerZeroPlace sazp = sendRequestAndGetAnswer();
	ServerAnswerFirstPlace safp = SERVER_NOT_OK;
	if (sazp == SERVER_OK) safp = unpackPackage();
	else if (sazp == SERVER_NO_RESULT) shutDownConnectionToServer();
	return safp;
}

PUBLIC SEND ServerAnswerFirstPlace EasyClient::addFriend(string friendName)
{
	if (!isConnectedToServer)
		return SERVER_NO_RESULT;
	fillAddFriendRequest(friendName);
	ServerAnswerZeroPlace sazp = sendRequestAndGetAnswer();
	ServerAnswerFirstPlace safp = SERVER_NOT_OK;
	if (sazp == SERVER_OK) safp = unpackPackage();
	else if (sazp == SERVER_NO_RESULT) shutDownConnectionToServer();
	return safp;
}

PUBLIC SEND ServerAnswerFirstPlace EasyClient::sendListOfFriendsRequest()
{
	if (!isConnectedToServer)
		return SERVER_NO_RESULT;
	fillShowListOfFriendsRequest();
	ServerAnswerZeroPlace sazp = sendRequestAndGetAnswer();
	ServerAnswerFirstPlace safp = SERVER_NOT_OK;
	if (sazp == SERVER_OK) safp = unpackPackage();
	else if (sazp == SERVER_NO_RESULT) shutDownConnectionToServer();
	return safp;
}

PUBLIC SEND ServerAnswerFirstPlace EasyClient::sendMessage(string friendName, System::String ^message)
{
	if (!isConnectedToServer)
		return SERVER_NO_RESULT;
	fillSendMessageRequest(friendName, message);
	ServerAnswerZeroPlace sazp = sendRequestAndGetAnswer();
	ServerAnswerFirstPlace safp = SERVER_NOT_OK;
	if (sazp == SERVER_OK) safp = unpackPackage();
	else if (sazp == SERVER_NO_RESULT) shutDownConnectionToServer();

	return safp;
}

PUBLIC SEND ServerAnswerFirstPlace EasyClient::update()
{
	if (!isConnectedToServer)
		return SERVER_NO_RESULT;
	fillGetMessagesRequest();
	ServerAnswerZeroPlace sazp = sendRequestAndGetAnswer();
	ServerAnswerFirstPlace safp = SERVER_NOT_OK;
	if (sazp == SERVER_OK) safp = unpackPackage();
	else if (sazp == SERVER_NO_RESULT) shutDownConnectionToServer();
	return safp;
}

PUBLIC void EasyClient::shutDownConnectionToServer()
{
	say "Shutting down\n";
	closesocket(sock);
	WSACleanup();

	delete[] packageToSend;
	delete[] packageToRecieve;

	packageToSend = nullptr;
	packageToRecieve = nullptr;

	isConnectedToServer = false;
}

EasyClient::~EasyClient()
{
	shutDownConnectionToServer();
	delete[] serverAnswerString;
}

HELPING_FUNCTION void EasyClient::fillBytesWithString(char * bytes, string str)
{
	for (size_t i = 0; i < str.size(); i++)
		bytes[i] = str[i];
}

GET char* EasyClient::getServerAnswerString()
{
	return serverAnswerString;
}

GET bool EasyClient::getIsConnectedToServer()
{
	return isConnectedToServer;
}
