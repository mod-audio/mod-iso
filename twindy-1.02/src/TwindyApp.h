//	TwindyApp.h - Main application stuff.
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

#ifndef MAIN_H_
#define MAIN_H_

class TwindyRootWindow;

///	Handles the main application stuff.
class TwindyApp : public JUCEApplication
{
  public:
	///	Constructor.
	TwindyApp() {};
	///	Destructor.
	~TwindyApp() {};

	///	Where all the important stuff happens for this class.
	void initialise(const String& commandLine);
	///	Not used by us?
	void shutdown();

	///	Returns the name of the application (duh...).
	const String getApplicationName() {return T("Twindy Window Manager");};
	///	Returns the current version.
	const String getApplicationVersion() {return T("1.01");};
	///	Whether or not we want to allow more than one instance to run.
	/*!
		\todo Do we?
	 */
	bool moreThanOneInstanceAllowed() {return true;};
	///	Not used right now?
	void anotherInstanceStarted(const String& commandLine) {};

	///	Returns the root window.
	TwindyRootWindow *getRootWindow() const {return win;};
	///	Sets the root window.
	/*!
		\todo Explain why this is necessary.
	 */
	void setRootWindow(TwindyRootWindow *newWin) {win = newWin;};
  private:
	///	The window.
	TwindyRootWindow *win;
};

#endif
