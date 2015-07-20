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
#include "../Utils.h"

#include <alsa/asoundlib.h>

static void scanMidiDevices(StringArray& deviceNamesFound /*, Array<int>& deviceIdsFound*/)
{
    snd_seq_t* handle = nullptr;

    if (snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0) == 0)
    {
        snd_seq_system_info_t* systemInfo = nullptr;
        snd_seq_client_info_t* clientInfo = nullptr;

        if (snd_seq_system_info_malloc(&systemInfo) == 0)
        {
            if (snd_seq_system_info(handle, systemInfo) == 0 && snd_seq_client_info_malloc(&clientInfo) == 0)
            {
                int numClients = snd_seq_system_info_get_cur_clients(systemInfo);

                while (--numClients >= 0)
                {
                    if (snd_seq_query_next_client(handle, clientInfo) == 0)
                    {
                        snd_seq_port_info_t* portInfo = nullptr;

                        if (snd_seq_port_info_malloc(&portInfo) == 0)
                        {
                            int numPorts = snd_seq_client_info_get_num_ports(clientInfo);
                            const int client = snd_seq_client_info_get_client(clientInfo);

                            snd_seq_port_info_set_client(portInfo, client);
                            snd_seq_port_info_set_port(portInfo, -1);

                            while (--numPorts >= 0)
                            {
                                if (snd_seq_query_next_port(handle, portInfo) == 0)
                                {
                                    const char* const deviceName(snd_seq_client_info_get_name(clientInfo));

                                    if (std::strstr(deviceName, "Midi Through") == nullptr &&
                                        std::strcmp(deviceName, "OSS sequencer") != 0)
                                    {
                                        deviceNamesFound.add(deviceName);
                                        /*deviceIdsFound.add();*/
                                    }
                                }
                            }

                            snd_seq_port_info_free(portInfo);
                        }
                    }
                }

                snd_seq_client_info_free(clientInfo);
            }

            snd_seq_system_info_free(systemInfo);
        }

        snd_seq_close(handle);
    }

    deviceNamesFound.appendNumbersToDuplicates(true, true);
}

//------------------------------------------------------------------------------
MidiPreferences::MidiPreferences(TwindyPreferences* const p)
    : Component(),
      prefs(p),
      applyButton("applyButton"),
      rescanButton("rescanButton")
{
    Font font;
    Colour color;

    font = Font(25.0f, Font::bold);
    title.setText(T("MIDI Configuration"), font);

    font = Font(16.0f, Font::plain);
    subtitle1.setText(T("MOD needs you to configure the Midi Interface."), font);
    subtitle2.setText(T("Please select your audio device from the list below."), font);
    labelSettingsChanged1.setText(T("MIDI device settings have been changed,"), font);
    labelSettingsChanged2.setText(T("click 'Apply Now' to restart the audio service."), font);

    applyButton.setButtonText(T("Apply Now"));
    applyButton.addButtonListener(this);

    rescanButton.setButtonText(T("Rescan"));
    rescanButton.addButtonListener(this);

    rescanDevices(false);
}

//------------------------------------------------------------------------------
MidiPreferences::~MidiPreferences()
{
    deviceButtons.clear(false);
}

//------------------------------------------------------------------------------
void MidiPreferences::selectDevices(const StringArray& devs)
{
    for (int i = deviceButtons.size(); --i >=0;)
    {
        TwindyToggleButton* const button(deviceButtons[i]);

        if (devs.contains(button->getButtonText()))
            button->setToggleState(true, false);
        else if (button->getToggleState())
            button->setToggleState(false, false);
    }
}

//------------------------------------------------------------------------------
void MidiPreferences::resized()
{
    applyButton.setBounds(getWidth()/2, 70, 100, 25);
    rescanButton.setBounds(10, 70, 90, 25);
}

//------------------------------------------------------------------------------
void MidiPreferences::paint(Graphics& g)
{
    title.drawAt(g, 10, 25);
    subtitle1.drawAt(g, 10, 50);
    subtitle2.drawAt(g, 10, 50 + 15);

//     if (curSettings.changed)
    {
        labelSettingsChanged1.drawAt(g, getWidth()/2, 48);
        labelSettingsChanged2.drawAt(g, getWidth()/2, 48 + 15);
    }
}

//------------------------------------------------------------------------------
void MidiPreferences::buttonClicked(Button* button)
{
    if (button == &rescanButton)
        return rescanDevices(true);
}

//------------------------------------------------------------------------------
void MidiPreferences::rescanDevices(bool restore)
{
    StringArray selected;

    if (restore)
    {
        for (int i = deviceButtons.size(); --i >=0;)
        {
            TwindyToggleButton* const button(deviceButtons[i]);

            if (button->getToggleState())
                selected.add(button->getButtonText());
        }
    }

    removeAllChildren();

    deviceNames.clear();
    deviceButtons.clear();

    addAndMakeVisible(&applyButton);
    addAndMakeVisible(&rescanButton);

    scanMidiDevices(deviceNames);

    for (int i = deviceNames.size(); --i >=0;)
    {
        const String& deviceName(deviceNames[i]);

        TwindyToggleButton* const button(new TwindyToggleButton(deviceName));
        button->setColours(Colour::fromRGBA(0xffu, 0xd0u, 0xccu, 0xf0u), Colour::fromRGB(0xffu, 0xffu, 0xffu));
        button->setBounds(10, 100+i*30, 300, 25);
        addAndMakeVisible(button);
        deviceButtons.add(button);

        if (restore && selected.contains(deviceName))
            button->setToggleState(true, false);
    }
}

//------------------------------------------------------------------------------
void MidiPreferences::settingsApplied()
{
}
