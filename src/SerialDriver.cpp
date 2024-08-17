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

#include "SerialDriver.hpp"
#include "Messages.hpp"

using namespace pc;

using namespace std;

SerialDriver::SerialDriver(BLooper* callback) : m_cb(callback), m_connected(false)
{

}

SerialDriver::~SerialDriver()
{

}

vector<string> SerialDriver::GetDevices()
{
	vector<string> tmp;
	
	return tmp;
}

void SerialDriver::MessageReceived(BMessage* message)
{
	switch(message->what) {
		case Message::LoadFile:
			//ToDo: parse file
			m_cb->PostMessage(Message::FileLoaded);
		break;

		case Message::Home:
			Exec("G28");
		break;

	}
}

void SerialDriver::Connect()
{

}

void SerialDriver::Disconnect()
{

}

void SerialDriver::LoadFile(string filename)
{
	BMessage* message = new BMessage(Message::LoadFile);
	message->AddString("filename",filename.c_str());

	PostMessage(message);
}

void SerialDriver::Exec(string line)
{

}

void SerialDriver::Home()
{
	PostMessage(Message::Home);
}
