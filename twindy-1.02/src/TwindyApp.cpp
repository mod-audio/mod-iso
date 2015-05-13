//	TwindyApp.cpp - Main application stuff.
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

#include "juce_amalgamated.h"
#include "TwindyApp.h"
#include "TwindyRootWindow.h"
#include "TwindyHelperStuff.h"
#include "AboutPage.h"

#include <iostream>

using namespace std;

///-----------------------------------------------------------------------------
void TwindyApp::initialise(const String& commandLine)
{
	Rectangle recadendrum = Desktop::getInstance().getMainMonitorArea();

	if(commandLine.contains(T("--help")))
	{
		cout << "Twindy Window Manager" << endl;
		cout << "Version: " << (const char *)AboutPage::version << endl;
		cout << endl;
		cout << "Usage:" << endl;
		cout << "twindy --version --help" << endl;
		cout << endl;
		JUCEApplication::quit();
	}
	else if(commandLine.contains(T("--version")))
	{
		cout << "Twindy Window Manager" << endl;
		cout << "Version: " << (const char *)AboutPage::version << endl;
		cout << endl;
		JUCEApplication::quit();
	}

	//Font::setDefaultSansSerifFontName(T("Bitstream Vera Sans"));

	win = 0;
	TWINDY_DBG_MESSAGE("Creating TwindyRootWindow.");
	win = new TwindyRootWindow();
	TWINDY_DBG_MESSAGE("TwindyRootWindow created.");
    win->centreWithSize(recadendrum.getWidth(), recadendrum.getHeight());
	TWINDY_DBG_MESSAGE("TwindyRootWindow centred and sized.");
	win->toFront(true);
	TWINDY_DBG_MESSAGE("TwindyRootWindow brought to front.");
}

///-----------------------------------------------------------------------------
void TwindyApp::shutdown()
{
	TWINDY_DBG_MESSAGE("About to delete TwindyRootWindow.");
	delete win;
	TWINDY_DBG_MESSAGE("TwindyRootWindow deleted.");
}

START_JUCE_APPLICATION(TwindyApp)
