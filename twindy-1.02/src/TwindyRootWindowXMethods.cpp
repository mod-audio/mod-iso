//	TwindyRootWindowXMethods.cpp - Separating the XWindows stuff from the JUCE
//								   stuff to try and avoid dependency problems.
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

#include "TwindyHelperStuff.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <sys/wait.h>
#include <unistd.h>

//Not sure if I need all these headers, but I figure it's better safe than sorry...
/*#include "juce_Config.h"

#include "src/juce_core/basics/juce_StandardHeader.h"*/
//#include "juce_StreamlinedHeaders.h"
#undef KeyPress
#include "juce_amalgamated.h"

void killZombies(int)
{
    TWINDY_DBG_MESSAGE("Killing zombie...");
    wait(nullptr);
}

BEGIN_JUCE_NAMESPACE
extern Display* display;
END_JUCE_NAMESPACE

#include "TwindyRootWindow.h"
#include "TwindyUpperPanel.h"
#include "TwindyPreferences.h"
#include "TwindyWindow.h"
#include "TwindyErrorDisplay.h"
#include "TwindyHelperStuff.h"
#include <iostream>
#include <sstream>

static int xErrorHandler(Display* const dpy, XErrorEvent* const event)
{
    const ::Window rootWindow = RootWindow(dpy, DefaultScreen(dpy));

    // I can't help feeling I should be doing something more than this here, but
    // I don't know what I could do - this is just to avoid gedit crashing Twindy
    // when you close it from it's file menu, and it doesn't seem to do any harm
    // if we just ignore the error...
    if (event->error_code == BadWindow)
    {
        TWINDY_DBG_MESSAGE("Got a BadWindow error from somewhere...  Hmm...");
    }
    else if(event->error_code == BadAccess && event->resourceid == rootWindow)
    {
        std::cout << "BadAccess Error: Another window manager is probably running. Quitting." << std::endl;
        exit(1);
    }
    else
    {
        std::string tempstr;
        tempstr = "X Error: ";
        tempstr += event->error_code;
        TWINDY_DBG_MESSAGE(tempstr);
    }

    return 0;
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::callbackFunction(void *event)
{
    XEvent* const evt = static_cast<XEvent*>(event);

    printf("received %i.\n", evt->xany.type);

    switch (evt->xany.type)
    {
    case MapRequest:
    {
        TWINDY_DBG_MESSAGE("MapRequest received.");

        String name;
        int target;
        XTextProperty textProp;
        const ::Window window = evt->xmaprequest.window;

        //We add all upper windows to the save set, because we might quit
        //unexpectedly while all but the currently mapped window are
        //hidden (unmapped).  This ensures these windows will be mapped if
        //we quit unexpectedly.
        XAddToSaveSet(display, window);

        if (XGetWMName(display, window, &textProp))
        {
            name = (const char*)(textProp.value);
            XFree(textProp.value); //?
        }

        if (name == T("Konsole") || name == T("Shell") || name == T("alsamixer"))
        {
            target = -1;
            printf("Mixer detected\n");
        }
        else if (name.isEmpty())
        {
            target = 0;
            printf("MOD-App detected\n");
        }
        else
        {
            target = upperPanelComps.size()-1;
            printf("name is %s\n", name.toUTF8());
        }

        XSetWindowBorderWidth(display, window, 0);

        switch (target)
        {
        case -1:
            XMoveWindow(display, window, 162, 250);
            XResizeWindow(display, window, getWidth()-162-12, getHeight()-250-12);
            break;
        case 0:
            XMoveWindow(display, window, 0, 26);
            XResizeWindow(display, window, getWidth(), getHeight()-26);
            break;
        default:
            XMoveWindow(display, window, 162, 37);
            XResizeWindow(display, window, getWidth()-162-12, getHeight()-37-12);
            break;
        }

        TwindyWindow* const newWin = new TwindyWindow(display, window);

        if (target >= 0)
        {
            TwindyUpperPanel* const panel(upperPanelComps[target]);
            TwindyWindow*     const twnd(panel->getCurrentWindow());

            if (target == 0)
            {
                if (twnd != nullptr)
                {
                    //twnd->closeWindow();
                    panel->removeWindow(twnd->getWindow());
                    //delete twnd;
                }
            }
            else if (panel->isWorkspaceVisible())
            {
                if (twnd != nullptr)
                    twnd->hide();
            }

            XMapWindow(display, window);

            if (target == currentUpperPanelComp)
                XSetInputFocus(display, window, RevertToPointerRoot, CurrentTime);

            //Make sure we get notified when the window's title changes.
            XSelectInput(display, window, PropertyChangeMask);

            TwindyUpperTab* const newTab = new TwindyUpperTab(T("Unnamed"),
                                                              colours.propertyPanelText,
                                                              colours.propertyPanelBackground);
            if (name.isNotEmpty())
                newTab->setName(name);

            upperPanelComps[target]->addWindow(newWin, newTab);
        }
        else
        {
            preferences->setMixerWindow(newWin);
        }

        break;
    }
    case ConfigureRequest:
    {
        TWINDY_DBG_MESSAGE("ConfigureRequest received.");

        // FIXME

        XWindowChanges windowChanges;

        switch (currentUpperPanelComp)
        {
        case -1:
            windowChanges.x = 162;
            windowChanges.y = 250;
            windowChanges.width = getWidth()-162-12;
            windowChanges.height = getHeight()-250-12;
            break;
        case 0:
            windowChanges.x = 0;
            windowChanges.y = 26;
            windowChanges.width = getWidth();
            windowChanges.height = getHeight()-26;
            break;
        default:
            windowChanges.x = 162;
            windowChanges.y = 37;
            windowChanges.width = getWidth()-162-12;
            windowChanges.height = getHeight()-37-12;
            break;
        }

        windowChanges.sibling = evt->xconfigurerequest.above;
        windowChanges.stack_mode = evt->xconfigurerequest.detail;

        XConfigureWindow(display, evt->xconfigurerequest.window, evt->xconfigurerequest.value_mask, &windowChanges);
        break;
    }
    case UnmapNotify:
    {
        TWINDY_DBG_MESSAGE("UnmapNotify received.");
        //I don't think we need to handle this...
        //removeWindow(evt->xdestroywindow.window);
        break;
    }
    case DestroyNotify:
    {
        TWINDY_DBG_MESSAGE("DestroyNotify received.");

        removeWindow(evt->xdestroywindow.window);
        break;
    }
    case EnterNotify:
    {
        TWINDY_DBG_MESSAGE("EnterNotify received.");
#if 1
        break;
    }
    case LeaveNotify:
    {
        TWINDY_DBG_MESSAGE("LeaveNotify received.");
#endif

        if (isCurrentlyBlockedByAnotherModalComponent())
            return;

        Rectangle temprect;

        switch (currentUpperPanelComp)
        {
        case -1:
            temprect.setBounds(162, 250, getWidth()-162-12, getHeight()-250-12);
            break;
        case 0:
            temprect.setBounds(0, 26, getWidth(), getHeight()-26);
            break;
        default:
            temprect.setBounds(162, 37, getWidth()-162-12, getHeight()-37-12);
            break;
        }

        if (temprect.contains(evt->xcrossing.x_root, evt->xcrossing.y_root))
        {
            TwindyWindow* window;

            if (currentUpperPanelComp == -1)
                window = preferences->getMixerWindow();
            else
                window = upperPanelComps[currentUpperPanelComp]->getCurrentWindow();

            if (window != nullptr)
                XSetInputFocus(display, window->getWindow(), RevertToPointerRoot, CurrentTime);
        }
        break;
    }
    case PropertyNotify:
    {
        TWINDY_DBG_MESSAGE("PropertyNotify received.");

        if (evt->xproperty.atom == XA_WM_NAME)
            redrawWindowName(evt->xproperty.window);
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------
Window TwindyRootWindow::getActualWindow()
{
    return reinterpret_cast<Window>(static_cast<ComponentPeer *>(getPeer())->getNativeHandle());
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::setupMappingRedirect()
{
    XSetWindowAttributes att;

    // Here we just add SubstructureRedirectMask (&...) in order to get notified
    // when new windows open - the rest are copied from juce_linux_windowing.cpp.
    // We need them in order to get all the key, mouse etc. events as normal.
    att.event_mask = SubstructureRedirectMask // To be told when a program wants to be mapped.
                   | SubstructureNotifyMask   // To be told when a program has quit.
                   | EnterWindowMask          // To be told when the cursor moves over a window.
                   | LeaveWindowMask          // To be told when the cursor leaves a window.
                   | KeyPressMask    | KeyReleaseMask
                   | ButtonPressMask | ButtonReleaseMask
                   | KeymapStateMask | PointerMotionMask | PointerMotionHintMask
                   | ExposureMask    | FocusChangeMask | StructureNotifyMask;

    // To set the cursor to the usual pointer, rather than that horrible default X...
    att.cursor = XCreateFontCursor(display, XC_left_ptr);

    if (XChangeWindowAttributes(display, RootWindow(display, DefaultScreen(display)),
                                         CWEventMask|CWCursor, &att) == BadAccess)
    {
        //Not sure this part is correct - we quit the program in xErrorHandler.
        TWINDY_DBG_MESSAGE("BadAccess error for XChangeWindowAttributes()");
        TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"),
            TRANS("Failed: BadAccess error (another window's already redirecting the relevant events)."));
        exit(1);
    }

    // Set up a handler so that zombie processes get killed.
    sigset_t maskset;
    sigemptyset(&maskset);

    struct sigaction act;
    act.sa_handler  = killZombies;
    act.sa_restorer = nullptr;
    act.sa_mask  = maskset;
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, NULL);

    // Set up an X error handler.
    XSetErrorHandler(xErrorHandler);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::launchExecutable(const char *cmd, bool storePid)
{
    String instr(cmd);
    String tempstr(cmd);

    if (instr.isEmpty())
    {
        AlertWindow dlg(TRANS("Run Executable"), TRANS("Choose an executable to run."), AlertWindow::QuestionIcon);

        dlg.addTextEditor(T("dlgTE"), T(""), TRANS("executable:"), false);
        dlg.addButton(TRANS("cancel"), 0);
        dlg.addButton(TRANS("ok"), 1, KeyPress(KeyPress::returnKey, ModifierKeys(), '\r'));

        if (! dlg.runModalLoop())
            return;

        tempstr = dlg.getTextEditorContents(T("dlgTE"));
    }

    printf("Starting %s...\n", (const char *)tempstr);

    pid_t pid = vfork();

    switch (pid)
    {
    case 0: //Child process - successful.
        //setsid();
        execlp("/bin/sh", "sh", "-c", (const char *)tempstr, NULL);
        exit(1);
        break;
    case -1: //Parent process - unsuccessful.
        TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"), TRANS("Could not start executable."));
        break;
    default: //Parent process - successful.
        if (storePid)
        {
            std::cout << "Storing pid " << pid << std::endl;
            pidArray.add(pid);
        }
        break;
    }
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::removeWindow(Window win)
{
    //upperPanel->removeWindow(win);
    if (TwindyWindow* const mixWindow = preferences->getMixerWindow())
        if (mixWindow->getWindow() == win)
            preferences->setMixerWindow(nullptr);

    for (int i=upperPanelComps.size(); --i>=0;)
    {
        if (TwindyUpperPanel* const panel = upperPanelComps[i])
            panel->removeWindow(win);
    }
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::giveWindowFocus(TwindyWindow*)
{
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::redrawWindowName(Window win)
{
    if (TwindyUpperPanel* const panel = upperPanelComps[1])
    {
        for (int j=0, numWindows=panel->getNumWindows(); j<numWindows; ++j)
        {
            if (panel->getWindow(j)->getWindow() != win)
                continue;

            XTextProperty textProp;

            if (XGetWMName(display, win, &textProp))
            {
                const String tempstr = (const char*)(textProp.value);
                TwindyUpperTab* const tempTab = static_cast<TwindyUpperTab*>(panel->getTabComponent(j));
                tempTab->setName(tempstr);
                repaint();

                XFree(textProp.value); //?
            }
        }
    }
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::raiseAllWindows()
{
    if (currentUpperPanelComp >= 0 && upperPanelComps[currentUpperPanelComp]->getNumWindows() > 0)
        XRaiseWindow(display, upperPanelComps[currentUpperPanelComp]->getCurrentWindow()->getWindow());

    // We get crashes if Twindy loses focus after a PopupMenu has been shown.
    // This should fix that.
    XSetInputFocus(display, RootWindow(display, DefaultScreen(display)), RevertToPointerRoot, CurrentTime);
}
