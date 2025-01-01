/*
The MIT License (MIT)

Copyright (c) 2024 Enrique Medina Gremaldos <quique@necos.es>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "PrintControl.hpp"
#include "MainWindow.hpp"
#include "Messages.hpp"
#include "SerialDriver.hpp"
#include "Settings.hpp"

#include <Application.h>
#include <TranslatorRoster.h>
#include <BitmapStream.h>
#include <Bitmap.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <TabView.h>
#include <TextView.h>
#include <ScrollView.h>
#include <Path.h>
#include <Entry.h>
#include <Directory.h>
#include <File.h>
#include <OS.h>

#include <iostream>

using namespace pc;

using namespace std;

MainWindow::MainWindow()
: BWindow(BRect(100, 100, 100 + 720, 100 + 512), "Print Control", B_TITLED_WINDOW, 0)
{

	settings = Settings::Load();
	settingsWindow = nullptr;

	BMenuBar* menu = new BMenuBar(BRect(0, 0, Bounds().Width(), 22), "menubar");
	BMenu* menuFile = new BMenu("File");
	
	menu->AddItem(menuFile);
	
	menuFile->AddItem(new BMenuItem("Open",new BMessage(Message::MenuOpen)));
	menuFile->AddItem(new BMenuItem("Settings", new BMessage(Message::MenuSettings)));
	menuFile->AddItem(new BMenuItem("Quit",new BMessage(Message::MenuQuit)));
	
	AddChild(menu);
	
	BMessage *openMsg = new BMessage(Message::OpenRequest);
	
	openPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL,
		B_FILE_NODE, false, openMsg, NULL, true, true);
	
	openPanel->SetTarget(this);
	
	BMenu* menuDevice = new BMenu("Device");
	
	menu->AddItem(menuDevice);
	
	vector<string> devices = SerialDriver::GetDevices();
	for (string dev : devices) {
		BMessage* msgDev = new BMessage(Message::MenuDevice);
		msgDev->AddString("name",dev.c_str());
		menuDevice->AddItem(new BMenuItem(dev.c_str(),msgDev));
	}
	
	
	BMenu* menuProgram = new BMenu("Program");
	menuProgram->AddItem(new BMenuItem("Run", new BMessage(Message::MenuRun)));
	menuProgram->AddItem(new BMenuItem("Pause", new BMessage(Message::MenuPause)));
	menuProgram->AddItem(new BMenuItem("Stop", new BMessage(Message::MenuStop)));
	menuProgram->AddItem(new BMenuItem("Restart", new BMessage(Message::MenuRestart)));
	menu->AddItem(menuProgram);
	
	BMenu* menuControl = new BMenu("Control");
	BMenu* submenuHome = new BMenu("Home");
	menuControl->AddItem(submenuHome);
	BMessage* msg = new BMessage(Message::MenuHome);
	msg->AddInt8("axis",7);
	submenuHome->AddItem(new BMenuItem("All", msg));
	
	msg = new BMessage(Message::MenuHome);
	msg->AddInt8("axis",1);
	submenuHome->AddItem(new BMenuItem("X", msg));
	
	msg = new BMessage(Message::MenuHome);
	msg->AddInt8("axis",2);
	submenuHome->AddItem(new BMenuItem("Y", msg));
	
	msg = new BMessage(Message::MenuHome);
	msg->AddInt8("axis",4);
	submenuHome->AddItem(new BMenuItem("Z", msg));
	
	
	BMenu* submenuFan = new BMenu("Fan");
	menuControl->AddItem(submenuFan);
	
	msg = new BMessage(Message::MenuFan);
	msg->AddInt8("speed",0);
	submenuFan->AddItem(new BMenuItem("Off", msg));
	
	msg = new BMessage(Message::MenuFan);
	msg->AddInt8("speed", 64);
	submenuFan->AddItem(new BMenuItem("25%", msg));
	
	msg = new BMessage(Message::MenuFan);
	msg->AddInt8("speed", 128);
	submenuFan->AddItem(new BMenuItem("50%", msg));
	
	msg = new BMessage(Message::MenuFan);
	msg->AddInt8("speed", 192);
	submenuFan->AddItem(new BMenuItem("75%", msg));
	
	msg = new BMessage(Message::MenuFan);
	msg->AddInt8("speed", 255);
	submenuFan->AddItem(new BMenuItem("100%", msg));
	
	BMenu* submenuHotend = new BMenu("Hot-end");
	menuControl->AddItem(submenuHotend);
	
	msg = new BMessage(Message::MenuHotend);
	msg->AddInt8("enable", 0);
	submenuHotend->AddItem(new BMenuItem("Off", msg));
	
	msg = new BMessage(Message::MenuHotend);
	msg->AddInt8("enable", 1);
	submenuHotend->AddItem(new BMenuItem("Heat", msg));
	
	BMenu* submenuBed = new BMenu("Bed");
	menuControl->AddItem(submenuBed);
	
	msg = new BMessage(Message::MenuBed);
	msg->AddInt8("enable", 0);
	submenuBed->AddItem(new BMenuItem("Off", msg));
	
	msg = new BMessage(Message::MenuBed);
	msg->AddInt8("enable", 1);
	submenuBed->AddItem(new BMenuItem("Heat", msg));
	
	menu->AddItem(menuControl);
	

	BTabView* tabView = new BTabView(BRect(0,menu->Frame().bottom,Bounds().right,Bounds().bottom),"tab");
	tabView->SetResizingMode(B_FOLLOW_ALL);
	
	BRect textBounds = tabView->Bounds();
	textBounds.right-=20;
	textBounds.bottom-=42;
	
	console = new BTextView(textBounds,"console",textBounds,B_FOLLOW_ALL);
	console->SetWordWrap(false);
	BScrollView* scrollText = new BScrollView("scrollText",console, B_FOLLOW_ALL,0,true,true);
	tabView->AddTab(scrollText);
	tabView->TabAt(0)->SetLabel("Console");
	AddChild(tabView);
	
	driver = new SerialDriver(this);
	driver->Run();
	
	Echo("*** Welcome to PrintControl ***\n");
}

MainWindow::~MainWindow()
{
}

bool MainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void MainWindow::MessageReceived(BMessage* message)
{

	switch (message->what) {
	
		
		//open file
		case Message::MenuOpen:
			openPanel->Show();
		break;
		
		case Message::MenuSettings:
			if (!settingsWindow) {
				settingsWindow = new SettingsWindow(this,settings);
				settingsWindow->Show();
			}
		break;
		
		case Message::SettingsClose:
			settingsWindow = nullptr;
		break;
		
		case Message::Settings:
			delete settings;
			settings = message;
			Settings::Save(settings);
		break;
		
		case Message::MenuQuit:
			be_app->PostMessage(B_QUIT_REQUESTED);
		break;
		
		case Message::MenuDevice:
			if (!driver->IsConnected()) {
				clog<<"opening device "<<message->FindString("name")<<endl;
				driver->Connect(message->FindString("name"),settings);
			}
			else {
				driver->Disconnect();
			}
		break;
		
		case Message::Connected:
			
			Echo("*** Connected ***");
			//driver->Exec("M115");
		break;
		
		case Message::Disconnected:
			clog<<"Disconnected"<<endl;
		break;
		
		case Message::Echo: {
			BString text;
			
			message->FindString("text",&text);
			Echo(text);
		}
		break;
		
		case Message::OpenRequest: {
				clog<<"open requested!"<<endl;
				entry_ref ref;
				message->FindRef("refs", 0, &ref);
				BEntry entry(&ref, true);
				BPath path;
				entry.GetPath(&path);
				clog<<"path:"<<path.Path()<<endl;
				
				Echo("Loading file...\n");
				BMessage* msg = new BMessage(Message::LoadFile);
				msg->AddRef("ref",&ref);
				driver->PostMessage(msg);
			}
		break;
		
		case Message::FileLoaded: {
			clog<<"File has been loaded"<<endl;
			Echo("File loaded\n");
		}
		break;
		
		case Message::MenuHome:
			driver->Home(message->FindInt8("axis"));
		break;
		
		case Message::MenuFan:
			driver->Fan(0, message->FindInt8("speed"));
		break;
		
		case Message::MenuHotend: {
			int8 enable = message->FindInt8("enable");
			//TODO
			int temperature = (enable == 0) ? 0 : 150;
			driver->Hotend(1,temperature);
		}
		break;
		
		case Message::MenuBed: {
			int8 enable = message->FindInt8("enable");
			//TODO
			int temperature = (enable == 0) ? 0 : 40;
			driver->Bed(temperature);
		}
		break;
		
		case Message::MenuRun:
			clog<<"Start printing..."<<endl;
		break;
			
		default:
		BWindow::MessageReceived(message);
	}
}

void MainWindow::Echo(BString text)
{
	console->Insert(text.String());
	console->Invalidate();
}
