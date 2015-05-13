//	TwindyPreferences.h - Preferences page for Twindy.
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

#ifndef TWINDYPREFERENCES_H_
#define TWINDYPREFERENCES_H_

#include "TwindyTabbedComponent.h"

class TwindyButtonPreferences;
class WorkspacePreferences;
class StartupPreferences;
class TracktionSchemeEditor;
class MiscPreferences;
class DocsPage;
class AboutPage;
class TracktionScheme;

///	Preferences page for Twindy.
class TwindyPreferences : public TwindyTabbedComponent
{
  public:
	///	Constructor.
	TwindyPreferences();
	///	Destructor.
	~TwindyPreferences();

	///	Called when a tab is clicked.
	void tabWasClicked(int newSelectedIndex,
                       bool clickedTabWasAlreadySelected,
                       const ModifierKeys& currentModifiers);

	///	Called to set the colours for the TwindyTabbedComponent and the various content components.
	void setPrefColours(const Colour& fillColour,
						const Colour& textColour,
						const Colour& selectedOutlineColour,
						const Colour& normalOutlineColour,
						const Colour& deselectedTabColour,
  						const Colour& buttonColour);
	///	Called to set the tracktion scheme used by the preferences page.
	void setTracktionScheme(const String& path);
	///	Returns the tracktionscheme used by the preferences page.
	TracktionScheme *getTracktionScheme() const {return realColours;};
	///	Called to save any changes made to the TwindyProperties instance.
	void saveChanges();
	///	Called to update workspaces' list of workspaces.
	void updateWorkspaces();
  private:
	///	The buttons tab.
	TwindyButtonPreferences *buttons;
	///	The workspaces tab.
	WorkspacePreferences *workspaces;
	///	The startup programs tab.
	StartupPreferences *startup;
	///	The colour editor tab.
	TracktionSchemeEditor *colours;
	///	The miscellaneous tab.
	MiscPreferences *misc;
	///	The docs tab.
	DocsPage *docs;
	///	The about tab.
	AboutPage *about;

	///	The current tracktionscheme for the preferences page.
	TracktionScheme *realColours;
};

#endif
