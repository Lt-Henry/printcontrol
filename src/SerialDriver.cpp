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

string prepare_line(int number,string line)
{
	string value;
	stringstream ss;
	bool valid=false;
	
	uint8 tmp = 0;
	if (line.size() > 0) {
		ss<<"N"<<number<<" ";
		
		for (size_t n=0;n<line.size();n++) {
			uint8 c = line[n];
			if (c == ';') {
				break;
			}
			valid=true;
			ss<<c;
			//tmp = tmp xor c;
		}
		
		line = ss.str();
		for (size_t n=0;n<line.size();n++) {
			uint8 c = line[n];
			tmp = tmp xor c;
		}
		ss<<"*"<<(int)tmp<<"\n";
	}
	
	if (!valid) {
		return "";
	}
	return ss.str();
}

SerialDriver::SerialDriver(BLooper* callback) : 
m_cb(callback), 
connected(false),
printStatus(PrintStatus::Off),
printLine(0)
{
	messenger = BMessenger(nullptr,this);
	messageQuery = new BMessage(Message::QueryInfo);
	messageRunner = new BMessageRunner(messenger, messageQuery, 1000000);
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
			clog<<"layers:"<<m_gcode.Layers()<<endl;
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
			
			tmp<<"M140 "<<"S"<<temperature;
			
			Exec(tmp.str());
		}
		break;
		
		case Message::Extrude: {
			stringstream tmp;
			int mm = message->FindInt32("mm");
			
			tmp<<"G0 "<<"E"<<mm;
			Exec("M83");
			Exec(tmp.str());
		}
		break;
		
		case Message::Retract: {
			stringstream tmp;
			int mm = message->FindInt32("mm");
			
			tmp<<"G0 "<<"E-"<<mm;
			
			Exec("M83");
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
				this->devicePath = path;
				fReaderThread = spawn_thread(_ReaderFunction, "readerThread", B_NORMAL_PRIORITY, (void*)this);
				resume_thread(fReaderThread);
			}
			else {
				cerr<<"Failed to open serial port:"<<status<<endl;
			}
		
		}
		break;
		
		case Message::ReadSerial:
			
		break;
		
		case Message::QueryInfo:
			if (connected) {
				
			}
		break;

		case Message::PrintStep: {
			
			if (printStatus != PrintStatus::Running) {
				break;
			}
			
			if (readLine >= m_gcode.Lines()) {
				printStatus = PrintStatus::Ended;
				break;
			}
			
			string code = m_gcode.Line(readLine);
			code = prepare_line(printLine+1,code);
			if (code.size() == 0) {
				readLine++;
				PostMessage(Message::PrintStep);
				break;
			}
			clog<<code;
			Send(code);
			PopOk();
			
			printLine++;
			readLine++;
			
			PostMessage(Message::PrintStep);
		}
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
	this->devicePath="";
	
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

void SerialDriver::Extrude(uint32 mm)
{
	BMessage* msg = new BMessage(Message::Extrude);
	msg->AddInt32("mm",mm);
	PostMessage(msg);
}

void SerialDriver::Retract(uint32 mm)
{
	BMessage* msg = new BMessage(Message::Retract);
	msg->AddInt32("mm",mm);
	PostMessage(msg);
}

void SerialDriver::PrintRun()
{
	if (printStatus == PrintStatus::Off) {
		printStatus = PrintStatus::Running;
		//Exec("M110 N1");
		printLine = 0;
		readLine = 0;
		PostMessage(Message::PrintStep);
	}
}

void SerialDriver::PrintPause()
{
	printStatus = PrintStatus::Paused;
}

void SerialDriver::PrintStop()
{
	printStatus = PrintStatus::Ended;
}

void SerialDriver::PrintRestart()
{
	printStatus = PrintStatus::Off;
	PrintRun();
}

void SerialDriver::Send(string line)
{
	string buffer;
	
	for (char c:line) {
		//strip comments from lines
		if (c == ';') {
			continue;
		}
		
		buffer = buffer + c;
	}
	
	size_t size = device.Write((const void *)buffer.c_str(),buffer.size());
	if (size<=0) {
		cerr<<"Output error:"<<size<<endl;
		return;
	}
	
}

void SerialDriver::PushOk()
{
	atomic_add(&okCount,1);
}

void SerialDriver::PopOk()
{
	int32 value = 0;
	
	while (true) {
		value = atomic_get(&okCount);
		
		if (value <= 0) {
			
			snooze(500); //500 us (0.5 ms)
		}
		else {
			atomic_add(&okCount,-1);
			break;
		}
	}
}

void SerialDriver::ResetOk()
{
	Lock();
	okCount = 0;
	Unlock();
}

void SerialDriver::PushEcho(string text)
{
	BMessage* msg = new BMessage(Message::Echo);
	msg->AddString("text",text.c_str());
	m_cb->PostMessage(msg);
}

uint32 _ProcessInput(SerialDriver* driver, string in)
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
		driver->PushEcho(token);
	}
	
	if (ok) {
		driver->PushOk();
	}
	
	return 0;
}

int32 _ReaderFunction(void* data)
{
	clog<<"Reader Thread"<<endl;
	
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
			clog<<"<<"<<line<<endl;
			_ProcessInput(driver, line);
			line.clear();
		}
	}
	
	return 0;
}
