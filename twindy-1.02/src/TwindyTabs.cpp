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
TwindyTabs::TwindyTabs()
    : TabbedComponent(TabbedButtonBar::TabsAtTop) {}

//------------------------------------------------------------------------------
void TwindyTabs::currentTabChanged(const int newCurrentTabIndex, const String& newCurrentTabName)
{
    if (TwindyRootWindow* const parent = static_cast<TwindyRootWindow*>(getParentComponent()))
    {
        printf("newCurrentTabIndex = %i\n",newCurrentTabIndex);
        parent->setButtonsVisible(newCurrentTabIndex == 2);
        parent->setVisibleUpperPanel(newCurrentTabIndex-1);
    }
}
