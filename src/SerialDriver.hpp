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

#ifndef PC_SERIAL_DRIVER
#define PC_SERIAL_DRIVER

#include "GCode.hpp"

#include <Looper.h>

#include <string>
#include <vector>

namespace pc
{
	class SerialDriver : public BLooper
	{
		public:

		SerialDriver(BLooper* callback);
		virtual ~SerialDriver();

		static std::vector<std::string> GetDevices();

		void MessageReceived(BMessage* message) override;

		void Connect();
		void Disconnect();

		bool IsConnected()
		{
			return m_connected;
		}

		void LoadFile(std::string filename);

		void Exec(std::string line);
		void Home(uint8 axis);

		protected:

		BLooper* m_cb;
		bool m_connected;
		
		pc::GCode m_gcode;
	};

}
#endif
