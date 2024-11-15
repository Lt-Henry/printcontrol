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

#include "Settings.hpp"
#include "Messages.hpp"

#include <Path.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <SerialPort.h>

#include <map>
#include <string>

using namespace pc;

using namespace std;

map<string,int32> values = {
	{"baudrate.9600", B_9600_BPS},
	{"baudrate.19200", B_19200_BPS},
	{"baudrate.31250", B_31250_BPS},
	{"baudrate.38400", B_38400_BPS},
	{"baudrate.57600", B_57600_BPS},
	{"baudrate.115200", B_115200_BPS},
	{"baudrate.230400", B_230400_BPS},
	{"parity.None", B_NO_PARITY},
	{"parity.Even", B_EVEN_PARITY},
	{"parity.Odd", B_ODD_PARITY}
};

void Settings::Save(BMessage* settings)
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

BMessage* Settings::Load()
{
	BMessage* settings = nullptr;

	BPath path;
	path.Append("PrintControl");
	
	if (find_directory (B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {

	}
	else {
	
		settings = new BMessage(Message::Settings);
		settings->AddInt64("baudrate",9600);
		settings->AddInt32("parity",0);
		settings->AddInt32("stop",0);
		settings->AddInt32("flow",0);
		settings->AddInt32("databits",8);
		settings->AddInt32("lineend",0);
	}
	return settings;
}

string Settings::Name(string section, int32 value)
{
	for (auto kv:values) {
		if (kv.first.starts_with(section) and kv.second == value) {
			size_t n = kv.first.find(".");
			string name = kv.first.substr(n);
			return name;
		}
	}
	
	return "";
}

int32 Settings::Value(string section, string name)
{
	return values[section+"."+name];
}
