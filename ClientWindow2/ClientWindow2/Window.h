#pragma once
#include "cmath"
#include "EasyClient.h"
#include "windows.h"
#include <cliext/list>
#include "FriendItemPanel.h"
#include "Message.h"
#include "FriendMessagesData.h"

typedef char MyWindowState;
#define NOT_CONNECTED 0
#define LOG_IN_UP_FORM 1
#define MAIN_WINDOW 2
#define SETTINGS_PAGE 3

#define FADING_TIME 250

namespace ClientWindow2 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace System::Collections::Generic;
	using namespace System::Media;

	//my code
	const string serverAdress = "192.168.0.21";
	const int port = 54000;

	string castClrStringToStdString(String^ str)
	{
		string stdString = "";

		for (int i = 0; i < str->Length; i++)
			stdString += (char)str->default[i];

		return stdString;
	}
	//end my code

/// <summary>
/// —водка дл€ Window
/// </summary>
	public ref class Window : public System::Windows::Forms::Form
	{
		//my code
	private:
		EasyClient *client;
		cliext::list<Panel^> pages;
		cliext::list<FriendItemPanel^> listOfFriendsWithPanles;
		cliext::list<Label^> listOfLabelsWithServerAnswer;
		String ^choosenFriendName = "";
		UnreadMessageData ^unreadMessageData;
		FriendItemPanel ^choosenFriendPanel = nullptr;
		SoundPlayer ^newMessageSound;
		SoundPlayer ^errorSound;
		SoundPlayer ^foundFriendSound;
		SoundPlayer ^loggedInSound;
		cliext::list<String^> insults;
		//for dragging and move
		bool dragging = false;
		int prevX;
		int prevY;
		//for fading server answer
		int fadingCounter = FADING_TIME;
		//end my code
	private: System::Windows::Forms::Label^  laodingText;
	private: System::Windows::Forms::Label^  errorMessageInLogInUpPage;
	private: System::Windows::Forms::Panel^  mainPage;




	private: System::Windows::Forms::Button^  sayHiButton;
	private: System::Windows::Forms::Button^  closeButton;
	private: System::Windows::Forms::Panel^  windowHeaderPanel;
	private: System::Windows::Forms::Button^  collapseButton;
	private: System::Windows::Forms::Panel^  findFriendPanel;
	private: System::Windows::Forms::Panel^  upPanel;
	private: System::Windows::Forms::Panel^  downPanel;
	private: System::Windows::Forms::Button^  closeFindFriendFormButton;
	private: System::Windows::Forms::Button^  findFriendButton;
	private: System::Windows::Forms::TextBox^  friendNameTextBox;
	private: System::Windows::Forms::Button^  addFriendButton;

	private: System::Windows::Forms::TextBox^  serverAdressTextBox;
	private: System::Windows::Forms::Panel^  friendsPanel;
	private: System::Windows::Forms::Panel^  friendItemPanleOriginal;
	private: System::Windows::Forms::Label^  friendNameInItemOriginal;
	private: System::Windows::Forms::RichTextBox^  chatBox;


	private: System::Windows::Forms::RichTextBox^  messagingTextBox;
	private: System::Windows::Forms::Label^  toWhoLabel;
	private: System::Windows::Forms::Panel^  chatPanel;

	private: System::Windows::Forms::Button^  refreshButton;

	private: System::Windows::Forms::Label^  infoLabel;
	private: System::Windows::Forms::Timer^  fadingServerAnswerTimer;
	private: System::Windows::Forms::Timer^  catchMessageTimer;
	private: System::Windows::Forms::Panel^  settingsPage;

	private: System::Windows::Forms::Button^  backButton;
	private: System::Windows::Forms::Button^  settingsButton;
	private: System::Windows::Forms::Button^  insultButton;
	private: System::Windows::Forms::Label^  loginLabel;
	private: System::Windows::Forms::Label^  passwordLabel;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  addInsultTextBox;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::CheckBox^  checkBox2;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::PictureBox^  myPicture;






	private: System::Windows::Forms::Timer^  loadingTimer;
	private:

		//my code
		void setWindowState(MyWindowState newWindowState)
		{
			int iterator = 0;

			for each(auto i in pages)
			{
				if (iterator == newWindowState)
				{
					i->BringToFront();
					break;
				}
				++iterator;
			}
		}
		void updateFriendsListInForm(bool shouldIFillAnswer, bool shouldWePlayErrorSound)
		{
			ServerAnswerFirstPlace error = client->sendListOfFriendsRequest();

			if (error == SERVER_NOT_OK)
			{
				if (client->getIsConnectedToServer())
					errorMessageInLogInUpPage->Text = "The server is not okay";
				else
					disconnectFromServer();
				return;
			}
			if (error != NICE)
			{
				if (shouldWePlayErrorSound)
				{
					try
					{
						errorSound->Play();
					}
					catch (...)
					{
					}
				}
				if (shouldIFillAnswer)
					fillServerAnswerLabel();
				return;
			}

			cliext::list<String^> friendNames;

			string friendsInfoString = client->getServerAnswerString();
			size_t x = 0;
			string strTmp = "";
			//getting list of friends
			do
			{
				if (friendsInfoString[x] == SPLIT_SYMBOL)
				{
					friendNames.push_back(gcnew String(strTmp.c_str()));
					strTmp = "";
				}
				else
					strTmp += friendsInfoString[x];
				++x;
			} while (x < friendsInfoString.size());

			//deleting
			for each(auto i in listOfFriendsWithPanles)
			{
				friendsPanel->Controls->Remove(i);
			}
			listOfFriendsWithPanles.clear();

			//adding
			int iterator = 0;
			for each(auto friendName in friendNames)
			{
				FriendItemPanel^ newPanel = gcnew FriendItemPanel();

				friendsPanel->Controls->Add(newPanel);
				Label^ newLabel = gcnew Label();
				{
					newLabel->AutoSize = true;
					newLabel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)),
						static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
					newLabel->Font = (gcnew System::Drawing::Font(L"K2D ExtraLight", 11.25F));
					newLabel->ForeColor = System::Drawing::Color::White;
					newLabel->Location = System::Drawing::Point(10, 2);
					newLabel->Margin = System::Windows::Forms::Padding(10, 1, 1, 1);
					newLabel->Name = L"friendNameInItemOriginal";
					newLabel->Size = System::Drawing::Size(50, 29);
					newLabel->TabIndex = 16;
					newLabel->Text = friendName;
				}
				newPanel->Controls->Add(newLabel);
				newPanel->Location = Point(0, iterator*friendItemPanleOriginal->Height);
				newPanel->BackColor = friendItemPanleOriginal->BackColor;
				newPanel->Margin = friendItemPanleOriginal->Padding;
				newPanel->Name = L"friendItemPanleOriginal";
				newPanel->Size = friendItemPanleOriginal->Size;
				newPanel->TabIndex = friendItemPanleOriginal->TabIndex + iterator;
				newPanel->Click += gcnew System::EventHandler(this, &Window::panel1_Click);
				newPanel->MouseEnter += gcnew System::EventHandler(this, &Window::panel1_MouseEnter);
				newPanel->MouseLeave += gcnew System::EventHandler(this, &Window::panel1_MouseLeave);
				newPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Window::friendItemPanleOriginal_MouseDown);
				newPanel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Window::friendItemPanleOriginal_MouseUp);

				newPanel->name = friendName;

				listOfFriendsWithPanles.push_back(newPanel);

				++iterator;
			}
		}
		void fillServerAnswerLabel()
		{
			String^ strTmp = gcnew String(client->getServerAnswerString());
			int charsLeft = strTmp->Length;
			int sizeOfStringYouCanFit = 60;
			int iterator = 0;
			for each(auto i in listOfLabelsWithServerAnswer)
			{
				i->Show();
				i->Text = "";
				if (charsLeft >= sizeOfStringYouCanFit)
				{
					i->Text = strTmp->Substring(iterator*sizeOfStringYouCanFit, sizeOfStringYouCanFit);
					charsLeft -= sizeOfStringYouCanFit;
				}
				else if (charsLeft > 0)
				{
					i->Text = strTmp->Substring(iterator * sizeOfStringYouCanFit);
					charsLeft -= sizeOfStringYouCanFit;
				}
				++iterator;
			}
			fadingCounter = FADING_TIME;
			fadingServerAnswerTimer->Enabled = true;
		}
		void updateIncomingMessages()
		{
			cliext::list<Message^> ^messagesStrings = gcnew cliext::list<Message^>();

			if (client->update() == SERVER_NOT_OK)
				disconnectFromServer();

			char *messagesData = client->getServerAnswerString();

			int x = 0;
			while (messagesData[x] != 0 && messagesData[x + 1] != 0)
			{
				Message^ msg = gcnew Message();

				while (messagesData[x] != DOUBLE_DOT)
				{
					msg->sender += Char(messagesData[x]);
					x += 1;
				}
				x += 1;
				while (messagesData[x] != SPLIT_SYMBOL && messagesData[x + 1] != SPLIT_SYMBOL)
				{
					msg->data += Char((messagesData[x] << 8) | messagesData[x + 1]);
					x += 2;
				}
				messagesStrings->push_back(msg);
				x += 2;

				//sounding
				try
				{
					newMessageSound->Play();
				}
				catch (...) {}
			}

			unreadMessageData->fillMessageData(messagesStrings);
		};
		void displayCameMessagesOfExactPerson(String^ friendName, bool shouldDeletePastMessages)
		{
			updateIncomingMessages();
			auto unreadMessagesDataOFExactPerson = unreadMessageData->readExactPerson(friendName);
			if (shouldDeletePastMessages)
				chatBox->Text = "";
			for each(auto message in unreadMessagesDataOFExactPerson)
				addMessageToTextBox(friendName, message);
		}
		void addMessageToTextBox(String^ friendName, String^ message)
		{
			chatBox->AppendText(friendName + "> " + message + "\r\n");
			chatBox->ScrollToCaret();
		}
		void addMessageToTextBox(String ^message)
		{
			chatBox->AppendText("me> " + message + "\r\n");
			chatBox->ScrollToCaret();
		}
		void disconnectFromServer()
		{
			errorMessageInLogInUpPage->Text = "";
			catchMessageTimer->Enabled = false;
			setWindowState(NOT_CONNECTED);
		}
		void goToSettingsPage()
		{
			catchMessageTimer->Enabled = false;
			setWindowState(SETTINGS_PAGE);
		}

		//end my code
	public:
		Window(void)
		{
			//my code
			client = new EasyClient();
			//end my code

			InitializeComponent();

			//my code
			newMessageSound = gcnew SoundPlayer(L"data\\sms.wav");
			errorSound = gcnew  SoundPlayer(L"data\\error.wav");
			foundFriendSound = gcnew SoundPlayer(L"data\\addedFriend.wav");
			loggedInSound = gcnew SoundPlayer(L"data\\logedIn.wav");

			//insults
			{
				insults.push_back("Go fuck yourself");
				insults.push_back("You smell like ebola");
				insults.push_back("Nobody likes you");
				insults.push_back("Please stop being such an asshole");
				insults.push_back("FUck yourself");
				insults.push_back("Bitch");
				insults.push_back("I wish you die");
				insults.push_back("Die please");
				insults.push_back("I cant wait you die");
				insults.push_back("I hope you suffer");
				insults.push_back("get cancer");
				insults.push_back("Shut up");
				insults.push_back("Go talk to your whores");
				insults.push_back("I dont give a fuck");
				insults.push_back("I dont wanna talk to you");
				insults.push_back("I wish you shut up");
				insults.push_back("i think you suck");
				insults.push_back("when you talk i can feel shit smell");
				insults.push_back("pee pee poo poo");
				insults.push_back("no please dont say anything again");
				insults.push_back("shut up finaly");
				insults.push_back("where did you learn to be such an asshole");
				insults.push_back("Dieeeeee");
				insults.push_back("shut the fuck up");
				insults.push_back("SHUT THE FUCK UPPP DO U HEAR ME");
				insults.push_back("no dont speak anymore please");
				insults.push_back("you poo poo in my brain");
				insults.push_back("honestly i never liked you and nobody either");
				insults.push_back("you are adopted");
				insults.push_back("you have cancer");
				insults.push_back("can u shut the fuck up for a moment");
				insults.push_back("you are going to black list");
				insults.push_back("bitch listen fuck you");
				insults.push_back("fuck you");
				insults.push_back("fuck");
				insults.push_back("shit");
				insults.push_back("slut");
				insults.push_back("whore yeah you are");
				insults.push_back("kill yourself");
				insults.push_back("never talk to me again");
				insults.push_back("god never existed if he created something liek you");
				insults.push_back("poop");
				insults.push_back("idiot");
			}

			pages.push_back(notConnectedPage);
			pages.push_back(loggingPage);
			pages.push_back(mainPage);
			pages.push_back(settingsPage);

			for each(auto i in pages)
			{
				i->Location = Point(0, windowHeaderPanel->Height);
				i->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Window::mainPage_MouseUp);
				i->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Window::mainPage_MouseDown);
				i->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Window::mainPage_MouseMove);
			}

			this->Width = 900;
			this->Height = 600 + windowHeaderPanel->Height;

			mainPage->Controls->Add(findFriendPanel);
			findFriendPanel->Location = Point(Width / 2 - findFriendPanel->Width / 2, Height / 2 - findFriendPanel->Height + downPanel->Height / 2);
			findFriendPanel->BringToFront();
			findFriendPanel->Hide();
			friendNameTextBox->Location = Point(findFriendPanel->Width / 2 - friendNameTextBox->Width / 2, friendNameTextBox->Location.Y);
			addFriendButton->Location = Point(findFriendPanel->Width / 2 - addFriendButton->Width / 2, addFriendButton->Location.Y);

			windowHeaderPanel->Location = Point(0, 0);

			//strighting elements
			passwordTextBox->Location = Point(this->Size.Width / 2 - passwordTextBox->Size.Width / 2, passwordTextBox->Location.Y);
			usernameTextBox->Location = Point(this->Size.Width / 2 - usernameTextBox->Size.Width / 2, usernameTextBox->Location.Y);
			loggingInButton->Location = Point(passwordTextBox->Location.X, loggingInButton->Location.Y);
			loggingUpButton->Location = Point(passwordTextBox->Location.X + passwordTextBox->Width - loggingUpButton->Width, loggingUpButton->Location.Y);
			label1->Location = Point(this->Width / 2 - label1->Width / 2, label1->Location.Y);
			tryAgainButton->Location = Point(this->Width / 2 - tryAgainButton->Width / 2, tryAgainButton->Location.Y);
			laodingText->Location = Point(this->Width / 2 - laodingText->Width / 2, laodingText->Location.Y);
			passwordLabel->Location = Point(passwordTextBox->Location.X, passwordTextBox->Location.Y - passwordLabel->Size.Height - 2);
			loginLabel->Location = Point(usernameTextBox->Location.X, usernameTextBox->Location.Y - loginLabel->Size.Height - 2);

			serverAdressTextBox->Text = gcnew String(serverAdress.c_str());

			friendItemPanleOriginal->Hide();

			//initing list of labels with server answers
			for (int i = 0; i < 3; i++)
			{
				Label^ newLabel = gcnew Label();

				//init label with answer
				mainPage->Controls->Add(newLabel);
				newLabel->AutoSize = true;
				newLabel->AutoSize = true;
				newLabel->AutoSize = true;
				newLabel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)),
					static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
				newLabel->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(204)));
				newLabel->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)),
					static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
				newLabel->Margin = System::Windows::Forms::Padding(8);
				newLabel->Name = L"serverAnswerOnMainPageLabel";
				newLabel->Size = System::Drawing::Size(908, 18);
				newLabel->Location = System::Drawing::Point(8, 8 + i * newLabel->Height);
				newLabel->TabIndex = 12;
				newLabel->Text = "";
				newLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;

				listOfLabelsWithServerAnswer.push_back(newLabel);
			}

			unreadMessageData = gcnew UnreadMessageData();

			// !!! !!! !!! !!!
			// !!! !!! !!! !!!
			// !!! !!! !!! !!!
			//UNBLOCK
			Error error;// = client->connectToServer(serverAdress, port);

			//DELETE IT AND UNBLOCK THINGS UPPER
			error = SERVER_NOT_OK;
			// !!! !!! !!! !!!
			// !!! !!! !!! !!!
			// !!! !!! !!! !!!

			if (error != NICE)
				setWindowState(NOT_CONNECTED);
			else
				setWindowState(LOG_IN_UP_FORM);
			//end my code
		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~Window()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TextBox^  passwordTextBox;
	private: System::Windows::Forms::TextBox^  usernameTextBox;
	private: System::Windows::Forms::Panel^  notConnectedPage;
	private: System::Windows::Forms::Panel^  loggingPage;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  tryAgainButton;

	private: System::Windows::Forms::Button^  loggingUpButton;
	private: System::Windows::Forms::Button^  loggingInButton;


	private: System::ComponentModel::IContainer^  components;


	private:
		/// <summary>
		/// ќб€зательна€ переменна€ конструктора.
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// “ребуемый метод дл€ поддержки конструктора Ч не измен€йте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Window::typeid));
			this->usernameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->notConnectedPage = (gcnew System::Windows::Forms::Panel());
			this->serverAdressTextBox = (gcnew System::Windows::Forms::TextBox());
			this->laodingText = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tryAgainButton = (gcnew System::Windows::Forms::Button());
			this->loggingPage = (gcnew System::Windows::Forms::Panel());
			this->passwordLabel = (gcnew System::Windows::Forms::Label());
			this->errorMessageInLogInUpPage = (gcnew System::Windows::Forms::Label());
			this->loggingUpButton = (gcnew System::Windows::Forms::Button());
			this->loginLabel = (gcnew System::Windows::Forms::Label());
			this->loggingInButton = (gcnew System::Windows::Forms::Button());
			this->passwordTextBox = (gcnew System::Windows::Forms::TextBox());
			this->loadingTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->mainPage = (gcnew System::Windows::Forms::Panel());
			this->insultButton = (gcnew System::Windows::Forms::Button());
			this->settingsButton = (gcnew System::Windows::Forms::Button());
			this->infoLabel = (gcnew System::Windows::Forms::Label());
			this->refreshButton = (gcnew System::Windows::Forms::Button());
			this->chatPanel = (gcnew System::Windows::Forms::Panel());
			this->toWhoLabel = (gcnew System::Windows::Forms::Label());
			this->messagingTextBox = (gcnew System::Windows::Forms::RichTextBox());
			this->chatBox = (gcnew System::Windows::Forms::RichTextBox());
			this->sayHiButton = (gcnew System::Windows::Forms::Button());
			this->friendsPanel = (gcnew System::Windows::Forms::Panel());
			this->friendItemPanleOriginal = (gcnew System::Windows::Forms::Panel());
			this->friendNameInItemOriginal = (gcnew System::Windows::Forms::Label());
			this->findFriendButton = (gcnew System::Windows::Forms::Button());
			this->findFriendPanel = (gcnew System::Windows::Forms::Panel());
			this->downPanel = (gcnew System::Windows::Forms::Panel());
			this->addFriendButton = (gcnew System::Windows::Forms::Button());
			this->friendNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->upPanel = (gcnew System::Windows::Forms::Panel());
			this->closeFindFriendFormButton = (gcnew System::Windows::Forms::Button());
			this->closeButton = (gcnew System::Windows::Forms::Button());
			this->windowHeaderPanel = (gcnew System::Windows::Forms::Panel());
			this->collapseButton = (gcnew System::Windows::Forms::Button());
			this->fadingServerAnswerTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->catchMessageTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->settingsPage = (gcnew System::Windows::Forms::Panel());
			this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->addInsultTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->backButton = (gcnew System::Windows::Forms::Button());
			this->myPicture = (gcnew System::Windows::Forms::PictureBox());
			this->notConnectedPage->SuspendLayout();
			this->loggingPage->SuspendLayout();
			this->mainPage->SuspendLayout();
			this->chatPanel->SuspendLayout();
			this->friendsPanel->SuspendLayout();
			this->friendItemPanleOriginal->SuspendLayout();
			this->findFriendPanel->SuspendLayout();
			this->downPanel->SuspendLayout();
			this->upPanel->SuspendLayout();
			this->windowHeaderPanel->SuspendLayout();
			this->settingsPage->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->myPicture))->BeginInit();
			this->SuspendLayout();
			// 
			// usernameTextBox
			// 
			this->usernameTextBox->BackColor = System::Drawing::Color::White;
			this->usernameTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->usernameTextBox->Font = (gcnew System::Drawing::Font(L"Courier New", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->usernameTextBox->ForeColor = System::Drawing::Color::Black;
			this->usernameTextBox->Location = System::Drawing::Point(244, 188);
			this->usernameTextBox->MaxLength = 20;
			this->usernameTextBox->Name = L"usernameTextBox";
			this->usernameTextBox->ShortcutsEnabled = false;
			this->usernameTextBox->Size = System::Drawing::Size(408, 31);
			this->usernameTextBox->TabIndex = 5;
			this->usernameTextBox->WordWrap = false;
			// 
			// notConnectedPage
			// 
			this->notConnectedPage->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"notConnectedPage.BackgroundImage")));
			this->notConnectedPage->Controls->Add(this->serverAdressTextBox);
			this->notConnectedPage->Controls->Add(this->laodingText);
			this->notConnectedPage->Controls->Add(this->label1);
			this->notConnectedPage->Controls->Add(this->tryAgainButton);
			this->notConnectedPage->Font = (gcnew System::Drawing::Font(L"K2D ExtraBold", 48, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->notConnectedPage->Location = System::Drawing::Point(0, 0);
			this->notConnectedPage->Margin = System::Windows::Forms::Padding(0);
			this->notConnectedPage->MaximumSize = System::Drawing::Size(900, 600);
			this->notConnectedPage->MinimumSize = System::Drawing::Size(900, 600);
			this->notConnectedPage->Name = L"notConnectedPage";
			this->notConnectedPage->Size = System::Drawing::Size(900, 600);
			this->notConnectedPage->TabIndex = 6;
			// 
			// serverAdressTextBox
			// 
			this->serverAdressTextBox->BackColor = System::Drawing::Color::White;
			this->serverAdressTextBox->Font = (gcnew System::Drawing::Font(L"Courier New", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->serverAdressTextBox->Location = System::Drawing::Point(701, 8);
			this->serverAdressTextBox->Margin = System::Windows::Forms::Padding(8);
			this->serverAdressTextBox->Name = L"serverAdressTextBox";
			this->serverAdressTextBox->Size = System::Drawing::Size(191, 24);
			this->serverAdressTextBox->TabIndex = 11;
			this->serverAdressTextBox->Text = L"192.168.0.21";
			// 
			// laodingText
			// 
			this->laodingText->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->laodingText->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->laodingText->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->laodingText->Location = System::Drawing::Point(338, 360);
			this->laodingText->Name = L"laodingText";
			this->laodingText->Size = System::Drawing::Size(208, 18);
			this->laodingText->TabIndex = 10;
			this->laodingText->Text = L"trying to connect...";
			this->laodingText->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->laodingText->Visible = false;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(158)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->label1->Font = (gcnew System::Drawing::Font(L"Courier New", 26.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->ForeColor = System::Drawing::Color::White;
			this->label1->Location = System::Drawing::Point(149, 240);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(584, 40);
			this->label1->TabIndex = 9;
			this->label1->Text = L"Can\'t connect to the server";
			// 
			// tryAgainButton
			// 
			this->tryAgainButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(30)));
			this->tryAgainButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->tryAgainButton->FlatAppearance->BorderSize = 2;
			this->tryAgainButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->tryAgainButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tryAgainButton->ForeColor = System::Drawing::Color::White;
			this->tryAgainButton->Location = System::Drawing::Point(367, 300);
			this->tryAgainButton->Margin = System::Windows::Forms::Padding(20);
			this->tryAgainButton->Name = L"tryAgainButton";
			this->tryAgainButton->Size = System::Drawing::Size(149, 40);
			this->tryAgainButton->TabIndex = 8;
			this->tryAgainButton->Text = L"Try again";
			this->tryAgainButton->UseVisualStyleBackColor = false;
			this->tryAgainButton->Click += gcnew System::EventHandler(this, &Window::tryAgainButton_Click);
			// 
			// loggingPage
			// 
			this->loggingPage->BackColor = System::Drawing::SystemColors::Control;
			this->loggingPage->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"loggingPage.BackgroundImage")));
			this->loggingPage->Controls->Add(this->passwordLabel);
			this->loggingPage->Controls->Add(this->errorMessageInLogInUpPage);
			this->loggingPage->Controls->Add(this->loggingUpButton);
			this->loggingPage->Controls->Add(this->loginLabel);
			this->loggingPage->Controls->Add(this->loggingInButton);
			this->loggingPage->Controls->Add(this->passwordTextBox);
			this->loggingPage->Controls->Add(this->usernameTextBox);
			this->loggingPage->Location = System::Drawing::Point(0, 0);
			this->loggingPage->Margin = System::Windows::Forms::Padding(0);
			this->loggingPage->MaximumSize = System::Drawing::Size(900, 600);
			this->loggingPage->MinimumSize = System::Drawing::Size(900, 600);
			this->loggingPage->Name = L"loggingPage";
			this->loggingPage->Size = System::Drawing::Size(900, 600);
			this->loggingPage->TabIndex = 7;
			// 
			// passwordLabel
			// 
			this->passwordLabel->AutoSize = true;
			this->passwordLabel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(48)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->passwordLabel->Font = (gcnew System::Drawing::Font(L"Courier New", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->passwordLabel->ForeColor = System::Drawing::Color::White;
			this->passwordLabel->Location = System::Drawing::Point(254, 225);
			this->passwordLabel->Margin = System::Windows::Forms::Padding(10, 1, 1, 1);
			this->passwordLabel->Name = L"passwordLabel";
			this->passwordLabel->Size = System::Drawing::Size(89, 17);
			this->passwordLabel->TabIndex = 18;
			this->passwordLabel->Text = L"password:";
			// 
			// errorMessageInLogInUpPage
			// 
			this->errorMessageInLogInUpPage->AutoSize = true;
			this->errorMessageInLogInUpPage->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->errorMessageInLogInUpPage->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->errorMessageInLogInUpPage->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->errorMessageInLogInUpPage->Location = System::Drawing::Point(241, 360);
			this->errorMessageInLogInUpPage->Name = L"errorMessageInLogInUpPage";
			this->errorMessageInLogInUpPage->Size = System::Drawing::Size(0, 18);
			this->errorMessageInLogInUpPage->TabIndex = 11;
			this->errorMessageInLogInUpPage->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->errorMessageInLogInUpPage->Visible = false;
			// 
			// loggingUpButton
			// 
			this->loggingUpButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(30)));
			this->loggingUpButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->loggingUpButton->FlatAppearance->BorderSize = 2;
			this->loggingUpButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->loggingUpButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->loggingUpButton->ForeColor = System::Drawing::Color::White;
			this->loggingUpButton->Location = System::Drawing::Point(468, 300);
			this->loggingUpButton->Margin = System::Windows::Forms::Padding(20);
			this->loggingUpButton->Name = L"loggingUpButton";
			this->loggingUpButton->Size = System::Drawing::Size(184, 40);
			this->loggingUpButton->TabIndex = 11;
			this->loggingUpButton->Text = L"Log up";
			this->loggingUpButton->UseVisualStyleBackColor = false;
			this->loggingUpButton->Click += gcnew System::EventHandler(this, &Window::loggingUpButton_Click);
			// 
			// loginLabel
			// 
			this->loginLabel->AutoSize = true;
			this->loginLabel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(48)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->loginLabel->Font = (gcnew System::Drawing::Font(L"Courier New", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->loginLabel->ForeColor = System::Drawing::Color::White;
			this->loginLabel->Location = System::Drawing::Point(254, 167);
			this->loginLabel->Margin = System::Windows::Forms::Padding(10, 1, 1, 1);
			this->loginLabel->Name = L"loginLabel";
			this->loginLabel->Size = System::Drawing::Size(62, 17);
			this->loginLabel->TabIndex = 17;
			this->loginLabel->Text = L"login:";
			// 
			// loggingInButton
			// 
			this->loggingInButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(30)));
			this->loggingInButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->loggingInButton->FlatAppearance->BorderSize = 2;
			this->loggingInButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->loggingInButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->loggingInButton->ForeColor = System::Drawing::Color::White;
			this->loggingInButton->Location = System::Drawing::Point(244, 300);
			this->loggingInButton->Margin = System::Windows::Forms::Padding(20);
			this->loggingInButton->Name = L"loggingInButton";
			this->loggingInButton->Size = System::Drawing::Size(184, 40);
			this->loggingInButton->TabIndex = 10;
			this->loggingInButton->Text = L"Log in";
			this->loggingInButton->UseVisualStyleBackColor = false;
			this->loggingInButton->Click += gcnew System::EventHandler(this, &Window::loggingInButton_Click);
			// 
			// passwordTextBox
			// 
			this->passwordTextBox->BackColor = System::Drawing::Color::White;
			this->passwordTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->passwordTextBox->Font = (gcnew System::Drawing::Font(L"Courier New", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->passwordTextBox->ForeColor = System::Drawing::Color::Black;
			this->passwordTextBox->Location = System::Drawing::Point(244, 246);
			this->passwordTextBox->MaxLength = 20;
			this->passwordTextBox->Name = L"passwordTextBox";
			this->passwordTextBox->ShortcutsEnabled = false;
			this->passwordTextBox->Size = System::Drawing::Size(408, 31);
			this->passwordTextBox->TabIndex = 6;
			this->passwordTextBox->UseSystemPasswordChar = true;
			this->passwordTextBox->WordWrap = false;
			// 
			// loadingTimer
			// 
			this->loadingTimer->Tick += gcnew System::EventHandler(this, &Window::loadingTimer_Tick);
			// 
			// mainPage
			// 
			this->mainPage->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"mainPage.BackgroundImage")));
			this->mainPage->Controls->Add(this->insultButton);
			this->mainPage->Controls->Add(this->settingsButton);
			this->mainPage->Controls->Add(this->infoLabel);
			this->mainPage->Controls->Add(this->refreshButton);
			this->mainPage->Controls->Add(this->chatPanel);
			this->mainPage->Controls->Add(this->sayHiButton);
			this->mainPage->Controls->Add(this->friendsPanel);
			this->mainPage->Location = System::Drawing::Point(0, 0);
			this->mainPage->Margin = System::Windows::Forms::Padding(0);
			this->mainPage->MaximumSize = System::Drawing::Size(900, 600);
			this->mainPage->MinimumSize = System::Drawing::Size(900, 600);
			this->mainPage->Name = L"mainPage";
			this->mainPage->Size = System::Drawing::Size(900, 600);
			this->mainPage->TabIndex = 12;
			// 
			// insultButton
			// 
			this->insultButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(70)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->insultButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->insultButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->insultButton->FlatAppearance->BorderSize = 2;
			this->insultButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->insultButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 9.749999F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->insultButton->ForeColor = System::Drawing::Color::White;
			this->insultButton->Location = System::Drawing::Point(107, 562);
			this->insultButton->Margin = System::Windows::Forms::Padding(8);
			this->insultButton->Name = L"insultButton";
			this->insultButton->Size = System::Drawing::Size(30, 30);
			this->insultButton->TabIndex = 21;
			this->insultButton->Text = L":\\";
			this->insultButton->UseVisualStyleBackColor = false;
			this->insultButton->Click += gcnew System::EventHandler(this, &Window::button1_Click_2);
			// 
			// settingsButton
			// 
			this->settingsButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(50)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->settingsButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"settingsButton.BackgroundImage")));
			this->settingsButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->settingsButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->settingsButton->FlatAppearance->BorderSize = 2;
			this->settingsButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->settingsButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 9.749999F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->settingsButton->ForeColor = System::Drawing::Color::White;
			this->settingsButton->Location = System::Drawing::Point(74, 562);
			this->settingsButton->Margin = System::Windows::Forms::Padding(8);
			this->settingsButton->Name = L"settingsButton";
			this->settingsButton->Size = System::Drawing::Size(30, 30);
			this->settingsButton->TabIndex = 20;
			this->settingsButton->UseVisualStyleBackColor = false;
			this->settingsButton->Click += gcnew System::EventHandler(this, &Window::settingsButton_Click);
			this->settingsButton->MouseLeave += gcnew System::EventHandler(this, &Window::settingsButton_MouseLeave);
			this->settingsButton->MouseHover += gcnew System::EventHandler(this, &Window::settingsButton_MouseHover);
			// 
			// infoLabel
			// 
			this->infoLabel->AutoSize = true;
			this->infoLabel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->infoLabel->Font = (gcnew System::Drawing::Font(L"K2D ExtraLight", 8.999999F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->infoLabel->ForeColor = System::Drawing::Color::White;
			this->infoLabel->Location = System::Drawing::Point(209, 520);
			this->infoLabel->Margin = System::Windows::Forms::Padding(10, 1, 1, 1);
			this->infoLabel->Name = L"infoLabel";
			this->infoLabel->Size = System::Drawing::Size(0, 23);
			this->infoLabel->TabIndex = 17;
			this->infoLabel->Visible = false;
			// 
			// refreshButton
			// 
			this->refreshButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(50)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->refreshButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"refreshButton.BackgroundImage")));
			this->refreshButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->refreshButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->refreshButton->FlatAppearance->BorderSize = 2;
			this->refreshButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->refreshButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 9.749999F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->refreshButton->ForeColor = System::Drawing::Color::White;
			this->refreshButton->Location = System::Drawing::Point(8, 562);
			this->refreshButton->Margin = System::Windows::Forms::Padding(8);
			this->refreshButton->Name = L"refreshButton";
			this->refreshButton->Size = System::Drawing::Size(30, 30);
			this->refreshButton->TabIndex = 19;
			this->refreshButton->UseVisualStyleBackColor = false;
			this->refreshButton->Click += gcnew System::EventHandler(this, &Window::button1_Click_1);
			this->refreshButton->MouseLeave += gcnew System::EventHandler(this, &Window::button2_MouseLeave);
			this->refreshButton->MouseHover += gcnew System::EventHandler(this, &Window::button2_MouseHover);
			// 
			// chatPanel
			// 
			this->chatPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(30)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->chatPanel->Controls->Add(this->toWhoLabel);
			this->chatPanel->Controls->Add(this->messagingTextBox);
			this->chatPanel->Controls->Add(this->chatBox);
			this->chatPanel->Location = System::Drawing::Point(8, 114);
			this->chatPanel->Name = L"chatPanel";
			this->chatPanel->Size = System::Drawing::Size(684, 391);
			this->chatPanel->TabIndex = 17;
			this->chatPanel->Visible = false;
			// 
			// toWhoLabel
			// 
			this->toWhoLabel->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->toWhoLabel->AutoSize = true;
			this->toWhoLabel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->toWhoLabel->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->toWhoLabel->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->toWhoLabel->Location = System::Drawing::Point(11, 10);
			this->toWhoLabel->Margin = System::Windows::Forms::Padding(10, 1, 1, 1);
			this->toWhoLabel->Name = L"toWhoLabel";
			this->toWhoLabel->Size = System::Drawing::Size(0, 18);
			this->toWhoLabel->TabIndex = 17;
			// 
			// messagingTextBox
			// 
			this->messagingTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->messagingTextBox->Font = (gcnew System::Drawing::Font(L"K2D ExtraLight", 11.9F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->messagingTextBox->Location = System::Drawing::Point(13, 340);
			this->messagingTextBox->Margin = System::Windows::Forms::Padding(0);
			this->messagingTextBox->MaxLength = 80;
			this->messagingTextBox->Name = L"messagingTextBox";
			this->messagingTextBox->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::None;
			this->messagingTextBox->Size = System::Drawing::Size(659, 38);
			this->messagingTextBox->TabIndex = 18;
			this->messagingTextBox->Text = L"";
			this->messagingTextBox->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &Window::messagingTextBox_KeyUp);
			// 
			// chatBox
			// 
			this->chatBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->chatBox->Font = (gcnew System::Drawing::Font(L"Meiryo", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->chatBox->Location = System::Drawing::Point(13, 44);
			this->chatBox->Name = L"chatBox";
			this->chatBox->ReadOnly = true;
			this->chatBox->Size = System::Drawing::Size(659, 275);
			this->chatBox->TabIndex = 17;
			this->chatBox->TabStop = false;
			this->chatBox->Text = L"";
			this->chatBox->Enter += gcnew System::EventHandler(this, &Window::chatBox_Enter);
			// 
			// sayHiButton
			// 
			this->sayHiButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(70)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->sayHiButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->sayHiButton->FlatAppearance->BorderSize = 2;
			this->sayHiButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->sayHiButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 9.749999F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->sayHiButton->ForeColor = System::Drawing::Color::White;
			this->sayHiButton->Location = System::Drawing::Point(41, 562);
			this->sayHiButton->Margin = System::Windows::Forms::Padding(8);
			this->sayHiButton->Name = L"sayHiButton";
			this->sayHiButton->Size = System::Drawing::Size(30, 30);
			this->sayHiButton->TabIndex = 12;
			this->sayHiButton->Text = L"Hi";
			this->sayHiButton->UseVisualStyleBackColor = false;
			this->sayHiButton->Click += gcnew System::EventHandler(this, &Window::sayHiButton_Click);
			this->sayHiButton->MouseLeave += gcnew System::EventHandler(this, &Window::sayHiButton_MouseLeave);
			this->sayHiButton->MouseHover += gcnew System::EventHandler(this, &Window::sayHiButton_MouseHover);
			// 
			// friendsPanel
			// 
			this->friendsPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(30)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->friendsPanel->Controls->Add(this->friendItemPanleOriginal);
			this->friendsPanel->Controls->Add(this->findFriendButton);
			this->friendsPanel->Dock = System::Windows::Forms::DockStyle::Right;
			this->friendsPanel->Location = System::Drawing::Point(700, 0);
			this->friendsPanel->Name = L"friendsPanel";
			this->friendsPanel->Size = System::Drawing::Size(200, 600);
			this->friendsPanel->TabIndex = 16;
			// 
			// friendItemPanleOriginal
			// 
			this->friendItemPanleOriginal->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(30)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->friendItemPanleOriginal->Controls->Add(this->friendNameInItemOriginal);
			this->friendItemPanleOriginal->Font = (gcnew System::Drawing::Font(L"K2D ExtraLight", 11.25F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->friendItemPanleOriginal->Location = System::Drawing::Point(31, 277);
			this->friendItemPanleOriginal->Margin = System::Windows::Forms::Padding(0);
			this->friendItemPanleOriginal->Name = L"friendItemPanleOriginal";
			this->friendItemPanleOriginal->Size = System::Drawing::Size(200, 31);
			this->friendItemPanleOriginal->TabIndex = 15;
			this->friendItemPanleOriginal->Click += gcnew System::EventHandler(this, &Window::panel1_Click);
			this->friendItemPanleOriginal->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Window::friendItemPanleOriginal_MouseDown);
			this->friendItemPanleOriginal->MouseEnter += gcnew System::EventHandler(this, &Window::panel1_MouseEnter);
			this->friendItemPanleOriginal->MouseLeave += gcnew System::EventHandler(this, &Window::panel1_MouseLeave);
			this->friendItemPanleOriginal->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Window::friendItemPanleOriginal_MouseUp);
			// 
			// friendNameInItemOriginal
			// 
			this->friendNameInItemOriginal->AutoSize = true;
			this->friendNameInItemOriginal->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->friendNameInItemOriginal->Font = (gcnew System::Drawing::Font(L"K2D ExtraLight", 11.25F));
			this->friendNameInItemOriginal->ForeColor = System::Drawing::Color::White;
			this->friendNameInItemOriginal->Location = System::Drawing::Point(10, 2);
			this->friendNameInItemOriginal->Margin = System::Windows::Forms::Padding(10, 1, 1, 1);
			this->friendNameInItemOriginal->Name = L"friendNameInItemOriginal";
			this->friendNameInItemOriginal->Size = System::Drawing::Size(50, 29);
			this->friendNameInItemOriginal->TabIndex = 16;
			this->friendNameInItemOriginal->Text = L"petya";
			// 
			// findFriendButton
			// 
			this->findFriendButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(30)));
			this->findFriendButton->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->findFriendButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->findFriendButton->FlatAppearance->BorderSize = 2;
			this->findFriendButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->findFriendButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 9.749999F));
			this->findFriendButton->ForeColor = System::Drawing::Color::White;
			this->findFriendButton->Location = System::Drawing::Point(0, 570);
			this->findFriendButton->Margin = System::Windows::Forms::Padding(8);
			this->findFriendButton->Name = L"findFriendButton";
			this->findFriendButton->Size = System::Drawing::Size(200, 30);
			this->findFriendButton->TabIndex = 12;
			this->findFriendButton->Text = L"Find friend";
			this->findFriendButton->UseVisualStyleBackColor = false;
			this->findFriendButton->Click += gcnew System::EventHandler(this, &Window::button1_Click);
			// 
			// findFriendPanel
			// 
			this->findFriendPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->findFriendPanel->Controls->Add(this->downPanel);
			this->findFriendPanel->Controls->Add(this->upPanel);
			this->findFriendPanel->Location = System::Drawing::Point(979, 50);
			this->findFriendPanel->Name = L"findFriendPanel";
			this->findFriendPanel->Size = System::Drawing::Size(300, 230);
			this->findFriendPanel->TabIndex = 15;
			// 
			// downPanel
			// 
			this->downPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(70)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->downPanel->Controls->Add(this->addFriendButton);
			this->downPanel->Controls->Add(this->friendNameTextBox);
			this->downPanel->Location = System::Drawing::Point(0, 30);
			this->downPanel->Margin = System::Windows::Forms::Padding(0);
			this->downPanel->Name = L"downPanel";
			this->downPanel->Size = System::Drawing::Size(300, 200);
			this->downPanel->TabIndex = 15;
			// 
			// addFriendButton
			// 
			this->addFriendButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(30)));
			this->addFriendButton->FlatAppearance->BorderColor = System::Drawing::Color::Blue;
			this->addFriendButton->FlatAppearance->BorderSize = 2;
			this->addFriendButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->addFriendButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 9.749999F));
			this->addFriendButton->ForeColor = System::Drawing::Color::White;
			this->addFriendButton->Location = System::Drawing::Point(107, 122);
			this->addFriendButton->Margin = System::Windows::Forms::Padding(8);
			this->addFriendButton->Name = L"addFriendButton";
			this->addFriendButton->Size = System::Drawing::Size(97, 30);
			this->addFriendButton->TabIndex = 16;
			this->addFriendButton->Text = L"Add friend";
			this->addFriendButton->UseVisualStyleBackColor = false;
			this->addFriendButton->Click += gcnew System::EventHandler(this, &Window::addFriendButton_Click);
			// 
			// friendNameTextBox
			// 
			this->friendNameTextBox->BackColor = System::Drawing::Color::WhiteSmoke;
			this->friendNameTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->friendNameTextBox->Font = (gcnew System::Drawing::Font(L"Courier New", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->friendNameTextBox->ForeColor = System::Drawing::Color::Black;
			this->friendNameTextBox->Location = System::Drawing::Point(34, 57);
			this->friendNameTextBox->MaxLength = 20;
			this->friendNameTextBox->Name = L"friendNameTextBox";
			this->friendNameTextBox->ShortcutsEnabled = false;
			this->friendNameTextBox->Size = System::Drawing::Size(239, 22);
			this->friendNameTextBox->TabIndex = 12;
			this->friendNameTextBox->WordWrap = false;
			// 
			// upPanel
			// 
			this->upPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->upPanel->Controls->Add(this->closeFindFriendFormButton);
			this->upPanel->Location = System::Drawing::Point(0, 0);
			this->upPanel->Margin = System::Windows::Forms::Padding(0);
			this->upPanel->Name = L"upPanel";
			this->upPanel->Size = System::Drawing::Size(300, 30);
			this->upPanel->TabIndex = 16;
			// 
			// closeFindFriendFormButton
			// 
			this->closeFindFriendFormButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->closeFindFriendFormButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"closeFindFriendFormButton.BackgroundImage")));
			this->closeFindFriendFormButton->FlatAppearance->BorderSize = 0;
			this->closeFindFriendFormButton->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->closeFindFriendFormButton->FlatAppearance->MouseOverBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->closeFindFriendFormButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->closeFindFriendFormButton->Location = System::Drawing::Point(280, 10);
			this->closeFindFriendFormButton->Margin = System::Windows::Forms::Padding(0);
			this->closeFindFriendFormButton->Name = L"closeFindFriendFormButton";
			this->closeFindFriendFormButton->Size = System::Drawing::Size(20, 20);
			this->closeFindFriendFormButton->TabIndex = 15;
			this->closeFindFriendFormButton->TabStop = false;
			this->closeFindFriendFormButton->UseVisualStyleBackColor = false;
			this->closeFindFriendFormButton->Click += gcnew System::EventHandler(this, &Window::closeFindFriendFormButton_Click);
			this->closeFindFriendFormButton->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Window::closeFindFriendFormButton_MouseDown);
			this->closeFindFriendFormButton->MouseEnter += gcnew System::EventHandler(this, &Window::closeFindFriendFormButton_MouseEnter);
			this->closeFindFriendFormButton->MouseLeave += gcnew System::EventHandler(this, &Window::closeFindFriendFormButton_MouseLeave);
			// 
			// closeButton
			// 
			this->closeButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(180)), static_cast<System::Int32>(static_cast<System::Byte>(13)),
				static_cast<System::Int32>(static_cast<System::Byte>(13)));
			this->closeButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"closeButton.BackgroundImage")));
			this->closeButton->FlatAppearance->BorderSize = 0;
			this->closeButton->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(100)), static_cast<System::Int32>(static_cast<System::Byte>(100)));
			this->closeButton->FlatAppearance->MouseOverBackColor = System::Drawing::Color::Red;
			this->closeButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->closeButton->Location = System::Drawing::Point(880, 8);
			this->closeButton->Margin = System::Windows::Forms::Padding(0);
			this->closeButton->Name = L"closeButton";
			this->closeButton->Size = System::Drawing::Size(20, 20);
			this->closeButton->TabIndex = 13;
			this->closeButton->TabStop = false;
			this->closeButton->UseVisualStyleBackColor = false;
			this->closeButton->Click += gcnew System::EventHandler(this, &Window::closeButton_Click);
			// 
			// windowHeaderPanel
			// 
			this->windowHeaderPanel->Controls->Add(this->collapseButton);
			this->windowHeaderPanel->Controls->Add(this->closeButton);
			this->windowHeaderPanel->Location = System::Drawing::Point(0, 622);
			this->windowHeaderPanel->Name = L"windowHeaderPanel";
			this->windowHeaderPanel->Size = System::Drawing::Size(900, 28);
			this->windowHeaderPanel->TabIndex = 14;
			// 
			// collapseButton
			// 
			this->collapseButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(20)), static_cast<System::Int32>(static_cast<System::Byte>(110)),
				static_cast<System::Int32>(static_cast<System::Byte>(172)));
			this->collapseButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"collapseButton.BackgroundImage")));
			this->collapseButton->FlatAppearance->BorderSize = 0;
			this->collapseButton->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(140)),
				static_cast<System::Int32>(static_cast<System::Byte>(226)), static_cast<System::Int32>(static_cast<System::Byte>(243)));
			this->collapseButton->FlatAppearance->MouseOverBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(59)),
				static_cast<System::Int32>(static_cast<System::Byte>(159)), static_cast<System::Int32>(static_cast<System::Byte>(218)));
			this->collapseButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->collapseButton->Location = System::Drawing::Point(860, 8);
			this->collapseButton->Margin = System::Windows::Forms::Padding(0);
			this->collapseButton->Name = L"collapseButton";
			this->collapseButton->Size = System::Drawing::Size(20, 20);
			this->collapseButton->TabIndex = 14;
			this->collapseButton->TabStop = false;
			this->collapseButton->UseVisualStyleBackColor = false;
			this->collapseButton->Click += gcnew System::EventHandler(this, &Window::collapseButton_Click);
			// 
			// fadingServerAnswerTimer
			// 
			this->fadingServerAnswerTimer->Interval = 1;
			this->fadingServerAnswerTimer->Tick += gcnew System::EventHandler(this, &Window::fadingServerAnswerTimer_Tick);
			// 
			// catchMessageTimer
			// 
			this->catchMessageTimer->Interval = 1000;
			this->catchMessageTimer->Tick += gcnew System::EventHandler(this, &Window::catchMessageTimer_Tick);
			// 
			// settingsPage
			// 
			this->settingsPage->BackColor = System::Drawing::SystemColors::Control;
			this->settingsPage->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"settingsPage.BackgroundImage")));
			this->settingsPage->Controls->Add(this->checkBox2);
			this->settingsPage->Controls->Add(this->checkBox1);
			this->settingsPage->Controls->Add(this->addInsultTextBox);
			this->settingsPage->Controls->Add(this->label5);
			this->settingsPage->Controls->Add(this->label4);
			this->settingsPage->Controls->Add(this->label3);
			this->settingsPage->Controls->Add(this->label2);
			this->settingsPage->Controls->Add(this->backButton);
			this->settingsPage->Controls->Add(this->myPicture);
			this->settingsPage->Location = System::Drawing::Point(0, 0);
			this->settingsPage->Margin = System::Windows::Forms::Padding(0);
			this->settingsPage->MaximumSize = System::Drawing::Size(900, 600);
			this->settingsPage->MinimumSize = System::Drawing::Size(900, 600);
			this->settingsPage->Name = L"settingsPage";
			this->settingsPage->Size = System::Drawing::Size(900, 600);
			this->settingsPage->TabIndex = 12;
			// 
			// checkBox2
			// 
			this->checkBox2->AutoSize = true;
			this->checkBox2->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(100)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->checkBox2->FlatAppearance->BorderColor = System::Drawing::Color::Yellow;
			this->checkBox2->FlatAppearance->BorderSize = 4;
			this->checkBox2->FlatAppearance->CheckedBackColor = System::Drawing::Color::Yellow;
			this->checkBox2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->checkBox2->Font = (gcnew System::Drawing::Font(L"K2D", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->checkBox2->ForeColor = System::Drawing::Color::White;
			this->checkBox2->Location = System::Drawing::Point(11, 552);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(258, 33);
			this->checkBox2->TabIndex = 23;
			this->checkBox2->Text = L"have my picture on settings page\?";
			this->checkBox2->UseVisualStyleBackColor = false;
			this->checkBox2->CheckedChanged += gcnew System::EventHandler(this, &Window::checkBox2_CheckedChanged);
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(100)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->checkBox1->Checked = true;
			this->checkBox1->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox1->FlatAppearance->BorderColor = System::Drawing::Color::Yellow;
			this->checkBox1->FlatAppearance->BorderSize = 4;
			this->checkBox1->FlatAppearance->CheckedBackColor = System::Drawing::Color::Yellow;
			this->checkBox1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->checkBox1->Font = (gcnew System::Drawing::Font(L"K2D", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->checkBox1->ForeColor = System::Drawing::Color::White;
			this->checkBox1->Location = System::Drawing::Point(11, 510);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(170, 33);
			this->checkBox1->TabIndex = 22;
			this->checkBox1->Text = L"high quality graphics";
			this->checkBox1->UseVisualStyleBackColor = false;
			// 
			// addInsultTextBox
			// 
			this->addInsultTextBox->BackColor = System::Drawing::Color::Black;
			this->addInsultTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->addInsultTextBox->Font = (gcnew System::Drawing::Font(L"K2D ExtraLight", 11.25F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->addInsultTextBox->ForeColor = System::Drawing::Color::White;
			this->addInsultTextBox->Location = System::Drawing::Point(11, 448);
			this->addInsultTextBox->MaxLength = 75;
			this->addInsultTextBox->Multiline = true;
			this->addInsultTextBox->Name = L"addInsultTextBox";
			this->addInsultTextBox->Size = System::Drawing::Size(392, 37);
			this->addInsultTextBox->TabIndex = 20;
			this->addInsultTextBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Window::addInsultTextBox_KeyPress);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(100)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->label5->Font = (gcnew System::Drawing::Font(L"K2D", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->ForeColor = System::Drawing::Color::White;
			this->label5->Location = System::Drawing::Point(14, 416);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(91, 29);
			this->label5->TabIndex = 21;
			this->label5->Text = L"add insults:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(100)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->label4->Font = (gcnew System::Drawing::Font(L"K2D", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->ForeColor = System::Drawing::Color::White;
			this->label4->Location = System::Drawing::Point(11, 360);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(111, 46);
			this->label4->TabIndex = 19;
			this->label4->Text = L"Settings";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(100)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->label3->Font = (gcnew System::Drawing::Font(L"K2D", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->ForeColor = System::Drawing::Color::White;
			this->label3->Location = System::Drawing::Point(490, 12);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(85, 46);
			this->label3->TabIndex = 18;
			this->label3->Text = L"About";
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(100)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->label2->Font = (gcnew System::Drawing::Font(L"K2D", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::White;
			this->label2->Location = System::Drawing::Point(490, 55);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(402, 225);
			this->label2->TabIndex = 17;
			this->label2->Text = resources->GetString(L"label2.Text");
			// 
			// backButton
			// 
			this->backButton->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->backButton->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(10)), static_cast<System::Int32>(static_cast<System::Byte>(10)));
			this->backButton->FlatAppearance->BorderSize = 2;
			this->backButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->backButton->Font = (gcnew System::Drawing::Font(L"K2D Thin", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->backButton->ForeColor = System::Drawing::Color::White;
			this->backButton->Location = System::Drawing::Point(776, 552);
			this->backButton->Margin = System::Windows::Forms::Padding(20);
			this->backButton->Name = L"backButton";
			this->backButton->Size = System::Drawing::Size(116, 40);
			this->backButton->TabIndex = 16;
			this->backButton->Text = L"Back";
			this->backButton->UseVisualStyleBackColor = false;
			this->backButton->Click += gcnew System::EventHandler(this, &Window::backButton_Click);
			// 
			// myPicture
			// 
			this->myPicture->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->myPicture->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"myPicture.BackgroundImage")));
			this->myPicture->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->myPicture->Location = System::Drawing::Point(431, 288);
			this->myPicture->Name = L"myPicture";
			this->myPicture->Size = System::Drawing::Size(302, 312);
			this->myPicture->TabIndex = 24;
			this->myPicture->TabStop = false;
			this->myPicture->Visible = false;
			// 
			// Window
			// 
			this->AllowDrop = true;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->CausesValidation = false;
			this->ClientSize = System::Drawing::Size(1347, 780);
			this->Controls->Add(this->windowHeaderPanel);
			this->Controls->Add(this->findFriendPanel);
			this->Controls->Add(this->settingsPage);
			this->Controls->Add(this->loggingPage);
			this->Controls->Add(this->mainPage);
			this->Controls->Add(this->notConnectedPage);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MaximumSize = System::Drawing::Size(2000, 2000);
			this->MinimumSize = System::Drawing::Size(1, 1);
			this->Name = L"Window";
			this->Text = L"69 chat";
			this->TransparencyKey = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->notConnectedPage->ResumeLayout(false);
			this->notConnectedPage->PerformLayout();
			this->loggingPage->ResumeLayout(false);
			this->loggingPage->PerformLayout();
			this->mainPage->ResumeLayout(false);
			this->mainPage->PerformLayout();
			this->chatPanel->ResumeLayout(false);
			this->chatPanel->PerformLayout();
			this->friendsPanel->ResumeLayout(false);
			this->friendItemPanleOriginal->ResumeLayout(false);
			this->friendItemPanleOriginal->PerformLayout();
			this->findFriendPanel->ResumeLayout(false);
			this->downPanel->ResumeLayout(false);
			this->downPanel->PerformLayout();
			this->upPanel->ResumeLayout(false);
			this->windowHeaderPanel->ResumeLayout(false);
			this->settingsPage->ResumeLayout(false);
			this->settingsPage->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->myPicture))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		//log in BUTTON
	private: System::Void loggingInButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		string username = castClrStringToStdString(usernameTextBox->Text);
		string password = castClrStringToStdString(passwordTextBox->Text);

		ServerAnswerFirstPlace error = client->logIn(username, password);
		if (error == NICE)
		{
			try
			{
				loggedInSound->Play();
			}
			catch (...) {}

			errorMessageInLogInUpPage->Text = gcnew String(client->getServerAnswerString());

			catchMessageTimer->Enabled = true;
			setWindowState(MAIN_WINDOW);
			updateFriendsListInForm(false, false);
		}
		else if (error == SERVER_NOT_OK)
			if (client->getIsConnectedToServer())
				errorMessageInLogInUpPage->Text = "The server is not okay";
			else
				disconnectFromServer();
		else
		{
			try
			{
				errorSound->Play();
			}
			catch (...)
			{
			}
			errorMessageInLogInUpPage->Text = gcnew String(client->getServerAnswerString());
		}

		errorMessageInLogInUpPage->Location = Point(this->Width / 2 - errorMessageInLogInUpPage->Width / 2, errorMessageInLogInUpPage->Location.Y);
		errorMessageInLogInUpPage->Show();
		usernameTextBox->Text = "";
		passwordTextBox->Text = "";
	}
			 // log up BUTTON
	private: System::Void loggingUpButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		string username = castClrStringToStdString(usernameTextBox->Text);
		string password = castClrStringToStdString(passwordTextBox->Text);

		ServerAnswerFirstPlace error = client->registerUser(username, password);
		if (error == NICE)
		{
			errorMessageInLogInUpPage->Text = gcnew String(client->getServerAnswerString());

			client->registerUser(username, password);

			// also + logging in
			ServerAnswerFirstPlace error = client->logIn(username, password);
			if (error == NICE)
			{
				loggedInSound->Play();
				catchMessageTimer->Enabled = true;
				setWindowState(MAIN_WINDOW);
				updateFriendsListInForm(false, false);
			}
		}
		else if (error == SERVER_NOT_OK)
		{
			if (client->getIsConnectedToServer())
				errorMessageInLogInUpPage->Text = "The server is not okay";
			else
				disconnectFromServer();
		}
		else
		{
			try
			{
				errorSound->Play();
			}
			catch (...)
			{
			}
			errorMessageInLogInUpPage->Text = gcnew String(client->getServerAnswerString());
		}

		errorMessageInLogInUpPage->Location = Point(this->Width / 2 - errorMessageInLogInUpPage->Width / 2, errorMessageInLogInUpPage->Location.Y);
		errorMessageInLogInUpPage->Show();
		usernameTextBox->Text = "";
		passwordTextBox->Text = "";
	}
			 //try again BUTTON
	private: System::Void tryAgainButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		laodingText->Show();
		laodingText->Text = "trying to connect...";
		loadingTimer->Enabled = true;
	}
			 //connectin to server TIMER
	private: System::Void loadingTimer_Tick(System::Object^  sender, System::EventArgs^  e)
	{
		string serverAdressTmp = "";
		if (serverAdressTextBox->Text == "")
			serverAdressTmp = serverAdress;
		else
			serverAdressTmp = castClrStringToStdString(serverAdressTextBox->Text);

		if (client->connectToServer(serverAdressTmp, port) == NICE)
			setWindowState(LOG_IN_UP_FORM);
		else
			laodingText->Text = "can't connect yet";
		loadingTimer->Enabled = false;
	}
			 // moving window MOUSE
	private: System::Void mainPage_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		findFriendPanel->Hide();
		dragging = true;
	}
			 // moving window MOUSE
	private: System::Void mainPage_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {

		if (dragging)
			this->SetDesktopLocation(Location.X + e->X - prevX, Location.Y + e->Y - prevY);
		else
		{
			prevX = e->X;
			prevY = e->Y;
		}
	}
			 // moving window MOUSE
	private: System::Void mainPage_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		dragging = false;
	}
			 // свернуть BUTTON
	private: System::Void collapseButton_Click(System::Object^  sender, System::EventArgs^  e) {
		this->WindowState = FormWindowState::Minimized;
	}
			 // close the app BUTTON
	private: System::Void closeButton_Click(System::Object^  sender, System::EventArgs^  e) {
		this->Close();
	}
			 //hi BUTTON
	private: System::Void sayHiButton_Click(System::Object^  sender, System::EventArgs^  e) {
		if (client->sayHelloToTheServer() == NICE)
			fillServerAnswerLabel();
		else
			if (client->getIsConnectedToServer())
				errorMessageInLogInUpPage->Text = "The server is not okay";
			else
				disconnectFromServer();
	}
			 //close little window BUTTON
	private: System::Void closeFindFriendFormButton_Click(System::Object^  sender, System::EventArgs^  e) {
		findFriendPanel->Hide();
	}
			 //shine closing button MOUSE
	private: System::Void closeFindFriendFormButton_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		closeFindFriendFormButton->BackgroundImage = gcnew Bitmap("data\\closeDown.png");
	}
			 //shine closing button MOUSE
	private: System::Void closeFindFriendFormButton_MouseEnter(System::Object^  sender, System::EventArgs^  e) {
		closeFindFriendFormButton->BackgroundImage = gcnew Bitmap("data\\closeHover.png");
	}
			 //shine closing button MOUSE
	private: System::Void closeFindFriendFormButton_MouseLeave(System::Object^  sender, System::EventArgs^  e) {
		closeFindFriendFormButton->BackgroundImage = gcnew Bitmap("data\\svernut.png");
	}
			 //find friend BUTTON
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		chatPanel->Hide();
		choosenFriendPanel = nullptr;
		findFriendPanel->Show();
		friendNameTextBox->Focus();
	}
			 //add friend in mini window BUTTON
	private: System::Void addFriendButton_Click(System::Object^  sender, System::EventArgs^  e) {
		ServerAnswerFirstPlace error = client->addFriend(castClrStringToStdString(friendNameTextBox->Text));
		if (error == NICE)
		{
			try
			{
				foundFriendSound->Play();
			}
			catch (...)
			{
			}
			fillServerAnswerLabel();
			updateFriendsListInForm(true, true);
			closeFindFriendFormButton_Click(closeFindFriendFormButton, e);
		}
		else if (error == SERVER_NOT_OK)
		{
			if (client->getIsConnectedToServer())
				fillServerAnswerLabel();
			else
				disconnectFromServer();
		}
		else
		{
			try
			{
				errorSound->Play();
			}
			catch (...)
			{
			}
			fillServerAnswerLabel();
		}

		friendNameTextBox->Text = "";
	}
			 //friends list shining MOUSE
	private: System::Void panel1_MouseEnter(System::Object^  sender, System::EventArgs^  e) {
		Panel ^panel = (Panel^)sender;
		panel->BackColor = Color::FromArgb(20, 255, 255, 255);
	}
			 //friends list shining MOUSE
	private: System::Void panel1_MouseLeave(System::Object^  sender, System::EventArgs^  e) {
		Panel ^panel = (Panel^)sender;
		panel->BackColor = Color::FromArgb(30, 0, 0, 0);
	}
			 //friends list shining MOUSE
	private: System::Void friendItemPanleOriginal_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		Panel ^panel = (Panel^)sender;
		panel->BackColor = Color::FromArgb(40, 255, 255, 255);
	}
			 //friends list shining MOUSE
	private: System::Void friendItemPanleOriginal_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		Panel ^panel = (Panel^)sender;
		panel->BackColor = Color::FromArgb(20, 255, 255, 255);
	}
			 //friends list BUTTON
	private: System::Void panel1_Click(System::Object^  sender, System::EventArgs^  e) {
		chatPanel->Show();
		FriendItemPanel ^item = (FriendItemPanel^)sender;
		choosenFriendName = item->name;
		toWhoLabel->Text = "to " + item->name + ":";
		displayCameMessagesOfExactPerson(item->name, true);
		messagingTextBox->Focus();
		choosenFriendPanel = item;
	}
			 //messaging textBox KEYBOARD and **_SENDING_**
	private: System::Void messagingTextBox_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		if (e->KeyCode == Keys::Enter)
		{
			String ^message = messagingTextBox->Text->Substring(0, messagingTextBox->Text->Length - 1);
			ServerAnswerFirstPlace error = client->sendMessage(castClrStringToStdString(choosenFriendName), message);
			if (error == NICE)
			{
				addMessageToTextBox(message);
				fillServerAnswerLabel();
			}
			else if (error == SERVER_NOT_OK)
			{
				if (client->getIsConnectedToServer())
					fillServerAnswerLabel();
				else
					disconnectFromServer();
			}
			else
			{
				try
				{
					errorSound->Play();
				}
				catch (...)
				{
				}
				fillServerAnswerLabel();
			}

			messagingTextBox->Text = "";
		}
	}
			 //chatbox FOCUS
	private: System::Void chatBox_Enter(System::Object^  sender, System::EventArgs^  e) {
		messagingTextBox->Focus();
	}
			 //update BUTTON
	private: System::Void button1_Click_1(System::Object^  sender, System::EventArgs^  e) {
		updateIncomingMessages();
	}
			 //refresh button INFO
	private: System::Void button2_MouseHover(System::Object^  sender, System::EventArgs^  e) {
		infoLabel->Location = Point(refreshButton->Location.X, refreshButton->Location.Y - infoLabel->Size.Height - 8);
		infoLabel->Show();
		infoLabel->Text = "Refresh whether there are new messages";
	}
			 //refresh button INFO when mouse LEAVE
	private: System::Void button2_MouseLeave(System::Object^  sender, System::EventArgs^  e) {
		infoLabel->Hide();
	}
			 //hi button INFO
	private: System::Void sayHiButton_MouseHover(System::Object^  sender, System::EventArgs^  e) {
		infoLabel->Location = Point(sayHiButton->Location.X, sayHiButton->Location.Y - infoLabel->Size.Height - 8);
		infoLabel->Show();
		infoLabel->Text = "Send \"Hi\" signal to the server and see what it says";
	}
			 //hi button INFO when mouse LEAVE
	private: System::Void sayHiButton_MouseLeave(System::Object^  sender, System::EventArgs^  e) {
		infoLabel->Hide();
	}
			 //fade server answer TIMER
	private: System::Void fadingServerAnswerTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
		fadingCounter--;
		if (fadingCounter <= 0)
		{
			for each(auto i in listOfLabelsWithServerAnswer)
				i->Hide();
			fadingServerAnswerTimer->Enabled = false;
		}
	}
			 //catch message timer TIMER
	private: System::Void catchMessageTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
		updateIncomingMessages();
		if (choosenFriendPanel != nullptr)
			displayCameMessagesOfExactPerson(choosenFriendPanel->name, false);
	}
			 //back button BUTTON 
	private: System::Void backButton_Click(System::Object^  sender, System::EventArgs^  e) {
		catchMessageTimer->Enabled = true;
		setWindowState(MAIN_WINDOW);
	}
			 //settings button BUTTON
	private: System::Void settingsButton_Click(System::Object^  sender, System::EventArgs^  e) {
		goToSettingsPage();
	}
			 //settings button INFO
	private: System::Void settingsButton_MouseHover(System::Object^  sender, System::EventArgs^  e) {
		infoLabel->Location = Point(settingsButton->Location.X, settingsButton->Location.Y - infoLabel->Size.Height - 8);
		infoLabel->Show();
		infoLabel->Text = "Settings and About";
	}
			 //setting button LEAVE
	private: System::Void settingsButton_MouseLeave(System::Object^  sender, System::EventArgs^  e) {
		infoLabel->Hide();
	}
			 //insult BUTTON
	private: System::Void button1_Click_2(System::Object^  sender, System::EventArgs^  e) {
		if (choosenFriendName != "")
		{

			int _iter = rand() % insults.size();
			String ^message = "oh no";
			for each(auto i in insults)
			{
				if (_iter <= 0)
				{
					message = i;
					break;
				}
				_iter -= 1;
			}
			ServerAnswerFirstPlace error = client->sendMessage(castClrStringToStdString(choosenFriendName), message);
			if (error == NICE)
			{
				addMessageToTextBox(message);
				fillServerAnswerLabel();
			}
			else if (error == SERVER_NOT_OK)
			{
				if (client->getIsConnectedToServer())
					fillServerAnswerLabel();
				else
					disconnectFromServer();
			}
			else
			{
				try
				{
					errorSound->Play();
				}
				catch (...)
				{
				}
				fillServerAnswerLabel();
			}

			messagingTextBox->Text = "";
		}
	}
			 //add insult BUTTON
	private: System::Void addInsultTextBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
		if (e->KeyChar == Char(Keys::Enter))
		{
			if (addInsultTextBox->Text->Length > 0)
				insults.push_back(addInsultTextBox->Text);
			addInsultTextBox->Text = "";
		}
	}
			 //check box CHECK
	private: System::Void checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		if (checkBox2->Checked == true)
			myPicture->Show();
		else
			myPicture->Hide();
	}
	};
}

