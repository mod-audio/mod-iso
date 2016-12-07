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

#include "ExtraComponents/TwindyTabbedComponent.h"
#include "ExtraComponents/TracktionScheme.h"

#include "PreferencesPages/AudioPreferences.h"
#include "PreferencesPages/MidiPreferences.h"
#include "PreferencesPages/AboutPage.h"

/// Preferences page for Twindy.
class TwindyPreferences : public TwindyTabbedComponent
{
public:
    /// Constructor.
    TwindyPreferences();
    /// Destructor.
    ~TwindyPreferences() override;

    /// Called when a tab is clicked.
    void tabWasClicked(int newSelectedIndex,
                       bool clickedTabWasAlreadySelected,
                       const ModifierKeys& currentModifiers) override;

    /// Called to set the colours for the TwindyTabbedComponent and the various content components.
    void setPrefColours(const Colour& fillColour,
                        const Colour& textColour,
                        const Colour& selectedOutlineColour,
                        const Colour& normalOutlineColour,
                        const Colour& deselectedTabColour,
                        const Colour& buttonColour);

    // Set the embed mixer window used in audio tab
    void setMixerPid(pid_t pid);
    void setMixerWindow(TwindyWindow* window);

    /// Called to save any changes made to the TwindyProperties instance.
    void saveChanges();

    // Get mixer window.
    TwindyWindow* getMixerWindow() const { return mixerWindow; }

    // Get list of audio devices
    StringArray getAudioDevices() const { return audio.getDeviceList(); }

    // Select audio device, used during initialization
    void selectAudioDevice(const String& dev) { audio.selectDevice(dev); }

    // Restart audio stuff
    void restartAudio() { audio.restart(); }

protected:
    // Called when this component's visiblility changes.
    void visibilityChanged() override;

private:
    /// The varius preferences pages
    AudioPreferences audio;
    AboutPage about;

    // Used for audio mixer
    pid_t mixerPid;
    TwindyWindow* mixerWindow;
};

#endif
