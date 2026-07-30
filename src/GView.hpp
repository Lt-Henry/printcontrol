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

#ifndef PC_G_VIEW
#define PC_G_VIEW

#include "GCode.hpp"

#include <View.h>

#include <map>
#include <string>

namespace pc
{
	class GView: public BView
	{
		public:
		
		GView(BRect frame,const char* name, uint32 resizingMode, uint32 flags);
		virtual ~GView();
		
		virtual void AttachedToWindow(void);
		virtual void Draw(BRect updateRect);
		
		void SetRender(GRender* render)
		{
			fRender = render;
			Invalidate();
		}
		
		protected:
		
		GRender* fRender;
	};
}
#endif
