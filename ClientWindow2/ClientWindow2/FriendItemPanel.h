#pragma once
#include <cliext/list>

namespace ClientWindow2
{
	using namespace System;
	public ref class FriendItemPanel : public System::Windows::Forms::Panel
	{
	public:
		FriendItemPanel() : Panel() {}

		String ^name;
	};
}