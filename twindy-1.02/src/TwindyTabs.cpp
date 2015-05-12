//	TwindyTabs.cpp - JUCE TabbedComponent subclass to handle the workspace and
//					 preferences tabs.
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
#include "TwindyTabs.h"
#include "TwindyPreferences.h"
#include "TwindyUpperPanel.h"
#include "TwindyRootWindow.h"
#include "TwindyErrorDisplay.h"

//------------------------------------------------------------------------------
TwindyTabs::TwindyTabs():
TabbedComponent(TabbedButtonBar::TabsAtTop),
preferences(false)
{
	
}

//------------------------------------------------------------------------------
TwindyTabs::~TwindyTabs()
{
	
}

//------------------------------------------------------------------------------
Component *TwindyTabs::createPanelComponentForTab(const int tabIndex,
  										  		  const String& tabName,
  										  		  bool& componentCanBeDeleted)
{
	componentCanBeDeleted = false;
	if(tabIndex == 0)
	{
		TwindyRootWindow *parent = static_cast<TwindyRootWindow *>(getParentComponent());

		if(parent)
			return parent->getPreferencesPanel();
		else
		{
			TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Preferences Tab Error"),
															   TRANS("TwindyTabs has no parent!"));
			return 0;
		}
	}
	else
	{
		TwindyRootWindow *parent = static_cast<TwindyRootWindow *>(getParentComponent());

		if(parent)
		{
			//-1 because tab 0 is always the preferences panel.
			return parent->getUpperPanel(tabIndex-1);
		}
		else
		{
			TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Workspace Tab Error"),
															   TRANS("TwindyTabs has no parent!"));
			return 0;
		}
	}
}

//------------------------------------------------------------------------------
void TwindyTabs::currentTabChanged(const int newCurrentTabIndex,
						   		   const String& newCurrentTabName)
{
	TwindyRootWindow *parent = static_cast<TwindyRootWindow *>(getParentComponent());

	if(parent)
		parent->setVisibleUpperPanel(newCurrentTabIndex-1);

	if(newCurrentTabName == TRANS("Preferences"))
		preferences = true;
	else if(preferences)
		preferences = false;

	if(parent)
		parent->updateColours();
}
