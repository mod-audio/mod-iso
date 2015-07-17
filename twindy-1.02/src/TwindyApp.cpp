// TwindyApp.cpp - Main application stuff.
// ----------------------------------------------------------------------------
// This file is part of the Tracktion-themed Twindy window manager.
// Copyright (c) 2005-2007 Niall Moody.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// ----------------------------------------------------------------------------

#include "TwindyApp.h"
#include "TwindyHelperStuff.h"
#include "TwindyPreferences.h"
#include "TwindyRootWindow.h"
#include "Utils.h"

#include <iostream>

using namespace std;

///-----------------------------------------------------------------------------
TwindyApp::TwindyApp()
    : win(nullptr),
      pidApp(-1),
      pidJackd(-1) {}

///-----------------------------------------------------------------------------
TwindyApp::~TwindyApp()
{
    jassert(win == nullptr);
    jassert(pidApp == -1);
    jassert(pidJackd == -1);
}

///-----------------------------------------------------------------------------
void TwindyApp::initialise(const String& commandLine)
{
    Rectangle recadendrum = Desktop::getInstance().getMainMonitorArea();

    if (commandLine.contains(T("--help")))
    {
        cout << "Twindy Window Manager" << endl;
        cout << "Version: " << ProjectInfo::versionString << endl;
        cout << endl;
        cout << "Usage:" << endl;
        cout << "twindy --version --help" << endl;
        cout << endl;
        quit();
    }
    else if(commandLine.contains(T("--version")))
    {
        cout << "Twindy Window Manager" << endl;
        cout << "Version: " << ProjectInfo::versionString << endl;
        cout << endl;
        quit();
    }

    TWINDY_DBG_MESSAGE("Creating TwindyRootWindow.");
    win = new TwindyRootWindow();
    TWINDY_DBG_MESSAGE("TwindyRootWindow created.");
    win->centreWithSize(recadendrum.getWidth(), recadendrum.getHeight());
    TWINDY_DBG_MESSAGE("TwindyRootWindow centred and sized.");
    win->toFront(true);
    TWINDY_DBG_MESSAGE("TwindyRootWindow brought to front.");

    // Initial values
    ::setenv("MOD_INGEN_NUM_AUDIO_INS", "2", 1);
    ::setenv("MOD_INGEN_NUM_AUDIO_OUTS","2", 1);
    ::setenv("MOD_INGEN_NUM_MIDI_INS",  "0", 1);
    ::setenv("MOD_INGEN_NUM_MIDI_OUTS", "0", 1);
    ::setenv("MOD_INGEN_NUM_CV_INS",    "0", 1);
    ::setenv("MOD_INGEN_NUM_CV_OUTS",   "0", 1);

    if (std::getenv("TWINDY_LOCAL_TEST") != nullptr)
        return restartMODApp(1);

    AlertWindow w(T("Live-MOD"), T("Welcome to Live-MOD!"), AlertWindow::NoIcon);
    w.addTextBlock(T("Before we begin please select which soundcard you plan to use."));
    w.addTextBlock(T("If you're not sure which to use, select the first one."));
    w.addComboBox(T("deviceList"), win->getPreferencesPanel()->getAudioDevices(), T("Soundcard:"));
    w.addButton(T("Ok"), 5, '\n');

    if (w.runModalLoop() == 5)
    {
        if (ComboBox* const box = w.getComboBoxComponent(T("deviceList")))
            win->getPreferencesPanel()->selectAudioDevice(box->getText());
    }
}

///-----------------------------------------------------------------------------
void TwindyApp::shutdown()
{
    TWINDY_DBG_MESSAGE("About to shutdown.");

    if (pidApp > 0)
    {
        terminateAndWaitForProcess(pidApp);
        pidApp = -1;
    }

    if (pidJackd > 0)
    {
        terminateAndWaitForProcess(pidJackd);
        pidJackd = -1;
    }

    TWINDY_DBG_MESSAGE("About to delete TwindyRootWindow.");
    delete win;
    win = nullptr;
    TWINDY_DBG_MESSAGE("TwindyRootWindow deleted.");
}

///-----------------------------------------------------------------------------
void TwindyApp::restartMODApp(int gitversion)
{
    if (pidApp > 0)
    {
        terminateAndWaitForProcess(pidApp);
        pidApp = -1;
    }

    if (gitversion == -1) {
        gitversion = (std::getenv("TWINDY_LOCAL_TEST") != nullptr) ? 1 : 0;
    }
    gitversion = 1;

    StringArray args;
    args.add(gitversion ? T("mod-app-git") : T("mod-app"));
    args.add(T("--using-live-iso"));

    pidApp = startProcess(args);

    win->changeToAppTab();
}

///-----------------------------------------------------------------------------
bool TwindyApp::restartJackd(const StringArray& args)
{
    if (pidJackd > 0)
    {
        terminateAndWaitForProcess(pidJackd);
        pidJackd = -1;
    }

    if ((pidJackd = startProcess(args)) == -1)
        return false;

    restartMODApp(-1);
    return true;
}

///-----------------------------------------------------------------------------
START_JUCE_APPLICATION(TwindyApp)
