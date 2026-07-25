/*
The MIT License (MIT)

Copyright (c) 2026 Enrique Medina Gremaldos <quique@necos.es>

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

#include "DataView.hpp"

#include <Window.h>

#include <iostream>

using namespace pc;
using namespace std;

DataView::DataView(BRect frame,const char* name, uint32 resizingMode, uint32 flags) : BView(frame,name,resizingMode,flags | B_WILL_DRAW)
{
}

DataView::~DataView()
{
}

void DataView::Push(map<string,float> data)
{
	for (auto q : data) {
		fData[q.first] = q.second;
	}
	
	Invalidate();
}

void DataView::AttachedToWindow(void)
{
	//ResizeTo(Window()->Bounds().right,Window()->Bounds().bottom);
}

void DataView::Draw(BRect updateRect)
{
	rgb_color color_background;
	color_background.red = 0xf0;
	color_background.green = 0xf0;
	color_background.blue = 0xf0;
	
	rgb_color color_text;
	color_text.red = 0x0e;
	color_text.green = 0x0e;
	color_text.blue = 0x0e;
	
	SetHighColor(color_background);
	FillRect(Bounds());
	
	int line = 16;
	for (auto q : fData) {
		SetHighColor(color_text);
		std::string text = q.first + "=" + std::to_string(q.second);
		DrawString(text.c_str(),BPoint(5,line));
		line = line + 32;
	}
}
