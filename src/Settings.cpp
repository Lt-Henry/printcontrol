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
#include <iostream>
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
	{"parity.Odd", B_ODD_PARITY},
	{"stop.1", B_STOP_BITS_1},
	{"stop.2", B_STOP_BITS_2},
	{"flow.Software", B_SOFTWARE_CONTROL},
	{"flow.Hardware", B_HARDWARE_CONTROL},
	{"flow.Both", B_SOFTWARE_CONTROL | B_HARDWARE_CONTROL},
	{"flow.None", 0},
	{"databits.7", B_DATA_BITS_7},
	{"databits.8", B_DATA_BITS_8}
};

void Settings::Save(BMessage* settings)
{
	BPath path;
	if (find_directory (B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append("PrintControl");

		BFile file(path.Path(), B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
		if (file.InitCheck() != B_OK || file.Lock() != B_OK) {
			cerr<<"Failed to open settings file"<<endl;
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
	find_directory(B_USER_SETTINGS_DIRECTORY,&path);
	path.Append("PrintControl");
	
	bool create_default = false;
	
	BFile file(path.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK or file.Lock() != B_OK) {
		cerr<<"Failed to open settings file"<<endl;
		create_default = true;
	}
	else {
		settings = new BMessage(Message::Settings);
		settings->Unflatten(&file);
		
		file.Sync();
		file.Unlock();
	}
	
	
	if (create_default) {
		settings = new BMessage(Message::Settings);
		settings->AddInt64("baudrate",Value("baudrate","9600"));
		settings->AddInt32("parity",Value("parity","None"));
		settings->AddInt32("stop",Value("stop","1"));
		settings->AddInt32("flow",Value("flow","None"));
		settings->AddInt32("databits",Value("databits","8"));
		
		Settings::Save(settings);
	}
	
	return settings;
}

string Settings::Name(string section, int32 value)
{
	for (auto kv:values) {
		if (kv.first.find(section) == 0 and kv.second == value) {
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

vector<string> Settings::Section(string section)
{
	vector<string> names;
	
	for (auto kv:values) {
		if (kv.first.find(section) == 0) {
			size_t n = kv.first.find(".");
			string name = kv.first.substr(n+1);
			names.push_back(name);
		}
	}
	return names;
}
