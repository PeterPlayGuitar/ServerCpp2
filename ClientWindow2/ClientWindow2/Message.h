#pragma once

namespace ClientWindow2
{
	using namespace System;

	ref class Message
	{
	public:
		Message()
		{
			sender = gcnew String("");
			data = gcnew String("");
		}
		~Message() {};

		String ^sender;
		String ^data;
	};
}