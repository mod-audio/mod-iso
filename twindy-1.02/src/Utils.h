/*
 * Utilities for Twindy
 * Copyright (C) 2015 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef FALKTX_TWINDY_UTILS_H_INCLUDED
#define FALKTX_TWINDY_UTILS_H_INCLUDED

#include "juce_amalgamated.h"
#include "TwindyErrorDisplay.h"

#include <sys/wait.h>
#include <unistd.h>

#define LIVEMOD_VERSION "2015.08-alpha1"

// -----------------------------------------------------------------------

static inline
double getSystemScalingImpl()
{
    if (const char* const QT_SCALE_FACTOR = getenv("QT_SCALE_FACTOR"))
    {
        const double scaling = atof(QT_SCALE_FACTOR);

        if (scaling >= 0.5 && scaling <= 10.0)
            return scaling;
    }

    return 1.0;
}

static inline
double getSystemScaling()
{
    static const double scaling = getSystemScalingImpl();
    return scaling;
}

struct Coordinates {
    int topPadding;
    int topTabBarHeight;
    int topTabBarIndent;
    int buttonMargin;
    int buttonWidth;
    int buttonHeight;
    int errorDisplayWidth;
    int errorDisplayHeight;
    int leftTabsWidth;
    float versionFontSize;

    Coordinates()
        : topPadding(30),
          topTabBarHeight(-1),
          topTabBarIndent(10),
          buttonMargin(10),
          buttonWidth(100),
          buttonHeight(25),
          errorDisplayWidth(200),
          errorDisplayHeight(60),
          leftTabsWidth(-1),
          versionFontSize(18.0f)
    {
        double scaling = getSystemScaling();

        if (scaling < 0.5 || scaling > 10.0)
        {
            scaling = 1.0;

            if (TwindyErrorDisplay* const errorDisplay = TwindyErrorDisplay::getInstance())
                errorDisplay->addErrorMessage(TRANS("Error"), TRANS("Invalid scaling requested."));
        }

        topPadding      = double(topPadding)      * scaling;
        topTabBarIndent = double(topTabBarIndent) * scaling;
        buttonMargin    = double(buttonMargin)    * scaling;
        buttonWidth     = double(buttonWidth)     * scaling;
        buttonHeight    = double(buttonHeight)    * scaling;
        versionFontSize = versionFontSize         * scaling;

        topTabBarHeight = topPadding;
        leftTabsWidth   = buttonMargin*2 + buttonWidth;
    }
};

static inline
const Coordinates& getCoordinates()
{
    static const Coordinates coord;
    return coord;
}

// -----------------------------------------------------------------------

static inline
String getExecutableAbsolutePath(const String& executable)
{
    // if the filename is absolute simply return it
    if (File::isAbsolutePath (executable))
        return executable;

    // if the filename is relative construct from CWD
    if (executable[0] == '.')
        return File::getCurrentWorkingDirectory().getChildFile (executable).getFullPathName();

    // filename is abstract, look up in PATH
    if (const char* const envpath = std::getenv("PATH"))
    {
        StringArray paths (StringArray::fromTokens (String(envpath), T(":"), T("")));

        for (int i=paths.size(); --i>=0;)
        {
            const File filepath (File (paths[i]).getChildFile (executable));

            if (filepath.existsAsFile())
                return filepath.getFullPathName();
        }
    }

    return String();
}

static inline
pid_t startProcess(const StringArray& args)
{
    const String executable(getExecutableAbsolutePath(args[0]));

    if (executable.isEmpty() || ! File(executable).existsAsFile())
    {
        String msg = TRANS("Requested external tool '%1' does not exist.");
        msg = msg.replace(T("%1"), args[0], false);
        TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"), msg);
        return -1;
    }

    const int    argc = args.size();
    const char* cargs[argc+1];

    for (int i=0; i<argc; ++i)
        cargs[i] = args[i].toUTF8();
    cargs[argc] = nullptr;

    const pid_t pid = vfork();

    switch (pid)
    {
    case 0:
        execvp(cargs[0], const_cast<char* const*>(cargs));
        _exit(1);
        return -1;

    case -1:
        TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"),
                                                           TRANS("Could not start external tool."));
        return -1;
    }

    return pid;
}

static inline
void terminateAndWaitForProcess(const pid_t pid)
{
    if (pid <= 0)
        return;

    bool sendTerm = true;

    for (pid_t p;;)
    {
        p = ::waitpid(pid, nullptr, WNOHANG);

        switch (p)
        {
        case 0:
            if (sendTerm)
            {
                sendTerm = false;
                ::kill(pid, SIGTERM);
            }
            break;

        case -1:
            if (errno == ECHILD)
                return;
            break;

        default:
            if (p == pid)
                return;
            break;
        }

        // 1sec
        usleep(1*1000*1000);
    }
}

// -----------------------------------------------------------------------

#endif // FALKTX_TWINDY_UTILS_H_INCLUDED
