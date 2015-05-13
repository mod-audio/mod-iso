//	TwindyPreferences.cpp - Preferences page for Twindy.
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
#include "TwindyPreferences.h"
#include "ButtonPreferences.h"
#include "WorkspacePreferences.h"
#include "StartupPreferences.h"
#include "TracktionSchemeEditor.h"
#include "MiscPreferences.h"
#include "DocsPage.h"
#include "AboutPage.h"
#include "TracktionScheme.h"

//------------------------------------------------------------------------------
TwindyPreferences::TwindyPreferences():
TwindyTabbedComponent(T("Twindy Preferences"))
{
	realColours = new TracktionScheme(T("~/.twindy/Default.tracktionscheme"));
	buttons = new TwindyButtonPreferences();
	workspaces = new WorkspacePreferences();
	startup = new StartupPreferences();
	colours = new TracktionSchemeEditor();
	misc = new MiscPreferences();
	//docs = new DocsPage();
	about = new AboutPage();
	addTab(-1, new TwindyTabbedComponentBasicTab(TRANS("Buttons")));
	addTab(-1, new TwindyTabbedComponentBasicTab(TRANS("Workspaces")));
	addTab(-1, new TwindyTabbedComponentBasicTab(TRANS("Startup")));
	addTab(-1, new TwindyTabbedComponentBasicTab(TRANS("Colours")));
	addTab(-1, new TwindyTabbedComponentBasicTab(TRANS("Misc")));
	//addTab(-1, new TwindyTabbedComponentBasicTab(T("Docs")));
	addTab(-1, new TwindyTabbedComponentBasicTab(TRANS("About")));
	setCurrentlySelectedTab(0);
}

//------------------------------------------------------------------------------
TwindyPreferences::~TwindyPreferences()
{
	if(buttons != getContentComponent())
		delete buttons;
	if(workspaces != getContentComponent())
		delete workspaces;
	if(startup != getContentComponent())
		delete startup;
	if(colours != getContentComponent())
		delete colours;
	if(misc != getContentComponent())
		delete misc;
	/*if(docs != getContentComponent())
		delete docs;*/
	if(about != getContentComponent())
		delete about;
	delete realColours;
}

//------------------------------------------------------------------------------
void TwindyPreferences::tabWasClicked(int newSelectedIndex,
									  bool clickedTabWasAlreadySelected,
									  const ModifierKeys& currentModifiers)
{
	switch(newSelectedIndex)
	{
		case 0:
			setContentComponent(buttons);
			break;
		case 1:
			setContentComponent(workspaces);
			break;
		case 2:
			setContentComponent(startup);
			break;
		case 3:
			setContentComponent(colours);
			break;
		case 4:
			setContentComponent(misc);
			break;
		case 5:
		/*	setContentComponent(docs);
			break;
		case 6:*/
			setContentComponent(about);
			break;
	}
}

//------------------------------------------------------------------------------
void TwindyPreferences::setPrefColours(const Colour& fillColour,
									   const Colour& textColour,
									   const Colour& selectedOutlineColour,
									   const Colour& normalOutlineColour,
									   const Colour& deselectedTabColour,
  									   const Colour& buttonColour)
{
	setColours(fillColour,
			   textColour,
			   selectedOutlineColour,
			   normalOutlineColour,
			   deselectedTabColour);
	buttons->setColours(fillColour, textColour, buttonColour);
	workspaces->setColours(fillColour, textColour, buttonColour);
	startup->setColours(fillColour, textColour, buttonColour);
	colours->setColours(fillColour, textColour, buttonColour);
	misc->setColours(fillColour, textColour, buttonColour);
	//docs->setColours(fillColour, textColour, buttonColour);
	about->setColours(fillColour, textColour, buttonColour);
}

//------------------------------------------------------------------------------
void TwindyPreferences::setTracktionScheme(const String& path)
{
	delete realColours;
	realColours = new TracktionScheme(path);
}

//------------------------------------------------------------------------------
void TwindyPreferences::saveChanges()
{
	if(getContentComponent() == buttons)
		buttons->saveChanges();
	else if(getContentComponent() == startup)
		startup->saveChanges();
	else if(getContentComponent() == workspaces)
		workspaces->saveChanges();
}

//------------------------------------------------------------------------------
void TwindyPreferences::updateWorkspaces()
{
	workspaces->updateWorkspaces();
}
