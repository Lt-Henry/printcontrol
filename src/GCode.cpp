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

#include "GCode.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>


using namespace pc;

using namespace std;

vector<string> Tokens(string& line)
{
	vector<string> tokens;
	bool comment = false;
	bool knee = false;
	string tmp;
	
	for (char c:line ) {
		if (c == ' ') {
			if(knee == true) {
				tokens.push_back(tmp);
				tmp.clear();
				knee = false;
			}
		}
		else {
			if (c == ';') {
				comment = true;
				break;
			}
			knee=true;
			tmp.push_back(c);
		}
	}
	
	if (knee) {
		tokens.push_back(tmp);
	}
	
	return tokens;
}

bool Value(string& token,char& name,float& value)
{
	if (token.size()<2) {
		return false;
	}
	
	name = token[0];
	
	if (name<'A' or name>'Z') {
		return false;
	}
	
	try {
		string tmp = token.substr(1);
		value = std::stof(tmp);
	}
	catch(std::exception& e) {
		return false;
	}
	
	return true;
}

GCode::GCode()
{
}

void GCode::LoadFile(const char* filename)
{
	Reset();
	
	int G1 = 0;
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
	float E = 0.0f;
	
	ifstream file(filename);
	string line;
	
	while(file.good()) {
		std::getline(file,line);
		
		m_lines.push_back(line);
		
		vector<string> tokens = Tokens(line);
		
		if (tokens.size() > 0) {
			string& token = tokens[0];
			
			if (token == "G1") {
				for (size_t n=1;n<tokens.size();n++) {
					char name;
					float value;
					
					if (Value(tokens[n],name,value)) {
						switch (name) {
							case 'X':
								X = value;
							break;
							
							case 'Y':
								Y = value;
							break;
							
							case 'Z':
								Z = value;
								
								if (Z > m_height) {
									m_height = Z;
								}
							break;
							
							case 'E':
								E = value;
								m_filament += E;
							break;
						}
					}
				}
			}
		}
	}
}

void GCode::Reset()
{
	m_lines.clear();
	m_filament = 0;
	m_height = 0;
}
