//	MidiPreferences.cpp - Preferences page for miscellaneous options.
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

#include "MidiPreferences.h"
#include "../TwindyApp.h"
#include "../TwindyPreferences.h"
#include "../TwindyRootWindow.h"
#include "../ExtraComponents/TwindyToggleButton.h"
#include "../ExtraComponents/DrawableTextButton.h"
#include "../Utils.h"

//------------------------------------------------------------------------------
MidiPreferences::MidiPreferences(TwindyPreferences* const p)
    : Component(),
      prefs(p),
      rescanButton("rescanButton")
{
    Font font;
    Colour color;

    font = Font(25.0f, Font::bold);
    title.setText(T("Midi Configuration"), font);

    font = Font(16.0f, Font::plain);
    subtitle1.setText(T("MOD needs you to configure the Midi Interface."), font);
    subtitle2.setText(T("Please select your audio device from the list below."), font);

    rescanButton.setButtonText(T("Rescan"));
    rescanButton.addButtonListener(this);

    addAndMakeVisible(&rescanButton);

    rescanDevices();
}

//------------------------------------------------------------------------------
MidiPreferences::~MidiPreferences()
{
    //deleteAllChildren();
}

//------------------------------------------------------------------------------
StringArray MidiPreferences::getDeviceList() const
{
    StringArray devices;

    return devices;
}

//------------------------------------------------------------------------------
void MidiPreferences::selectDevice(const String& dev)
{
}

//------------------------------------------------------------------------------
void MidiPreferences::resized()
{
    rescanButton.setBounds(getWidth()/2, 70, 100, 25);
}

//------------------------------------------------------------------------------
void MidiPreferences::paint(Graphics& g)
{
    title.drawAt(g, 10, 25);
    subtitle1.drawAt(g, 10, 50);
    subtitle2.drawAt(g, 10, 50 + 15);
}

//------------------------------------------------------------------------------
void MidiPreferences::buttonClicked(Button* button)
{
}

//------------------------------------------------------------------------------
void MidiPreferences::rescanDevices()
{
}

//------------------------------------------------------------------------------
void MidiPreferences::settingsApplied()
{
}
