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

#include "SettingsWindow.hpp"
#include "Messages.hpp"

#include <TextControl.h>
#include <Button.h>
#include <String.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <ControlLook.h>
#include <PopUpMenu.h>
#include <Path.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>

#include <iostream>
#include <vector>
#include <string>

using namespace pc;

using namespace std;

SettingsWindow::SettingsWindow(BWindow* parent)
: BWindow(BRect(100, 100, 100 + 512, 100 + 512), "Settings", B_FLOATING_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL,
	B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE, 0), fParent(parent)
{
	
	BPopUpMenu* popMenu = new BPopUpMenu("data");
	vector<string> baudrateValues = {"9600","19200","31250","38400","57600","115200","230400"};
	for (string value:baudrateValues) {
		popMenu->AddItem(new BMenuItem(value.c_str(),new BMessage(Message::SettingsChanged)));
	}
	popMenu->FindItem("9600")->SetMarked(true);
	BMenuField* fieldBaudrate = new BMenuField("baudrate","Baud rate", popMenu);
	
	popMenu = new BPopUpMenu("data");
	vector<string> parityValues = {"None","Even","Odd"};
	for (string value:parityValues) {
		popMenu->AddItem(new BMenuItem(value.c_str(),new BMessage(Message::SettingsChanged)));
	}
	popMenu->FindItem("None")->SetMarked(true);
	BMenuField* fieldParity = new BMenuField("parity","Parity", popMenu);
	
	popMenu = new BPopUpMenu("data");
	vector<string> stopValues = {"1","2"};
	for (string value:stopValues) {
		popMenu->AddItem(new BMenuItem(value.c_str(),new BMessage(Message::SettingsChanged)));
	}
	popMenu->FindItem("1")->SetMarked(true);
	BMenuField* fieldStop = new BMenuField("stop","Stop bits", popMenu);
	
	popMenu = new BPopUpMenu("data");
	vector<string> flowValues = {"Hardware","Software","Both","None"};
	for (string value:flowValues) {
		popMenu->AddItem(new BMenuItem(value.c_str(),new BMessage(Message::SettingsChanged)));
	}
	popMenu->FindItem("None")->SetMarked(true);
	BMenuField* fieldFlow = new BMenuField("flow","Flow control", popMenu);
	
	popMenu = new BPopUpMenu("data");
	vector<string> dataValues = {"7","8"};
	for (string value:dataValues) {
		popMenu->AddItem(new BMenuItem(value.c_str(),new BMessage(Message::SettingsChanged)));
	}
	popMenu->FindItem("8")->SetMarked(true);
	BMenuField* fieldDatabits = new BMenuField("databits","Data bits", popMenu);
	
	popMenu = new BPopUpMenu("data");
	vector<string> lineendValues = {"LF","CR","CR+LF"};
	for (string value:lineendValues) {
		popMenu->AddItem(new BMenuItem(value.c_str(),new BMessage(Message::SettingsChanged)));
	}
	popMenu->FindItem("LF")->SetMarked(true);
	BMenuField* fieldLineend = new BMenuField("lineend","Line end", popMenu);
	
	fBtnOk = new BButton("Ok", new BMessage(Message::SettingsClose));
	fBtnOk->SetEnabled(false);
	
	float padding = be_control_look->DefaultItemSpacing();
	BLayoutBuilder::Grid<>(this, padding, padding)
		.Add(fieldBaudrate, 1, 1)
		.Add(fieldParity, 1, 2)
		.Add(fieldStop, 1, 3)
		.Add(fieldFlow, 1, 4)
		.Add(fieldDatabits, 1, 5)
		.Add(fieldLineend, 1, 6)
		.Add(fBtnOk, 2, 10);
}

SettingsWindow::~SettingsWindow()
{
}

bool SettingsWindow::QuitRequested()
{
	fParent->PostMessage(Message::SettingsClose);
	Quit();
	return true;
}

void SettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case Message::SettingsClose: {
			clog<<"closing settings..."<<endl;
			BMessage* msg = new BMessage(Message::Settings);
			BMenuField* field = static_cast<BMenuField*>(FindView("baudrate"));
			msg->AddString("baudrate",field->MenuItem()->Label());
			
			field = static_cast<BMenuField*>(FindView("parity"));
			msg->AddString("parity",field->MenuItem()->Label());
			
			SettingsWindow::SaveSettings(msg);
			delete msg;
			Quit();
		}
		break;
		
		case Message::SettingsChanged:
			fBtnOk->SetEnabled(true);
		break;
	}
}

void SettingsWindow::SaveSettings(BMessage* settings)
{
	BPath path;
	if (find_directory (B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append("PrintControl");

		BFile file(path.Path(), B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
		if (file.InitCheck() != B_OK || file.Lock() != B_OK) {
			return;
			}
		
		settings->Flatten(&file);
		
		file.Sync();
		file.Unlock();
	}
}

void SettingsWindow::LoadSettings()
{
}
