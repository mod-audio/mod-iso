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
#include "TwindyWindow.h"
#include "Utils.h"

//------------------------------------------------------------------------------
TwindyPreferences::TwindyPreferences()
    : TwindyTabbedComponent(T("Twindy Preferences"), TwindyTabbedComponent::ModePreferences),
      audio(this),
      midi(this),
      about(),
      mixerPid(-1),
      mixerWindow(nullptr)
{
    addTab(-1, new TwindyTabbedComponentBasicTab(TRANS("Audio")));
    addTab(-1, new TwindyTabbedComponentBasicTab(TRANS("About")));

    setCurrentlySelectedTab(0);
}

//------------------------------------------------------------------------------
TwindyPreferences::~TwindyPreferences()
{
    setContentComponent(nullptr);

    terminateAndWaitForProcess(mixerPid);
}

//------------------------------------------------------------------------------
void TwindyPreferences::tabWasClicked(int newSelectedIndex,
                                      bool clickedTabWasAlreadySelected,
                                      const ModifierKeys& currentModifiers)
{
    if (newSelectedIndex != 0 && mixerWindow != nullptr)
        mixerWindow->hide();

    switch (newSelectedIndex)
    {
    case 0:
        setContentComponent(&audio);
        if (mixerWindow != nullptr)
            mixerWindow->show();
        break;

    case 1:
        setContentComponent(&about);
        break;

    default:
        setContentComponent(nullptr);
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
    setColours(fillColour, textColour, selectedOutlineColour, normalOutlineColour, deselectedTabColour);

//     audio.setColours(fillColour, textColour, buttonColour);
    about.setColours(fillColour, textColour, buttonColour);
}

//------------------------------------------------------------------------------
void TwindyPreferences::setMixerPid(pid_t p)
{
    terminateAndWaitForProcess(mixerPid);
    mixerPid = p;
}

//------------------------------------------------------------------------------
void TwindyPreferences::setMixerWindow(TwindyWindow* w)
{
    if (mixerWindow != nullptr)
    {
        mixerWindow->closeWindow();
        delete mixerWindow;
    }

    mixerWindow = w;

    if (w == nullptr || ! isVisible())
        return;

    if (getCurrentlySelectedTab() == 0)
        w->show();
    else
        w->hide();
}

//------------------------------------------------------------------------------
void TwindyPreferences::saveChanges()
{
    //if (getContentComponent() == &audio)
    //    audio.saveChanges();
}

//------------------------------------------------------------------------------
void TwindyPreferences::visibilityChanged()
{
    if (mixerWindow == nullptr)
        return;

    if (! isVisible())
    {
        mixerWindow->hide();
    }
    else if (getCurrentlySelectedTab() == 0)
    {
        mixerWindow->show();
    }
}
