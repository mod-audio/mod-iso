//  MiscPreferences.h - Preferences page for miscellaneous options.
//  ----------------------------------------------------------------------------
//  This file is part of the Tracktion-themed Twindy window manager.
//  Copyright (c) 2005-2007 Niall Moody.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  ----------------------------------------------------------------------------

#ifndef MIDI_PREFERENCES_H_
#define MIDI_PREFERENCES_H_

#include "../juce_amalgamated.h"
#include <map>

class TwindyWindow;
class TwindyPreferences;

/// Preferences page for audio.
class MidiPreferences : public Component,
                        private ButtonListener
{
public:
    // Constructor.
    MidiPreferences(TwindyPreferences* const p);

    // Destructor.
    ~MidiPreferences() override;

    // Get list of devices.
    StringArray getDeviceList() const;

    // Select device.
    void selectDevice(const String& dev);

protected:
    // Places and sizes the various components.
    void resized() override;

    // Draws the component.
    void paint(Graphics& g) override;

    // Called when the button is clicked.
    void buttonClicked(Button* button) override;

private:
    // Parent
    TwindyPreferences* const prefs;

    // Widgets.
    TextButton rescanButton;
    DrawableText title, subtitle1, subtitle2;

    // Rescan audio devices.
    void rescanDevices();

    // hide apply now section
    void settingsApplied();
};

#endif
