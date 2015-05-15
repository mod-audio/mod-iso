//	TwindyWindow.h - Class to hold certain details about windows opened in
//					 the window manager.
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

#ifndef TWINDYWINDOW_H_
#define TWINDYWINDOW_H_

typedef unsigned long XID;
typedef XID Window;
struct _XDisplay;
typedef struct _XDisplay Display;

///	Class to hold certain details about windows opened in the window manager.
class TwindyWindow
{
  public:
	///	Constructor.
	/*!
		This takes a note of the current window tree, to compare with in theme
		initialiseWindow() method.
	 */
	TwindyWindow(Display *display, Window window);
	///	Destructor.
	~TwindyWindow();

	///	Called to close the window/program.
	/*!
		Note: This does \emph not get called from the destructor, because
		windows have a tendency to have destroyed themselves by the time we get
		there.  This method should only be called when Twindy wants to signal to
		an open window that it should quit.
	 */
	void closeWindow();

	///	Sets the top left position of the window.
	void setTopLeftPosition(int x, int y);
	///	Sets the size of the window.
	void setSize(int width, int height);

	///	Shows the window.
	void show();
	///	Hides the window.
	void hide();
	///	Returns whether the window is showing or not.
	bool getIsShowing() {return isShowing;};

	///	Returns the X window for this object.
	Window getWindow() {return window;};
  private:
	///	The X11 window this object controls.
	Window window;
	///	The display we're running in.
	Display *display;

	///	X position in the window manager/screen.
	int x;
	///	Y position in the window manager/screen.
	int y;
	///	Width of the window.
	int width;
	///	Height of the window.
	int height;
	///	Whether or not the window is currently showing.
	bool isShowing;
};

#endif
