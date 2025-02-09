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

#ifndef PC_GCODE
#define PC_GCODE

#include <string>
#include <vector>

namespace pc
{
	class GCode
	{
		public:
		
		GCode();
		
		void LoadFile(const char* filename);
		
		int Lines() const
		{
			return m_lines.size();
		}
		
		std::string Line(int n) const
		{
			return m_lines[n];
		}
		
		float Height() const
		{
			return m_height;
		}
		
		float Filament() const
		{
			return m_filament;
		}
		
		protected:
		
		void Reset();
		
		float m_height;
		float m_filament;
		
		std::vector<std::string> m_lines;
	};
}

#endif
