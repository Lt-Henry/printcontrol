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

#ifndef PC_MAIN_WINDOW
#define PC_MAIN_WINDOW

#include "SerialDriver.hpp"
#include "SettingsWindow.hpp"

#include <Window.h>
#include <GroupView.h>
#include <FilePanel.h>
#include <TextView.h>
#include <TextControl.h>
#include <Button.h>
#include <StringView.h>
#include <Messenger.h>
#include <MessageRunner.h>

#include <string>

namespace pc
{
	class MainWindow : public BWindow
	{
		public:
		
		MainWindow();
		~MainWindow();
		
		virtual bool QuitRequested() override;
		void MessageReceived(BMessage* message) override;
		
		void Echo(BString text);
		
		protected:
		
		void UpdateStatus();
		
		BMessenger messenger;
		BMessageRunner* messageRunner;
		
		BMessage* settings;
		BFilePanel* openPanel;
		
		// Console view
		BTextView* console;
		BTextControl* txtCmd;
		BButton* btnCmd;
		BStringView* statusText;
		
		// Info view
		
		pc::SerialDriver* driver;
		
		SettingsWindow* settingsWindow;
	};
}
#endif
