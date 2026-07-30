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
	
	rgb_color color_fly;
	color_fly.red = 0x0e;
	color_fly.green = 0x0e;
	color_fly.blue = 0xff;
	
	rgb_color color_fill;
	color_fill.red = 0xff;
	color_fill.green = 0x0e;
	color_fill.blue = 0x0e;
	
	rgb_color color_back;
	color_back.red = 0x7e;
	color_back.green = 0x7e;
	color_back.blue = 0x7e;
	
	
	if (fRender) {
		clog<<"drawing layer "<<fCurrentLayer<<endl;
		
		if (fCurrentLayer > 0) {
			Layer& layer = fRender->layers[fCurrentLayer-1];
			for (Segment& segment : layer.segments) {
				
				if (segment.type == SegmentType::Fly) {
					continue;
				}
				
				if (segment.type == SegmentType::Fill) {
					SetHighColor(color_back);
				}
				
				StrokeLine(segment.start,segment.end);
			}
		}
		
		Layer& layer = fRender->layers[fCurrentLayer];
		for (Segment& segment : layer.segments) {
			
			if (segment.type == SegmentType::Fly) {
				SetHighColor(color_fly);
			}
			
			if (segment.type == SegmentType::Fill) {
				SetHighColor(color_fill);
			}
			
			StrokeLine(segment.start,segment.end);
		}
	}
}

void GView::MessageReceived(BMessage* message)
{
	float delta;
	switch (message->what) {
		case B_MOUSE_WHEEL_CHANGED:
			if(message->FindFloat("be:wheel_delta_y",&delta) == B_OK) {
				if (delta > 0) {
					fCurrentLayer--;
				}
				else {
					fCurrentLayer++;
				}
				
				if (fCurrentLayer < 0) {
					fCurrentLayer = 0;
				}
				
				if (fRender and fCurrentLayer >= fRender->layers.size()) {
					fCurrentLayer = fRender->layers.size() - 1;
				}
				Invalidate();
			}
		break;

		default:
			BView::MessageReceived(message);
		break;
	}
}
