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
#include <LayoutBuilder.h>
#include <ControlLook.h>
#include <Path.h>
#include <Entry.h>
#include <Directory.h>
#include <File.h>
#include <OS.h>

#include <iostream>
#include <sstream>
#include <iomanip>

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
	
	
	BMenu* submenuExtruder = new BMenu("Extruder");
	menuControl->AddItem(submenuExtruder);
	
	msg = new BMessage(Message::MenuExtrude);
	msg->AddInt8("mm", 10);
	submenuExtruder->AddItem(new BMenuItem("Extrude 10 mm", msg));
	
	msg = new BMessage(Message::MenuExtrude);
	msg->AddInt8("mm", 1);
	submenuExtruder->AddItem(new BMenuItem("Extrude 1 mm", msg));
	
	msg = new BMessage(Message::MenuRetract);
	msg->AddInt8("mm", 1);
	submenuExtruder->AddItem(new BMenuItem("Retract 1 mm", msg));
	menu->AddItem(menuControl);
	
	msg = new BMessage(Message::MenuRetract);
	msg->AddInt8("mm", 10);
	submenuExtruder->AddItem(new BMenuItem("Retract 10 mm", msg));

	BRect area = Bounds();
	area.top = menu->Bounds().bottom;
	
	clog<<area.left<<","<<area.top<<","<<area.right<<","<<area.bottom<<endl;
	
	BView* statusBar = new BView(BRect(0,area.bottom-24,area.right,area.bottom),"statusBar", B_FOLLOW_ALL, 0);
	statusText = new BStringView(statusBar->Bounds(),"statusText","Connection: None, Status: None", B_FOLLOW_ALL);
	AddChild(statusBar);
	statusBar->Looper()->AddHandler(this);
	statusBar->AddChild(statusText);
	
	BTabView* tabView = new BTabView(BRect(0,area.top, area.right, area.bottom),"tab");
	tabView->SetResizingMode(B_FOLLOW_ALL);

	area.bottom-=64;
	BView* consoleView = new BView(area,"consoleArea", B_FOLLOW_ALL,0);
	consoleView->SetResizingMode(B_FOLLOW_ALL);
	tabView->AddTab(consoleView);
	
	console = new BTextView("console");
	console->SetWordWrap(false);
	console->SetResizingMode(B_FOLLOW_ALL);
	BScrollView* scrollText = new BScrollView("scrollText", console, 0, true, true);
	//tabView->AddTab(scrollText);
	
	txtCmd = new BTextControl("","",new BMessage(Message::CommandSend));
	txtCmd->SetAlignment(B_ALIGN_LEFT, B_ALIGN_LEFT);
	
	//tabView->AddTab(cmdBox);
	
	btnCmd = new BButton("Send", new BMessage(Message::CommandSend));
	
	float padding = be_control_look->DefaultItemSpacing();
	BLayoutBuilder::Grid<>(consoleView, padding, padding)
		.SetInsets(padding, padding, padding, padding)
		.Add(scrollText, 0, 0, 4, 1)
		.Add(txtCmd, 0,2,3,1)
		.Add(btnCmd, 3,2,1,1);
		
	
	BView* infoView = new BView(area,"infoArea", B_FOLLOW_ALL,0);
	infoView->SetResizingMode(B_FOLLOW_ALL);
	tabView->AddTab(infoView);
	
	tabView->TabAt(0)->SetLabel("Console");
	tabView->TabAt(1)->SetLabel("Info");
	AddChild(tabView);
	
	messenger = BMessenger(nullptr,this);
	messageRunner = new BMessageRunner(messenger, new BMessage(Message::QueryInfo), 1000000);
	
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
				Echo(BString("Opening device ") << message->FindString("name") << "\n");
				driver->Connect(message->FindString("name"),settings);
			}
			else {
				driver->Disconnect();
			}
		break;
		
		case Message::CommandSend: {
			BString text = txtCmd->Text();
			text<<"\n";
			
			if (text.Length() > 0) {
				if (driver->IsConnected()) {
					driver->Exec(txtCmd->Text());
				}
				Echo(text);
				txtCmd->SetText("");
			}
		}
		break;
		
		case Message::Connected:
			
			Echo("Connected\n");
		break;
		
		case Message::Disconnected:
			
			Echo("Disconnected\n");
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
			Echo(BString("Number of lines: ") << driver->GCode().Lines() << "\n");
			Echo(BString("Height: ") << driver->GCode().Height() << "mm\n");
			Echo(BString("Filament estimation: ") << (int)driver->GCode().Filament() << "mm\n");
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
			int temperature = (enable == 0) ? 0 : 190;
			driver->Hotend(0,temperature);
		}
		break;
		
		case Message::MenuBed: {
			int8 enable = message->FindInt8("enable");
			//TODO
			int temperature = (enable == 0) ? 0 : 40;
			driver->Bed(temperature);
		}
		break;
		
		case Message::MenuExtrude:
			driver->Extrude(message->FindInt8("mm"));
		break;
		
		case Message::MenuRetract:
			driver->Retract(message->FindInt8("mm"));
		break;
		
		case Message::MenuRun:
			Echo("Start printing...\n");
			driver->PrintRun();
		break;
		
		case Message::MenuPause:
			Echo("Pause print\n");
			driver->PrintPause();
		break;
		
		case Message::MenuStop:
			Echo("Stop printing...\n");
			driver->PrintStop();
		break;
		
		case Message::MenuRestart:
			Echo("Restart printing...\n");
			driver->PrintRestart();
		break;
		
		case Message::QueryInfo:
			if (driver->IsConnected()) {
				UpdateStatus();
			}
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

void MainWindow::UpdateStatus()
{
	stringstream ss;
	
	ss<<"Connection:";
	
	if (driver->IsConnected()) {
		ss<<driver->DevicePath();
	}
	else {
		ss<<"None";
	}
	
	ss<<", Status:";
	
	PrintStatus status = driver->Status();
	
	switch (status) {
		case PrintStatus::Off:
			ss<<"Off";
		break;
		
		case PrintStatus::Running:
			ss<<"Running";
		break;
		
		case PrintStatus::Paused:
			ss<<"Paused";
		break;
		
		case PrintStatus::Ended:
			ss<<"Ended";
		break;
		default:
			ss<<"Unknown";
	}
	
	int count = driver->Lines();
	int current = driver->CurrentLine();
	
	if (count > 0) {
		ss<<" Progress:"<<current<<"/"<<count<<" "<<std::fixed<<std::setprecision(2)<<(100.0f * (float)current/count)<<"%";
	}
	statusText->SetText(ss.str().c_str());
	
}
