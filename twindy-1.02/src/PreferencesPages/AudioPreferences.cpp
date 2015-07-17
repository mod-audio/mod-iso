//	AudioPreferences.cpp - Preferences page for miscellaneous options.
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

#include "AudioPreferences.h"
#include "../TwindyApp.h"
#include "../TwindyPreferences.h"
#include "../TwindyRootWindow.h"
#include "../ExtraComponents/TwindyToggleButton.h"
#include "../ExtraComponents/DrawableTextButton.h"
#include "../Utils.h"
#include <iostream>

//#define ALSA_PCM_NEW_HW_PARAMS_API
//#define ALSA_PCM_NEW_SW_PARAMS_API
#include <alsa/asoundlib.h>

#define JUCE_ALSA_LOG(dbgtext)   { juce::String tempDbgBuf ("ALSA: "); tempDbgBuf << dbgtext; Logger::writeToLog (tempDbgBuf); DBG (tempDbgBuf); }
//#define JUCE_CHECKED_RESULT(x)   (logErrorMessage (x, __LINE__))
#define JUCE_CHECKED_RESULT(x)   (x)

/*
 * Get next power of 2.
 */
static int nextPowerOfTwo(int size) noexcept
{
    // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    --size;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return ++size;
}

//------------------------------------------------------------------------------
static void enumerateAlsaSoundcards(StringArray& inputNames, StringArray& outputNames,
                                    StringArray& inputIds, StringArray& outputIds)
{
    snd_ctl_t* handle = nullptr;
    snd_ctl_card_info_t* info = nullptr;
    snd_ctl_card_info_alloca (&info);

    int cardNum = -1;

    while (outputIds.size() + inputIds.size() <= 64)
    {
        snd_card_next (&cardNum);

        if (cardNum < 0)
            break;

        if (JUCE_CHECKED_RESULT (snd_ctl_open (&handle, ("hw:" + String (cardNum)).toUTF8(), SND_CTL_NONBLOCK)) >= 0)
        {
            if (JUCE_CHECKED_RESULT (snd_ctl_card_info (handle, info)) >= 0)
            {
                String cardId (snd_ctl_card_info_get_id (info));

                if (cardId.removeCharacters (T("0123456789")).isEmpty())
                    cardId = String (cardNum);

                String cardName = snd_ctl_card_info_get_name (info);

                if (cardName.isEmpty())
                    cardName = cardId;

                int device = -1;

                snd_pcm_info_t* pcmInfo;
                snd_pcm_info_alloca (&pcmInfo);

                for (;;)
                {
                    if (snd_ctl_pcm_next_device (handle, &device) < 0 || device < 0)
                        break;

                    snd_pcm_info_set_device (pcmInfo, device);

                    for (int subDevice = 0, nbSubDevice = 1; subDevice < nbSubDevice; ++subDevice)
                    {
                        snd_pcm_info_set_subdevice (pcmInfo, subDevice);
                        snd_pcm_info_set_stream (pcmInfo, SND_PCM_STREAM_CAPTURE);
                        const bool isInput = (snd_ctl_pcm_info (handle, pcmInfo) >= 0);

                        snd_pcm_info_set_stream (pcmInfo, SND_PCM_STREAM_PLAYBACK);
                        const bool isOutput = (snd_ctl_pcm_info (handle, pcmInfo) >= 0);

                        if (! (isInput || isOutput))
                            continue;

                        if (nbSubDevice == 1)
                            nbSubDevice = snd_pcm_info_get_subdevices_count (pcmInfo);

                        String id, name;

                        if (nbSubDevice == 1)
                        {
                            id << "hw:" << cardId << "," << device;
                            name << cardName << ", " << snd_pcm_info_get_name (pcmInfo);
                        }
                        else
                        {
                            id << "hw:" << cardId << "," << device << "," << subDevice;
                            name << cardName << ", " << snd_pcm_info_get_name (pcmInfo)
                                  << " {" <<  snd_pcm_info_get_subdevice_name (pcmInfo) << "}";
                        }

                        JUCE_ALSA_LOG ("Soundcard ID: " << id << ", name: '" << name
                                        << ", isInput:" << isInput << ", isOutput:" << isOutput << "\n");

                        if (isInput)
                        {
                            inputNames.add (name);
                            inputIds.add (id);
                        }

                        if (isOutput)
                        {
                            outputNames.add (name);
                            outputIds.add (id);
                        }
                    }
                }
            }

            JUCE_CHECKED_RESULT (snd_ctl_close (handle));
        }
    }
}

//------------------------------------------------------------------------------
static void getDeviceBufferSizes (snd_pcm_t* handle, Array<uint>& bufSizes)
{
    snd_pcm_hw_params_t* hwParams;
    snd_pcm_hw_params_alloca (&hwParams);

    juce::String tempDbgBuf ("ALSA: getDeviceBufferSizes: ");

    if (snd_pcm_hw_params_any (handle, hwParams) >= 0)
    {
        int dir = 0;
        snd_pcm_uframes_t minSize = 0, maxSize = 0;
        snd_pcm_hw_params_get_period_size_min (hwParams, &minSize, &dir);
        snd_pcm_hw_params_get_period_size_max (hwParams, &maxSize, &dir);

        minSize = jmax(nextPowerOfTwo(minSize), 16);
        maxSize = jmin(maxSize, 8192U);

        for (snd_pcm_uframes_t s = minSize; s <= maxSize; s = nextPowerOfTwo(s+1))
        {
            if (snd_pcm_hw_params_test_period_size (handle, hwParams, minSize, dir) == 0)
            {
                bufSizes.addIfNotAlreadyThere (s);

                tempDbgBuf << (int)s << " ";
            }

            if (s == 8192)
                break;
        }
    }

    Logger::writeToLog (tempDbgBuf);
    DBG (tempDbgBuf);

    //snd_pcm_hw_params_free (hwParams);
}

//------------------------------------------------------------------------------
static void getDeviceSampleRates (snd_pcm_t* handle, Array<double>& rates)
{
    const int ratesToTry[] = { 22050, 32000, 44100, 48000, 88200, 96000, 176400, 192000, 0 };

    juce::String tempDbgBuf ("ALSA: getDeviceSampleRates: ");

    snd_pcm_hw_params_t* hwParams;
    snd_pcm_hw_params_alloca (&hwParams);

    for (int i = 0; ratesToTry[i] != 0; ++i)
    {
        if (snd_pcm_hw_params_any (handle, hwParams) >= 0
             && snd_pcm_hw_params_test_rate (handle, hwParams, ratesToTry[i], 0) == 0)
        {
            rates.addIfNotAlreadyThere ((double) ratesToTry[i]);

            tempDbgBuf << ratesToTry[i] << " ";
        }
    }

    Logger::writeToLog (tempDbgBuf);
    DBG (tempDbgBuf);

    //snd_pcm_hw_params_free (hwParams);
}

//------------------------------------------------------------------------------
static void getDeviceNumChannels (snd_pcm_t* handle, unsigned int* minChans, unsigned int* maxChans)
{
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca (&params);

    if (snd_pcm_hw_params_any (handle, params) >= 0)
    {
        snd_pcm_hw_params_get_channels_min (params, minChans);
        snd_pcm_hw_params_get_channels_max (params, maxChans);

        JUCE_ALSA_LOG ("getDeviceNumChannels: " << (int) *minChans << " " << (int) *maxChans);

        // some virtual devices (dmix for example) report 10000 channels , we have to clamp these values
        *maxChans = jmin (*maxChans, 32u);
        *minChans = jmin (*minChans, *maxChans);
    }
    else
    {
        JUCE_ALSA_LOG ("getDeviceNumChannels failed");
    }

    //snd_pcm_hw_params_free (params);
}

//------------------------------------------------------------------------------
static void getDeviceProperties (const String& deviceID,
                                 unsigned int& minChansOut,
                                 unsigned int& maxChansOut,
                                 unsigned int& minChansIn,
                                 unsigned int& maxChansIn,
                                 Array<uint>& bufSizes,
                                 Array<double>& rates,
                                 bool testOutput,
                                 bool testInput)
{
    minChansOut = maxChansOut = minChansIn = maxChansIn = 0;

    if (deviceID.isEmpty())
        return;

    JUCE_ALSA_LOG ("getDeviceProperties(" << deviceID.toUTF8() << ")");

    snd_pcm_info_t* info;
    snd_pcm_info_alloca (&info);

    if (testOutput)
    {
        snd_pcm_t* pcmHandle;

        if (JUCE_CHECKED_RESULT (snd_pcm_open (&pcmHandle, deviceID.toUTF8(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) >= 0)
        {
            getDeviceNumChannels (pcmHandle, &minChansOut, &maxChansOut);
            getDeviceBufferSizes (pcmHandle, bufSizes);
            getDeviceSampleRates (pcmHandle, rates);

            snd_pcm_close (pcmHandle);
        }
    }

    if (testInput)
    {
        snd_pcm_t* pcmHandle;

        if (JUCE_CHECKED_RESULT (snd_pcm_open (&pcmHandle, deviceID.toUTF8(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK) >= 0))
        {
            getDeviceNumChannels (pcmHandle, &minChansIn, &maxChansIn);

            if (bufSizes.size() == 0)
                getDeviceBufferSizes (pcmHandle, bufSizes);

            if (rates.size() == 0)
                getDeviceSampleRates (pcmHandle, rates);

            snd_pcm_close (pcmHandle);
        }
    }
}

using namespace std;

//------------------------------------------------------------------------------
AudioPreferences::AudioPreferences(TwindyPreferences* const p)
    : Component(),
      prefs(p),
      applyButton("applyButton"),
      deviceBox("deviceBox"),
      sampleRateBox("sampleRateBox"),
      bufferSizeBox("bufferSizeBox")
{
    Font font;
    Colour color;

    font = Font(25.0f, Font::bold);
    title.setText(T("Audio Configuration"), font);

    font = Font(16.0f, Font::plain);
    subtitle1.setText(T("MOD needs you to configure the Audio Interface."), font);
    subtitle2.setText(T("Please select your audio device from the list below."), font);
    labelSettingsChanged1.setText(T("Audio device settings have been changed,"), font);
    labelSettingsChanged2.setText(T("click 'Apply Now' to restart the audio service."), font);

    font = Font(15.0f, Font::plain);
    labelAdvanced.setText(T("Sample rate and buffer size. Change values only if you know what you're doing."), font);

    font = Font(14.0f, Font::plain);
    labelBufSize.setText(T("Buffer Size"), font);
    labelSampleRate.setText(T("Sample Rate"), font);

    applyButton.setButtonText(T("Apply Now"));
    applyButton.addButtonListener(this);
    bufferSizeBox.addListener(this);
    sampleRateBox.addListener(this);
    deviceBox.addListener(this);

    addAndMakeVisible(&applyButton);
    addAndMakeVisible(&deviceBox);
    addAndMakeVisible(&sampleRateBox);
    addAndMakeVisible(&bufferSizeBox);

    rescanDevices();

    // pick the best available soundcard
    const int numItems(deviceBox.getNumItems());

    // let's try some external ones first
    // if they're connected the user probably wants to use them
    for (int i=numItems; --i>=0;)
    {
        // TODO more checks
        if (deviceBox.getItemText(i).compareIgnoreCase(T("USB")))
        {
            deviceBox.setSelectedItemIndex(i);
            settingsApplied();
            return;
        }
    }

    // no external cards, select the first one that is not HDMI
    for (int i=numItems; --i>=0;)
    {
        if (deviceBox.getItemText(i).compareIgnoreCase(T("HDMI")))
            continue;

        deviceBox.setSelectedItemIndex(i);
        settingsApplied();
        return;
    }

    // fallback
    deviceBox.setSelectedItemIndex(0);
    settingsApplied();
}

//------------------------------------------------------------------------------
AudioPreferences::~AudioPreferences()
{
    //deleteAllChildren();
}

//------------------------------------------------------------------------------
StringArray AudioPreferences::getDeviceList() const
{
    StringArray devices;

    devices.add(deviceBox.getText());

    for (int i=0, numItems=deviceBox.getNumItems(); i<numItems; ++i)
        devices.addIfNotAlreadyThere(deviceBox.getItemText(i));

    return devices;
}

//------------------------------------------------------------------------------
void AudioPreferences::selectDevice(const String& dev)
{
    const int oldSelection(deviceBox.getSelectedItemIndex());

    for (int i=deviceBox.getNumItems(); --i>=0;)
    {
        if (deviceBox.getItemText(i) != dev)
            continue;

        deviceBox.setSelectedItemIndex(i);
    }

    // force scan of selected device/card
    if (oldSelection == deviceBox.getSelectedItemIndex())
        comboBoxChanged(&deviceBox);

    // press the button
    buttonClicked(&applyButton);
}

void AudioPreferences::restart()
{
    buttonClicked(&applyButton);
}

//------------------------------------------------------------------------------
void AudioPreferences::resized()
{
    deviceBox.setBounds(10, 70, 250, 20);
    applyButton.setBounds(getWidth()/2, 70, 100, 25);
    sampleRateBox.setBounds(20, 160, 150, 20);
    bufferSizeBox.setBounds(200, 160, 150, 20);
}

//------------------------------------------------------------------------------
void AudioPreferences::paint(Graphics& g)
{
    title.drawAt(g, 10, 25);
    subtitle1.drawAt(g, 10, 50);
    subtitle2.drawAt(g, 10, 50 + 15);
    labelAdvanced.drawAt(g, 10, 140);
    labelSampleRate.drawAt(g, 20, 155);
    labelBufSize.drawAt(g, 200, 155);

    if (curSettings.changed)
    {
        labelSettingsChanged1.drawAt(g, getWidth()/2, 50);
        labelSettingsChanged2.drawAt(g, getWidth()/2, 50 + 15);
    }
}

//------------------------------------------------------------------------------
void AudioPreferences::buttonClicked(Button* button)
{
    if (button != &applyButton)
        return;

    settingsApplied();

    unsigned int numIns  = 2;
    unsigned int numOuts = 2;

    if (curSettings.deviceId.isNotEmpty())
    {
        const DeviceInfo& devInfo(cachedDevInfo[curSettings.deviceId]);
        numIns  = devInfo.maxChansIn;
        numOuts = devInfo.maxChansOut;
    }

    printf("--------------------------------------------------------------\n");
    printf("Dev IO changed to %u | %u\n", numIns, numOuts);
    printf("--------------------------------------------------------------\n");

    ::setenv("MOD_INGEN_NUM_AUDIO_INS",  String(numIns).toUTF8(), 1);
    ::setenv("MOD_INGEN_NUM_AUDIO_OUTS", String(numOuts).toUTF8(), 1);

    StringArray args;
    args.add(T("jackd"));
    args.add(T("-R"));
    args.add(T("-P"));
    args.add(T("89"));
    args.add(T("-Z"));

    if (true)
    {
        // using JACK1
        args.add(T("-X"));
        args.add(T("alsa_midi"));
    }
    else
    {
        // using JACK2
        args.add(T("-S"));
    }

    args.add(T("-d"));
    args.add(T("alsa"));

    if (curSettings.deviceId.isNotEmpty())
    {
        args.add(T("-d"));
        args.add(curSettings.deviceId);

        if (deviceBox.getText().containsIgnoreCase(T("usb")))
        {
            args.add(T("-n"));
            args.add(T("3"));
        }
    }

    args.add(T("-r"));
    args.add(curSettings.sampleRate);
    args.add(T("-p"));
    args.add(curSettings.bufferSize);
    args.add(T("-s"));

    TwindyApp* const app(static_cast<TwindyApp*>(JUCEApplication::getInstance()));
    app->restartJackd(args);
}

//------------------------------------------------------------------------------
void AudioPreferences::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &bufferSizeBox)
    {
        curSettings.bufferSizeChanged = (curSettings.bufferSize != bufferSizeBox.getText());
        applyButton.setVisible(curSettings.check());
        repaint();
        return;
    }

    if (comboBoxThatHasChanged == &sampleRateBox)
    {
        curSettings.sampleRateChanged = (curSettings.sampleRate != sampleRateBox.getText());
        applyButton.setVisible(curSettings.check());
        repaint();
        return;
    }

    if (comboBoxThatHasChanged != &deviceBox)
        return;

    const String oldSampleRate(sampleRateBox.getText());
    const String oldBufferSize(bufferSizeBox.getText());

    sampleRateBox.clear();
    bufferSizeBox.clear();

    if (deviceBox.getSelectedItemIndex() < 0)
        return;
    if (deviceBox.getSelectedId() <= 0)
        return;

    const String deviceId(outputIds[deviceBox.getSelectedId()-1]);

    {
        curSettings.deviceIdChanged = (curSettings.deviceId != deviceId);
        applyButton.setVisible(curSettings.check());
        repaint();
    }

    uint minChansOut = 0;
    uint maxChansOut = 0;
    uint minChansIn  = 0;
    uint maxChansIn  = 0;
    Array<uint> bufferSizes;
    Array<double> sampleRates;

    // check for cached info first
    if (cachedDevInfo.count(deviceId))
    {
        const DeviceInfo& deviceInfo(cachedDevInfo.at(deviceId));

        minChansOut = deviceInfo.minChansOut;
        maxChansOut = deviceInfo.maxChansOut;
        minChansIn  = deviceInfo.minChansIn;
        maxChansIn  = deviceInfo.maxChansIn;
        bufferSizes = deviceInfo.bufferSizes;
        sampleRates = deviceInfo.sampleRates;
    }
    else
    {
        getDeviceProperties(deviceId,
                            minChansOut, maxChansOut, minChansIn, maxChansIn,
                            bufferSizes, sampleRates, true, true);

        DeviceInfo deviceInfo;
        deviceInfo.minChansOut = minChansOut;
        deviceInfo.maxChansOut = maxChansOut;
        deviceInfo.minChansIn  = minChansIn;
        deviceInfo.maxChansIn  = maxChansIn;
        deviceInfo.bufferSizes = bufferSizes;
        deviceInfo.sampleRates = sampleRates;

        cachedDevInfo[deviceId] = deviceInfo;
    }

    for (int i=0, size=sampleRates.size(); i<size; ++i)
    {
        const String text(sampleRates[i]);
        sampleRateBox.addItem(text, i+1);

        if (oldSampleRate == text)
            sampleRateBox.setSelectedItemIndex(i);
    }

    for (int i=0, size=bufferSizes.size(); i<size; ++i)
    {
        const String text(bufferSizes[i]);
        bufferSizeBox.addItem(text, i+1);

        if (oldBufferSize == text)
            bufferSizeBox.setSelectedItemIndex(i);
    }

    if (sampleRateBox.getSelectedItemIndex() < 0)
    {
        // no sample rate selected yet, try 48000 and 44100
        int index;

        /**/ if ((index = sampleRates.indexOf(48000.0)) != -1)
            index += 1;
        else if ((index = sampleRates.indexOf(44100.0)) != -1)
            index += 1;
        else if (sampleRates[0] >= 96000.0)
            index = 1;
        else
            index = 1 + std::floor(float(sampleRates.size())/2.0f);

        sampleRateBox.setSelectedId(index);
    }

    if (bufferSizeBox.getSelectedItemIndex() < 0)
    {
        // no buffer size selected yet, try 512, 256 and 1024
        int index;

        /**/ if ((index = bufferSizes.indexOf(512)) != -1)
            index += 1;
        else if ((index = bufferSizes.indexOf(256)) != -1)
            index += 1;
        else if ((index = bufferSizes.indexOf(1024)) != -1)
            index += 1;
        else if (bufferSizes[0] >= 128)
            index = 1;
        else
            index = 1 + std::floor(float(bufferSizes.size())/2.0f);

        bufferSizeBox.setSelectedId(index);
    }

    StringArray args;
    args.add(T("terminator"));
    args.add(T("-u"));
    args.add(T("-T"));
    args.add(T("__mod_mixer__"));
    args.add(T("-e"));
    args.add(T("alsamixer"));
    args.add(T("-D"));
    args.add(deviceId.upToFirstOccurrenceOf(T(","), false, false));

    prefs->setMixerPid(startProcess(args));
}

//------------------------------------------------------------------------------
void AudioPreferences::rescanDevices()
{
    deviceBox.clear();
    inputNames.clear();
    inputIds.clear();
    outputNames.clear();
    outputIds.clear();

    enumerateAlsaSoundcards(inputNames, outputNames, inputIds, outputIds);

    inputNames.appendNumbersToDuplicates(false, true);
    outputNames.appendNumbersToDuplicates(false, true);

    int i=0;
    for (int size=outputNames.size(); i<size; ++i)
    {
        const String& name(outputNames[i]);

        if (name.startsWith(T("Loopback, Loopback PCM")))
            continue;

        deviceBox.addItem(name, i+1);
    }

    if (i == 0)
    {
        outputNames.add(T("Default"));
        deviceBox.addItem(T("Default"), -1);
    }
}

//------------------------------------------------------------------------------
void AudioPreferences::settingsApplied()
{
    curSettings.deviceId   = (deviceBox.getSelectedId() > 0) ? outputIds[deviceBox.getSelectedId()-1] : T("");
    curSettings.bufferSize = bufferSizeBox.getText();
    curSettings.sampleRate = sampleRateBox.getText();
    curSettings.deviceIdChanged   = false;
    curSettings.bufferSizeChanged = false;
    curSettings.sampleRateChanged = false;
    curSettings.changed           = false;
    applyButton.setVisible(false);
}
