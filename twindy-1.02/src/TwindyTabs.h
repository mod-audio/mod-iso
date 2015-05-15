//	TwindyTabs.h - JUCE TabbedComponent subclass to handle the workspace and
//				   preferences tabs.
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

#ifndef TWINDYTABS_H_
#define TWINDYTABS_H_

#include "juce_amalgamated.h"

/// JUCE TabbedComponent subclass to handle the workspace and preferences tabs.
class TwindyTabs : public TabbedComponent
{
public:
    /// Constructor.
    TwindyTabs();

    /// We use this to determine when to hide/show windows.
    void currentTabChanged(const int newCurrentTabIndex,
                           const String& newCurrentTabName) override;

private:
    /// We use this to determine when the user's on the Preferences panel.
    bool preferences;
};

#endif
