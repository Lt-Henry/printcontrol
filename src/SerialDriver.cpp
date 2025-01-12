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
#include "Settings.hpp"

#include <String.h>
#include <Path.h>
#include <Entry.h>
#include <SerialPort.h>

#include <iostream>
#include <sstream>

using namespace pc;

using namespace std;

int32 _ReaderFunction(void* data);

SerialDriver::SerialDriver(BLooper* callback) : m_cb(callback), connected(false)
{

}

SerialDriver::~SerialDriver()
{

}

vector<string> SerialDriver::GetDevices()
{
	vector<string> tmp;
	BSerialPort serial;
	char buffer[B_OS_NAME_LENGTH];
	
	for (int32 n=0;n<serial.CountDevices();n++) {
		serial.GetDeviceName(n,buffer);
		tmp.push_back(buffer);
	}
	
	return tmp;
}

void SerialDriver::MessageReceived(BMessage* message)
{
	switch(message->what) {
		case Message::LoadFile: {
			//ToDo: parse file
			entry_ref ref;
			message->FindRef("ref", 0, &ref);
			BEntry entry(&ref, true);
			BPath path;
			entry.GetPath(&path);
			clog<<"parsing "<<path.Path()<<endl;
			m_gcode.LoadFile(path.Path());
			clog<<"lines:"<<m_gcode.Lines()<<endl;
			clog<<"height:"<<m_gcode.Height()<<endl;
			clog<<"filament:"<<m_gcode.Filament()<<endl;
			m_cb->PostMessage(Message::FileLoaded);
			}
		break;

		case Message::Home: {
			string tmp = "G28";
			uint8 axis = message->FindInt8("axis");
			if ((axis & 1) == 1) {
				tmp+=" X";
			}
			if ((axis & 2) == 2) {
				tmp+=" Y";
			}
			
			if ((axis & 4) == 4) {
				tmp+=" Z";
			}
			Exec(tmp);
		}
		break;
		
		case Message::Fan: {
			stringstream tmp;
			int fan = message->FindInt8("fan");
			int speed = (uint8)message->FindInt8("speed");
			
			tmp<<"M106 "<<"P"<<fan<<" S"<<speed;
			
			Exec(tmp.str());
			
		}
		break;
		
		case Message::Hotend: {
			stringstream tmp;
			int hotend = message->FindInt8("hotend");
			int temperature = message->FindInt16("temperature");
			
			tmp<<"M104 "<<"T"<<hotend<<" S"<<temperature;
			
			Exec(tmp.str());
		}
		break;
		
		case Message::Bed: {
			stringstream tmp;
			int temperature = message->FindInt16("temperature");
			
			tmp<<"M190 "<<"S"<<temperature;
			
			Exec(tmp.str());
		}
		break;
		
		case Message::Connect: {
		
			BMessage* settings = new BMessage();
			message->FindMessage("settings",settings);
			
			BString path;
			message->FindString("path",&path);
			
			int32 value;
	
			//baud rate
			settings->FindInt32("baudrate",&value);
			clog<<"baudrate "<<value<<endl;
			device.SetDataRate((data_rate)value);
			
			//parity
			settings->FindInt32("parity",&value);
			clog<<"parity "<<value<<endl;
			device.SetParityMode((parity_mode)value);
			
			//stop
			settings->FindInt32("stop",&value);
			clog<<"stop "<<value<<endl;
			device.SetStopBits((stop_bits)value);
			
			//flow
			settings->FindInt32("flow",&value);
			clog<<"flow "<<value<<endl;	
			device.SetFlowControl(value);
			
			//databits
			settings->FindInt32("databits",&value);
			clog<<"databits "<<value<<endl;
			device.SetDataBits((data_bits)value);
			
			device.SetBlocking(false);
			device.SetTimeout(250000);
			
			status_t status = device.Open(path);
			if (status > 0) {
				connected = true;
				accepted = true;
				
				m_cb->PostMessage(Message::Connected);
				
				fReaderThread = spawn_thread(_ReaderFunction, "readerThread", B_NORMAL_PRIORITY, (void*)this);
				
			}
			else {
				cerr<<"Failed to open serial port:"<<status<<endl;
			}
		
		}
		break;
		
		case Message::ReadSerial:
			
		break;

	}
}

void SerialDriver::Connect(string path, BMessage* settings)
{
	if (connected) {
		return;
	}
	
	//settings->PrintToStream();
	BMessage* msg = new BMessage(Message::Connect);
	msg->AddString("path",path.c_str());
	msg->AddMessage("settings",settings);
	PostMessage(msg);
	
}

void SerialDriver::Disconnect()
{
	if (!connected) {
		return;
	}
	
	device.Close();
	
	connected = false;
	m_cb->PostMessage(Message::Disconnected);
}

void SerialDriver::LoadFile(string filename)
{
	BMessage* message = new BMessage(Message::LoadFile);
	message->AddString("filename",filename.c_str());

	PostMessage(message);
}

void SerialDriver::Exec(string line)
{
	clog<<"command:"<<line<<endl;
	Send(line + "\n");
	PopOk();
}

void SerialDriver::Home(uint8 axis)
{
	BMessage* msg = new BMessage(Message::Home);
	msg->AddInt8("axis",axis);
	PostMessage(msg);
}

void SerialDriver::Fan(uint8 fan, uint8 speed)
{
	BMessage* msg = new BMessage(Message::Fan);
	msg->AddInt8("fan",fan);
	msg->AddInt8("speed",speed);
	PostMessage(msg);
}

void SerialDriver::Hotend(uint8 hotend, uint16 temperature)
{
	BMessage* msg = new BMessage(Message::Hotend);
	msg->AddInt8("hotend",hotend);
	msg->AddInt16("temperature",temperature);
	PostMessage(msg);
}

void SerialDriver::Bed(uint16 temperature)
{
	BMessage* msg = new BMessage(Message::Bed);
	msg->AddInt16("temperature",temperature);
	PostMessage(msg);
}

uint32 SerialDriver::ProcessInput(string in)
{
	string token;
	string cmd;
	string value;
	
	bool echo = false;
	bool ok = false;
	
	for (char c:in) {
	
		if (echo) {
			token.push_back(c);
			continue;
		}
		
		switch(c) {
			case ' ':
			case '\n':
				if (token.size() == 0) {
					continue;
				}
				else {
					
					if (token == "ok") {
						ok = true;
						clog<<"ok!"<<endl;
					}
					
					if (cmd.size() > 0) {
						clog<<cmd<<"="<<token<<endl;
					}
					
					token.clear();
					cmd.clear();
				}
			break;
			
			case ':':
				cmd = token;
				token.clear();
				
				if (cmd == "echo") {
					echo = true;
				}
			break;
			
			default:
				token.push_back(c);
		}
	
	}
	
	if (echo) {
		clog<<token;
		
		BMessage* msg = new BMessage(Message::Echo);
		msg->AddString("text",token.c_str());
		m_cb->PostMessage(msg);
	}
	
	if (ok) {
		PushOk();
	}
	
	return 0;
}

void SerialDriver::Send(string line)
{
	size_t size = device.Write((const void *)line.c_str(),line.size());
	if (size<=0) {
		cerr<<"Output error:"<<size<<endl;
		return;
	}
	
}

void SerialDriver::PushOk()
{
	Lock();
		okCount++;
	Unlock();
}

void SerialDriver::PopOk()
{
	bool fetch = false;
	
	while (!fetch) {
		Lock();
		if (okCount > 0) {
			okCount--;
			fetch = true;
		}
		Unlock();
		
		if (!fetch) {
			snooze(100000);
		}
	}
}

void SerialDriver::ResetOk()
{
	Lock();
	okCount = 0;
	Unlock();
}

int32 _ReaderFunction(void* data)
{
	SerialDriver* driver = (SerialDriver *) data;
	BSerialPort* device = driver->Device();
	
	uint8 buffer;
	string line;
	size_t size;
	
	while (true) {
		size = device->Read((void *)&buffer,1);
		
		if (size<=0) {
			break;
		}
		
		line+=buffer;
		
		if (buffer == '\n') {
			driver->ProcessInput(line);
			line.clear();
		}
	}
	
	return 0;
}
