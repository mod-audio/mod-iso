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
#include "TwindyErrorDisplay.h"
#include "TwindyHelperStuff.h"
#include "TwindyRootWindow.h"

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

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

    if (client != nullptr)
    {
        jack_deactivate(client);
        jack_client_close(client);
        client = nullptr;
    }

    if (pidApp > 0)
        ::kill(pidApp, SIGTERM);

    if (pidJackd > 0)
        ::kill(pidJackd, SIGTERM);
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

    TWINDY_DBG_MESSAGE("Initial JACK start test.");

    printf("testing jack...\n");
    client = jack_client_open("mod-twindy", JackNoStartServer, nullptr);
    printf("done!\n");

    if (client != nullptr)
    {
        jack_client_close(client);
        jackWasStartedBeforeUs = true;
    }

    TWINDY_DBG_MESSAGE("Auto-start MOD-APP.");
    restartMODApp();
}

///-----------------------------------------------------------------------------
void TwindyApp::shutdown()
{
    TWINDY_DBG_MESSAGE("About to delete TwindyRootWindow.");
    delete win;
    win = nullptr;
    TWINDY_DBG_MESSAGE("TwindyRootWindow deleted.");
}

///-----------------------------------------------------------------------------
bool TwindyApp::restartJackd(const String& command)
{
    if (jackWasStartedBeforeUs)
        return false;

    if (client != nullptr)
    {
        jack_deactivate(client);
        jack_client_close(client);
        client = nullptr;
    }

#if 0
    if (pidJackd > 0)
    {
        ::kill(pidJackd, SIGTERM);

        // FIXME
        //::waitpid(pidJackd, nullptr, 0x0);
        sleep(1);
    }

    pidJackd = vfork();

    switch (pidJackd)
    {
    case 0: //Child process - successful.
        execlp("/bin/sh", "sh", "-c", command.toUTF8(), NULL);
        exit(1);
        break;
    case -1: //Parent process - unsuccessful.
        TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"),
                                                           TRANS("Could not start audio service."));
        return;
    }

    sleep(1);
#else
    //const File jackdFile = File::getSpecialLocation(File::userHomeDirectory).getChildFile(".jackdrc");
    //jackdFile.replaceWithText(command);
#endif
    printf("starting jackd using this:\n%s\n", command.toUTF8());

    client = jack_client_open("mod-twindy", JackNoStartServer, nullptr);

    printf("jack_client_open done!\n");

    if (client == nullptr)
        return false;

    restartMODApp();
    return true;
}

///-----------------------------------------------------------------------------
void TwindyApp::restartMODApp()
{
    if (pidApp > 0)
    {
        ::kill(pidApp, SIGTERM);
        // FIXME
        //::waitpid(pidApp, nullptr, 0x0);
        sleep(1);
    }

#define APP_PREFIX "/Shared/Personal/FOSS/GIT/MOD/mod-app/source/"
    const char* const appWithJack    = APP_PREFIX "mod-app";
    const char* const appWithoutJack = APP_PREFIX "mod-app --no-autostart";
#undef APP_PREFIX

    pidApp = vfork();

    switch (pidApp)
    {
    case 0: //Child process - successful.
        execlp("/bin/sh", "sh", "-c", client ? appWithJack : appWithoutJack, NULL);
        exit(1);
        break;
    case -1: //Parent process - unsuccessful.
        TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"),
                                                           TRANS("Could not start mod-app."));
        break;
    }
}

///-----------------------------------------------------------------------------
START_JUCE_APPLICATION(TwindyApp)
