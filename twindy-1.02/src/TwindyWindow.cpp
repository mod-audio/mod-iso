//	TwindyWindow.cpp - Class to hold certain details about windows opened in
//					   the window manager.
//	----------------------------------------------------------------------------
//	This file is part of the Tracktion-themed Twindy window manager.
//	Copyright (c) 2005-2007 Niall Moody.
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//	----------------------------------------------------------------------------

#include "TwindyWindow.h"
#include "TwindyHelperStuff.h"
#include <X11/Xlib.h>

///-----------------------------------------------------------------------------
TwindyWindow::TwindyWindow(Display *display, Window window):
x(0), y(0),
width(400), height(300),
isShowing(false)
{
	this->display = display;
	this->window = window;
}

///-----------------------------------------------------------------------------
TwindyWindow::~TwindyWindow()
{
	//This bit's not necessary, because the window should take care of it
	//anyway, and we'll sometimes get here only to find the window's already
	//destroyed itself.
	//if(isShowing)
		//hide();

	//This is also the reason we don't call XRemoveFromSaveSet(), although it
	//doesn't seem to cause problems that we don't call it.

	//Ditto...
	//std::cout << "TwindyWindow: Destroying window " << window << std::endl;
	//XDestroyWindow(display, window);
}

/*///-----------------------------------------------------------------------------
void TwindyWindow::initialiseWindow()
{
	unsigned int i;
	Window rootWindow2;
	Window parentWindow;
	Window *childrenNew;
	unsigned int numChildrenNew;
	bool foundNewWindow = false;

	XQueryTree(display,
			   DefaultRootWindow(display),
			   &rootWindow2,
			   &parentWindow,
			   &childrenNew,
			   &numChildrenNew);
	std::cout << "numChildrenNew: " << numChildrenNew << std::endl;

	if((numChildrenOld > 0) && (numChildrenNew > numChildrenOld))
	{
		window = new Window;
		///	Get the Window handle for the new window.
		for(i=0;i<numChildrenOld;i++)
		{
			std::cout << "childrenOld[" << i << "]:" << childrenOld[i] << std::endl;
			std::cout << "childrenNew[" << i << "]:" << childrenNew[i] << std::endl;
			if(childrenOld[i] != childrenNew[i])
			{
				*window = childrenNew[i];
				foundNewWindow = true;
				break;
			}
		}
		if(!foundNewWindow)
			*window = childrenNew[i];
	}

	XFree(childrenOld);
	XFree(childrenNew);

	XSetWindowBorderWidth(display, *window, 0);
	XSetInputFocus(display, *window, RevertToPointerRoot, CurrentTime);
	//Reparent the window so that it closes when Twindy does.
	std::cout << "JUCE rootWindow: " << rootWindow << std::endl;
	XReparentWindow(display, *window, rootWindow, 0, 0);
	XMapRaised(display, *window);

	isShowing = true;
}*/

///-----------------------------------------------------------------------------
void TwindyWindow::closeWindow()
{
	//The following mostly borrowed from WindowLab:
	int i, num;
	bool handlesDelete = false;
	Atom *protocols;
	XClientMessageEvent event;

	Atom wm_protos = XInternAtom(display, "WM_PROTOCOLS", false);
	Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", false);

	if(XGetWMProtocols(display, window, &protocols, &num))
	{
		for(i=0;i<num;i++)
		{
			if(protocols[i] == wm_delete)
			{
				handlesDelete = true;
				break;
			}
		}
		XFree(protocols);
	}

	if(handlesDelete)
	{
		event.type = ClientMessage;
		event.window = window;
		event.message_type = wm_protos;
		event.format = 32;
		event.data.l[0] = wm_delete;
		event.data.l[1] = CurrentTime;

		XSendEvent(display, window, false, NoEventMask, (XEvent *)&event);
	}
	else
	{
		TWINDY_DBG_MESSAGE("Window doesn't respond to WM_DELETE_WINDOW, killing forcefully");
		XKillClient(display, window);
	}
}

///-----------------------------------------------------------------------------
void TwindyWindow::setTopLeftPosition(int x, int y)
{
	this->x = x;
	this->y = y;

	if(window)
		XMoveWindow(display, window, x, y);
}

///-----------------------------------------------------------------------------
void TwindyWindow::setSize(int width, int height)
{
	this->width = width;
	this->height = height;

	if(window)
		XResizeWindow(display, window, width, height);
}

///-----------------------------------------------------------------------------
void TwindyWindow::show()
{
	isShowing = true;
	if(window)
		XMapRaised(display, window);
	else
		TWINDY_DBG_MESSAGE("Trying to show a non-existent window");
}

///-----------------------------------------------------------------------------
void TwindyWindow::hide()
{
	isShowing = false;
	if(window)
		XUnmapWindow(display, window);
	else
		TWINDY_DBG_MESSAGE("Trying to hide a non-existent window.");
}
