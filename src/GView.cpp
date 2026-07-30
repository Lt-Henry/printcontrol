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

#include "GView.hpp"

#include <Window.h>

#include <iostream>

using namespace pc;
using namespace std;

GView::GView(BRect frame,const char* name, uint32 resizingMode, uint32 flags) : BView(frame,name,resizingMode,flags | B_WILL_DRAW), fRender(nullptr)
{
}

GView::~GView()
{
}

void GView::AttachedToWindow(void)
{
	//ResizeTo(Window()->Bounds().right,Window()->Bounds().bottom);
}

void GView::Draw(BRect updateRect)
{
	SetScale(2.0);
	
	rgb_color color_fill;
	color_fill.red = 0xff;
	color_fill.green = 0x0e;
	color_fill.blue = 0x0e;
	SetHighColor(color_fill);
	
	if (fRender) {
		
		for(Layer& layer : fRender->layers) {
			for (Segment& segment : layer.segments) {
				BPoint a(segment.x0,segment.y0);
				BPoint b(segment.x1,segment.y1);
				
				StrokeLine(a,b);
			}
		}
	}
}
