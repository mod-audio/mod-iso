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
      pidJackd(-1),
      client(nullptr),
      jackWasStartedBeforeUs(false) {}

///-----------------------------------------------------------------------------
TwindyApp::~TwindyApp()
{
    jassert(win == nullptr);
    jassert(client == nullptr);
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

    //Font::setDefaultSansSerifFontName("Bitstream Vera Sans");

    TWINDY_DBG_MESSAGE("Creating TwindyRootWindow.");
    win = new TwindyRootWindow();
    TWINDY_DBG_MESSAGE("TwindyRootWindow created.");
    win->centreWithSize(recadendrum.getWidth(), recadendrum.getHeight());
    TWINDY_DBG_MESSAGE("TwindyRootWindow centred and sized.");
    win->toFront(true);
    TWINDY_DBG_MESSAGE("TwindyRootWindow brought to front.");

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

    if (client != nullptr)
    {
        jack_deactivate(client);
        jack_client_close(client);
        client = nullptr;
    }

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
bool TwindyApp::restartJackd(const StringArray& args)
{
    if (jackWasStartedBeforeUs)
        return false;

    printf("previous client => : %p %li\n", client, (long)pidJackd);

    if (client != nullptr)
    {
        jack_deactivate(client);
        jack_client_close(client);
        client = nullptr;
    }

    if (pidJackd > 0)
    {
        terminateAndWaitForProcess(pidJackd);
        pidJackd = -1;
    }

    if ((pidJackd = startProcess(args)) == -1)
        return false;

#if 0
    printf("jack_client_open calling...\n");
    client = jack_client_open("mod-twindy2", JackNoStartServer, nullptr);
    printf("jack_client_open done!\n");

    if (client == nullptr)
        return false;
#endif

    restartMODApp();
    return true;
}

///-----------------------------------------------------------------------------
void TwindyApp::restartMODApp()
{
    if (pidApp > 0)
    {
        terminateAndWaitForProcess(pidApp);
        pidApp = -1;
    }

#define APP_PREFIX "/Shared/Personal/FOSS/GIT/MOD/mod-app/source/"
// #define APP_PREFIX
    StringArray args;
    args.add(T(APP_PREFIX "mod-app"));

    //if (client == nullptr)
    //    args.add(T("--no-autostart"));
#undef APP_PREFIX

    pidApp = startProcess(args);

    win->changeToAppTab();
}

///-----------------------------------------------------------------------------
START_JUCE_APPLICATION(TwindyApp)
