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
#include "../TwindyProperties.h"
#include "../TwindyRootWindow.h"
#include "../ExtraComponents/TwindyToggleButton.h"
#include "../ExtraComponents/DrawableTextButton.h"
#include "../ExtraComponents/Clock.h"
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
AudioPreferences::AudioPreferences()
    : Component(),
      deviceBox("deviceBox"),
      sampleRateBox("sampleRateBox"),
      bufferSizeBox("bufferSizeBox")
{
    deviceBox.addListener(this);
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
            return;
        }
    }

    // no external cards, select the first one that is not HDMI
    for (int i=numItems; --i>=0;)
    {
        if (deviceBox.getItemText(i).compareIgnoreCase(T("HDMI")))
            continue;

        deviceBox.setSelectedItemIndex(i);
        return;
    }

    // fallback
    deviceBox.setSelectedItemIndex(0);
}

//------------------------------------------------------------------------------
AudioPreferences::~AudioPreferences()
{
    //deleteAllChildren();
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

    inputNames.appendNumbersToDuplicates (false, true);
    outputNames.appendNumbersToDuplicates (false, true);

    for (int i=0, size=outputNames.size(); i<size; ++i)
    {
        const String& name(outputNames[i]);

        if (name.startsWith(T("Loopback, Loopback PCM")))
            continue;

        deviceBox.addItem(name, i+1);
    }
}

//------------------------------------------------------------------------------
void AudioPreferences::resized()
{
    deviceBox.setBounds(10, 5, 250, 20);
    sampleRateBox.setBounds(20, 30, 240, 20);
    bufferSizeBox.setBounds(20, 55, 240, 20);
}

//------------------------------------------------------------------------------
void AudioPreferences::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged != &deviceBox)
        return;

    const String oldSampleRate(sampleRateBox.getText());
    const String oldBufferSize(bufferSizeBox.getText());

    sampleRateBox.clear();
    bufferSizeBox.clear();

    if (deviceBox.getSelectedItemIndex() < 0)
        return;

    const String deviceId(deviceBox.getText());

    uint minChansOut = 0;
    uint maxChansOut = 0;
    uint minChansIn = 0;
    uint maxChansIn = 0;
    Array<uint> bufferSizes;
    Array<double> sampleRates;
    getDeviceProperties(outputIds[deviceBox.getSelectedId()-1],
                        minChansOut, maxChansOut, minChansIn, maxChansIn,
                        bufferSizes, sampleRates, true, true);

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
        else if (sampleRates[0] >= 96000)
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
}

#if 0
//------------------------------------------------------------------------------
void AudioPreferences::buttonClicked(Button *button)
{
	TwindyProperty tempProp;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();
	TwindyProperties *prop = root->getProperties();

	if(button == includeSeconds)
	{
		tempProp.name = T("ClockIncludeSeconds");
		if(button->getToggleState())
		{
			tempProp.value = T("TRUE");
			root->getClock()->setIncludeSeconds(true);
		}
		else
		{
			tempProp.value = T("FALSE");
			root->getClock()->setIncludeSeconds(false);
		}
		prop->setProperty(T("ClockIncludeSeconds"), tempProp);
	}
	else if(button == use24HourClock)
	{
		tempProp.name = T("ClockUse24HourClock");
		if(button->getToggleState())
		{
			tempProp.value = T("TRUE");
			root->getClock()->setUse24HourClock(true);
		}
		else
		{
			tempProp.value = T("FALSE");
			root->getClock()->setUse24HourClock(false);
		}
		prop->setProperty(T("ClockUse24HourClock"), tempProp);
	}
	else if(button == monthDisplayedFirst)
	{
		tempProp.name = T("ClockMonthDisplayedFirst");
		if(button->getToggleState())
		{
			tempProp.value = T("TRUE");
			root->getClock()->setMonthDisplayedFirst(true);
		}
		else
		{
			tempProp.value = T("FALSE");
			root->getClock()->setMonthDisplayedFirst(false);
		}
		prop->setProperty(T("ClockMonthDisplayedFirst"), tempProp);
	}
	else if(button == localisationDialog)
	{
		FileChooser dlg(TRANS("Open localisation file..."),
						File::nonexistent,
						String::empty,
						false);

		if(dlg.browseForFileToOpen())
		{
			LocalisedStrings *tempStrings = new LocalisedStrings(dlg.getResult());
			localisationFile->setText(dlg.getResult().getFullPathName());
			LocalisedStrings::setCurrentMappings(tempStrings);
			saveChanges();

			cout << "  loc: Setting text to " << (const char *)dlg.getResult().getFullPathName() << endl;
			cout << "  loc: language = " << (const char *)tempStrings->getLanguageName() << endl;
			cout << "  loc: current language = " << (const char *)LocalisedStrings::getCurrentMappings()->getLanguageName() << endl;
		}
	}

	root->getClock()->repaint();
}

//------------------------------------------------------------------------------
void AudioPreferences::textEditorReturnKeyPressed(TextEditor &editor)
{
	if(editor.getName() == T("lF"))
	{
		if(localisationFile->getText() == T(""))
		{
			cout << "  loc: Resetting text." << endl;
			LocalisedStrings::setCurrentMappings(0);
		}
		else
		{
			LocalisedStrings *tempStrings = new LocalisedStrings(File(localisationFile->getText()));
			LocalisedStrings::setCurrentMappings(tempStrings);
			cout << "  loc: Setting text to" << (const char *)localisationFile->getText() << endl;
		}
	}
	saveChanges();
}

//------------------------------------------------------------------------------
void AudioPreferences::paintListBoxItem(int rowNumber,
									   Graphics &g,
									   int width,
									   int height,
									   bool rowIsSelected)
{
	if(rowIsSelected)
	{
		Colour tempCol = backgroundColour.contrasting();
		if(tempCol.brighter(0.5f) == Colours::white)
			tempCol = backgroundColour;
		g.setColour(tempCol.withAlpha(0.2f));
		g.fillRect(0, 0, width, 25);
	}

	Font *f = fonts[rowNumber];

	if(f != 0)
	{
		f->setHeight(height * 0.7f);
		f->setBold(false);
		f->setItalic(false);

		g.setFont(*f);
		g.setColour(Colours::black);

		g.drawText(f->getTypefaceName(), 
                   4,
				   0,
				   (width-4),
				   height,
				   Justification::centredLeft,
				   true);
	}
}

//------------------------------------------------------------------------------
void AudioPreferences::selectedRowsChanged(int lastRowselected)
{
	TwindyProperty tempProp;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();
	TwindyProperties *prop = root->getProperties();

	tempProp.name = fonts[lastRowselected]->getTypefaceName();
	tempProp.value = fonts[lastRowselected]->getTypefaceName();
	prop->setProperty(T("GlobalFont"), tempProp);
	//Font::setDefaultSansSerifFontName(tempProp.name);
	LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(tempProp.name);
	root->repaint();
}

//------------------------------------------------------------------------------
void AudioPreferences::paintOverChildren(Graphics &g)
{
	//Fonts.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(8, 23, 254, 284, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(9, 24, 252, 282, 1);

	//Clock group.
	g.setOrigin(270, 3);
	drawGroupComponentOutline(g,
							  200,
							  130,
							  TRANS("Clock Options"),
							  Justification::left);
	g.setOrigin(-270, -3);

	//Date separator.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(403, 98, 54, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(404, 99, 52, 22, 1);

	//Localisation File.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(223, 313, 204, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(224, 314, 202, 22, 1);
}

//------------------------------------------------------------------------------
void AudioPreferences::setColours(const Colour& backCol,
								 const Colour& textCol,
								 const Colour& buttonCol)
{
	backgroundColour = backCol;
	textColour = textCol;

	fontListLabel->setColour(Label::textColourId, textColour);
	includeSeconds->setColours(backgroundColour, textColour);
	use24HourClock->setColours(backgroundColour, textColour);
	monthDisplayedFirst->setColours(backgroundColour, textColour);
	dateSepLabel->setColour(Label::textColourId, textColour);
	localisationLabel->setColour(Label::textColourId, textColour);
}

//------------------------------------------------------------------------------
void AudioPreferences::saveChanges()
{
	String tempstr;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if (root)
	{
		TwindyProperties *prop = root->getProperties();

		TwindyProperty tempProp;
		tempProp.name = T("ClockMonthDateSeparator");
		tempProp.value = dateSeparator->getText();
		prop->setProperty(T("ClockMonthDateSeparator"), tempProp);
		root->getClock()->setDateSeparator(tempProp.value);
		root->getClock()->repaint();

		tempProp.name = T("LocalisationFile");
		tempProp.value = localisationFile->getText();
		prop->setProperty(T("LocalisationFile"), tempProp);
	}
}

//------------------------------------------------------------------------------
void AudioPreferences::drawGroupComponentOutline(Graphics& g,
                                                 int width, int height,
                                                 const String& text, const Justification& position)
{
    const float textH = 15.0f;
    const float indent = 3.0f;
    const float textEdgeGap = 4.0f;
    float cs = 5.0f;

    Font f (textH, Font::bold);

    Path p;
    float x = indent;
    float y = f.getAscent() - 3.0f;
    float w = width - x * 2.0f;
    float h = height - y  - indent;
    cs = jmin (cs, w * 0.5f, h * 0.5f);
    const float cs2 = 2.0f * cs;

    float textW = jlimit (0.0f, f.getStringWidth (text) + textEdgeGap * 2.0f, w - cs2);
    float textX = cs + textEdgeGap;

    if (position.testFlags (Justification::verticallyCentred))
        textX = cs + (w - cs2 - textW) * 0.5f;
    else if (position.testFlags (Justification::right))
        textX = w - cs - textW - textEdgeGap;

    p.startNewSubPath (x + textX + textW, y);
    p.lineTo (x + w - cs, y);

    p.addArc (x + w - cs2, y, cs2, cs2, 0, float_Pi * 0.5f);
    p.lineTo (x + w, y + h - cs);

    p.addArc (x + w - cs2, y + h - cs2, cs2, cs2, float_Pi * 0.5f, float_Pi);
    p.lineTo (x + cs, y + h);

    p.addArc (x, y + h - cs2, cs2, cs2, float_Pi, float_Pi * 1.5f);
    p.lineTo (x, y + cs);

    p.addArc (x, y, cs2, cs2, float_Pi * 1.5f, float_Pi * 2.0f);
    p.lineTo (x + textX, y);

    g.setColour (textColour);
    g.strokePath (p, PathStrokeType (2.0f));

    g.setColour (textColour);
    g.setFont (f);
    g.drawText (text,
                roundFloatToInt (x + textX), 0,
                roundFloatToInt (textW),
                roundFloatToInt (textH),
                Justification::centred, true);
}
#endif
