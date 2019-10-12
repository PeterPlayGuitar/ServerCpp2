#pragma once
#include "cliext\list"

#define HELPING

namespace ClientWindow2
{
	using namespace System;

	HELPING ref class FriendPlusItsMessages
	{
	public:
		FriendPlusItsMessages(String ^_name, array<String^> ^_array)
		{
			msgs = _array;
			name = _name;
		}

		array<String^> ^msgs;
		String ^name;
	};

	ref class FriendMessagesData
	{
		String ^friendName;
		cliext::list<String^> ^messages;
	public:
		FriendMessagesData(String^ friendName)
		{
			this->friendName = friendName;
			messages = gcnew cliext::list<String^>();
		}

		String^ getFriendName()
		{
			return friendName;
		}

		void addMessages(String^ msg)
		{
			messages->push_back(msg);
		}

		array<String^> ^getMessages()
		{
			array<String^> ^messagesArray = gcnew array<String^>(messages->size());
			int _iter = 0;
			for each(auto i in messages)
			{
				messagesArray[_iter] = i;
				_iter += 1;
			}
			messages->clear();
			return messagesArray;
		}
	};

	ref class UnreadMessageData
	{
		cliext::list<FriendMessagesData^> ^unreadMessagesData;
	public:
		UnreadMessageData()
		{
			unreadMessagesData = gcnew cliext::list<FriendMessagesData^>();
		}

		void fillMessageData(cliext::list<Message^>^ msgs)
		{
			for each(auto i in msgs)
			{
				//checking if there is that friend already
				FriendMessagesData^ foundFriendPointer = nullptr;
				for each(FriendMessagesData^ j in unreadMessagesData)
					if (i->sender == j->getFriendName())
					{
						foundFriendPointer = j;
						foundFriendPointer->addMessages(i->data);
						break;
					}

				if (foundFriendPointer == nullptr)
				{
					foundFriendPointer = gcnew FriendMessagesData(i->sender);
					foundFriendPointer->addMessages(i->data);
					unreadMessagesData->push_back(foundFriendPointer);
				}
			}
		}

		array<FriendPlusItsMessages^>^ read()
		{
			array<FriendPlusItsMessages^>^ returningArray = gcnew array<FriendPlusItsMessages^>(unreadMessagesData->size());
			int _iter = 0;
			for each(auto a in unreadMessagesData)
			{
				returningArray[_iter] = gcnew FriendPlusItsMessages(a->getFriendName(), a->getMessages());
				_iter += 1;
			}
			unreadMessagesData->clear();
			return returningArray;
		}

		array<String^>^ readExactPerson(String ^clientName)
		{
			for each(auto i in unreadMessagesData)
				if (i->getFriendName() == clientName)
				{
					auto tmpArray = i->getMessages();
					unreadMessagesData->remove(i);
					return tmpArray;
				}
			return gcnew array<String^>(0);
		}
	};
}
