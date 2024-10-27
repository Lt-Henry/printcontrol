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

using namespace pc;

using namespace std;

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
	BMessage* settings = new BMessage(Message::Settings);
	
	return settings;
}
