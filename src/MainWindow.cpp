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

#include <Application.h>
#include <TranslatorRoster.h>
#include <BitmapStream.h>
#include <Bitmap.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
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

	BMenuBar* menu = new BMenuBar(BRect(0, 0, Bounds().Width(), 22), "menubar");
	BMenu* menuFile = new BMenu("File");
	
	menu->AddItem(menuFile);
	
	menuFile->AddItem(new BMenuItem("Open",new BMessage(Message::MenuOpen)));
	menuFile->AddItem(new BMenuItem("Quit",new BMessage(Message::MenuQuit)));
	
	AddChild(menu);

	
	BMessage *openMsg = new BMessage(Message::OpenRequest);
	
	openPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL,
		B_FILE_NODE, false, openMsg, NULL, true, true);
	
	openPanel->SetTarget(this);
	
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
		
		case Message::MenuQuit:
			be_app->PostMessage(B_QUIT_REQUESTED);
		break;

		case Message::OpenRequest: {
				clog<<"open requested!"<<endl;
				
			}
		break;
		
		default:
		BWindow::MessageReceived(message);
	}
}