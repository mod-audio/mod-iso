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

void killZombies(int thing)
{
	TWINDY_DBG_MESSAGE("Killing zombie...");
	wait(NULL);
}

BEGIN_JUCE_NAMESPACE
extern Display* display;
END_JUCE_NAMESPACE

#include "TwindyRootWindow.h"
#include "TwindyUpperPanel.h"
#include "TwindyWindow.h"
#include "TwindyErrorDisplay.h"
#include "TwindyHelperStuff.h"
#include <iostream>
#include <sstream>

int xErrorHandler(Display *dpy, XErrorEvent *event)
{
	Window rootWindow;

	rootWindow = RootWindow(dpy, DefaultScreen(dpy));

	//I can't help feeling I should be doing something more than this here, but
	//I don't know what I could do - this is just to avoid gedit crashing Twindy
	//when you close it from it's file menu, and it doesn't seem to do any harm
	//if we just ignore the error...
	if(event->error_code == BadWindow)
		TWINDY_DBG_MESSAGE("Got a BadWindow error from somewhere...  Hmm...");
	else if((event->error_code == BadAccess) &&
			(event->resourceid == rootWindow))
	{
		std::cout << "BadAccess Error: Another window manager is probably";
		std::cout << " running.  Quitting." << std::endl;
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

    switch (evt->xany.type)
    {
    case MapRequest:
    {
        TWINDY_DBG_MESSAGE("MapRequest received.");

        XTextProperty textProp;

        int workspaceIndex = currentUpperPanelComp;
        if (workspaceIndex < 0)
            workspaceIndex = 1;

        if (upperPanelComps[workspaceIndex]->getCurrentWindow() && upperPanelComps[workspaceIndex]->isWorkspaceVisible())
            upperPanelComps[workspaceIndex]->getCurrentWindow()->hide();

        TwindyUpperTab* const newTab = new TwindyUpperTab(T("Browser"),
                                                          colours.propertyPanelText,
                                                          colours.propertyPanelBackground);

        //We add all upper windows to the save set, because we might quit
        //unexpectedly while all but the currently mapped window are
        //hidden (unmapped).  This ensures these windows will be mapped if
        //we quit unexpectedly.
        XAddToSaveSet(display, evt->xmaprequest.window);

        XSetWindowBorderWidth(display, evt->xmaprequest.window, 0);

        if (workspaceIndex == 0)
        {
            XMoveWindow(display, evt->xmaprequest.window, 0, 26);
            XResizeWindow(display, evt->xmaprequest.window, getWidth(), getHeight()-26);
        }
        else
        {
            XMoveWindow(display, evt->xmaprequest.window, 162, 37);
            XResizeWindow(display, evt->xmaprequest.window, getWidth()-162-13, getHeight()-37-13);
        }

        XMapWindow(display, evt->xmaprequest.window);
        XSetInputFocus(display, evt->xmaprequest.window, RevertToPointerRoot, CurrentTime);

        //Make sure we get notified when the window's title changes.
        XSelectInput(display, evt->xmaprequest.window, PropertyChangeMask);

        if (XGetWMName(display, evt->xmaprequest.window, &textProp))
        {
            newTab->setName((const char*)(textProp.value));
            XFree(textProp.value); //?
        }

        TwindyWindow* const newWin = new TwindyWindow(display, evt->xmaprequest.window);
        upperPanelComps[workspaceIndex]->addWindow(newWin, newTab);
        break;
    }
    case ConfigureRequest:
    {
        TWINDY_DBG_MESSAGE("ConfigureRequest received.");

        // FIXME

        XWindowChanges windowChanges;

        if (currentUpperPanelComp == 0)
        {
            windowChanges.x = 0;
            windowChanges.y = 26;
            windowChanges.width = getWidth();
            windowChanges.height = getHeight()-26;
        }
        else
        {
            windowChanges.x = 162;
            windowChanges.y = 37;
            windowChanges.width = getWidth()-162-13;
            windowChanges.height = getHeight()-37-13;
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
#if 0
        break;
    }
    case LeaveNotify:
    {
        TWINDY_DBG_MESSAGE("LeaveNotify received.");
#endif

        if (currentUpperPanelComp < 0 || isCurrentlyBlockedByAnotherModalComponent())
            return;

        Rectangle temprect;

        if (currentUpperPanelComp == 0)
            temprect.setBounds(0, 26, getWidth(), getHeight()-26);
        else
            temprect.setBounds(162, 37, getWidth()-162-13, getHeight()-37-13);

        if (temprect.contains(evt->xcrossing.x_root, evt->xcrossing.y_root))
        {
            if (TwindyWindow* const window = upperPanelComps[currentUpperPanelComp]->getCurrentWindow())
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
	int test;
	struct sigaction act;
    sigset_t maskSet;
	XSetWindowAttributes att;
	//To set the cursor to the usual pointer, rather than that horrible default X...
	Cursor cursor = XCreateFontCursor(display, XC_left_ptr);

	//Here we just add SubstructureRedirectMask (&...) in order to get notified
	//when new windows open - the rest are copied from juce_linux_windowing.cpp.
	//We need them in order to get all the key, mouse etc. events as normal.
	att.event_mask = SubstructureRedirectMask | //To be told when a program wants to be mapped.
					 SubstructureNotifyMask | //To be told when a program has quit.
					 EnterWindowMask | //To be told when the cursor moves over a window.
					 LeaveWindowMask | //To be told when the cursor leaves a window.
					 //StructureNotifyMask | //Necessary?
					 KeyPressMask | KeyReleaseMask |
					 ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
					 LeaveWindowMask | PointerMotionMask |KeymapStateMask |
     				 ExposureMask | StructureNotifyMask | FocusChangeMask;
	att.cursor = cursor;
	test = XChangeWindowAttributes(display,
								   RootWindow(display,
											  DefaultScreen(display)),
								   CWEventMask|CWCursor,
								   &att);
	if(test == BadAccess)
	{
		//Not sure this part is correct - we quit the program in xErrorHandler.
		TWINDY_DBG_MESSAGE("BadAccess error for XChangeWindowAttributes()");
		TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"),
														   TRANS("Failed: BadAccess error (another window's already redirecting the relevant events)."));
		exit(1);
	}

	//Set up a handler so that zombie processes get killed.
	sigemptyset(&maskSet);
	act.sa_handler = killZombies;
	act.sa_mask = maskSet;
	act.sa_flags = 0;
	sigaction(SIGCHLD, &act, NULL);

	//Set up an X error handler.
	XSetErrorHandler(xErrorHandler);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::launchExecutable(const char *cmd, bool storePid)
{
	String instr(cmd);
	String tempstr;

	if(instr == String::empty)
	{
		AlertWindow dlg(TRANS("Run Executable"),
										TRANS("Choose an executable to run."),
										AlertWindow::QuestionIcon);

		dlg.addTextEditor(T("dlgTE"),
						  T(""),
						  TRANS("executable:"),
						  false);
		dlg.addButton(TRANS("cancel"), 0);
		dlg.addButton(TRANS("ok"),
					  1,
					  KeyPress(KeyPress::returnKey,
											   ModifierKeys(),
											   '\r'));

		if(dlg.runModalLoop())
			tempstr = dlg.getTextEditorContents(T("dlgTE"));
		else
			return;
	}
	else
		tempstr = cmd;

	pid_t pid = fork();

	switch (pid)
	{
		case 0: //Child process - successful.
			setsid();
			printf("Starting %s...\n", (const char *)tempstr);
			execlp("/bin/sh", "sh", "-c", (const char *)tempstr, NULL);
			//Only get to this point if execlp didn't work.
			TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"),
														 	   TRANS("Could not start executable.  Cleaning up."));
			exit(1);
			break;
		case -1: //Parent process - unsuccessful.
			TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Error"),
														 	   TRANS("Could not start executable."));
			break;
		default: //Parent process - successful.
			if(storePid)
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
    for(int i=0;i<upperPanelComps.size();++i)
            upperPanelComps[i]->removeWindow(win);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::giveWindowFocus(TwindyWindow *win)
{
	
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::redrawWindowName(Window win)
{
	int i, j;
	String tempstr;
	XTextProperty textProp;
	TwindyUpperTab *tempTab;

	//Update the upper window's text if necessary.
	for(i=0;i<upperPanelComps.size();++i)
	{
		for(j=0;j<upperPanelComps[i]->getNumWindows();++j)
		{
			if(upperPanelComps[i]->getWindow(j)->getWindow() == win)
			{
				if(XGetWMName(display, win, &textProp))
				{
					tempstr = (const char *)(textProp.value);
					tempTab = static_cast<TwindyUpperTab *>(upperPanelComps[i]->getTabComponent(j));
					tempTab->setName(tempstr);
					repaint();
					free(textProp.value); //?
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::raiseAllWindows()
{
	ComponentPeer *twindy;

	if(currentUpperPanelComp > -1)
	{
		if(upperPanelComps[currentUpperPanelComp]->getNumWindows() > 0)
		{
			XRaiseWindow(display,
						 upperPanelComps[currentUpperPanelComp]->getCurrentWindow()->getWindow());
		}
	}

	//We get crashes if Twindy loses focus after a PopupMenu has been shown.
	//This should fix that.
	twindy = static_cast<ComponentPeer *>(getPeer());
	XSetInputFocus(display,
				   RootWindow(display, DefaultScreen(display)),
				   RevertToPointerRoot,
				   CurrentTime);
}

//----------------------------------------------------------------------------------------------
/*Window WindowHandle::getHandle() const
{
	return handle;
}*/
