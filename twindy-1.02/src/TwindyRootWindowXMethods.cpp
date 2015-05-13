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

/*namespace JUCE_NAMESPACE {
#include "src/juce_appframework/events/juce_Timer.h"
#include "src/juce_appframework/application/juce_DeletedAtShutdown.h"
#undef KeyPress
#include "src/juce_appframework/gui/components/keyboard/juce_KeyPress.h"
#include "src/juce_appframework/application/juce_SystemClipboard.h"
#include "src/juce_appframework/gui/components/windows/juce_AlertWindow.h"
#include "src/juce_appframework/gui/components/special/juce_OpenGLComponent.h"
#include "src/juce_appframework/gui/components/juce_Desktop.h"
#include "src/juce_appframework/events/juce_MessageManager.h"
#include "src/juce_appframework/gui/components/windows/juce_ComponentPeer.h"
#include "src/juce_appframework/gui/graphics/geometry/juce_RectangleList.h"
#include "src/juce_appframework/gui/graphics/imaging/juce_ImageFileFormat.h"
#include "src/juce_core/basics/juce_Logger.h"
#include "src/juce_core/threads/juce_Process.h"
#include "src/juce_core/misc/juce_PlatformUtilities.h"
#include "src/juce_appframework/gui/components/layout/juce_TabbedComponent.h"
#include "src/juce_core/containers/juce_OwnedArray.h"
#include "src/juce_appframework/gui/components/mouse/juce_MouseEvent.h"
#include "src/juce_appframework/gui/components/buttons/juce_ShapeButton.h"
#include "src/juce_appframework/gui/components/buttons/juce_DrawableButton.h"
#include "src/juce_core/text/juce_LocalisedStrings.h"

extern Display* display;
}*/

BEGIN_JUCE_NAMESPACE
extern Display* display;
END_JUCE_NAMESPACE

#include "TwindyRootWindow.h"
#include "TwindyLowerPanel.h"
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
	TwindyUpperTab *newTab;
	TwindyWindow *newWin;
	XEvent *evt = static_cast<XEvent *>(event);
	XWindowChanges windowChanges;
	XTextProperty textProp;
	JUCE_NAMESPACE::String tempstr;
	JUCE_NAMESPACE::Rectangle temprect;
	Window tempWin;
	int result;

	if(evt->xany.type == MapRequest)
	{
		TWINDY_DBG_MESSAGE("MapRequest received.");
		if(destinedForLowerPanel)
		{
			XSetWindowBorderWidth(JUCE_NAMESPACE::display,
								  evt->xmaprequest.window,
								  0);
			XMoveWindow(JUCE_NAMESPACE::display,
						evt->xmaprequest.window,
						lowerPanel->getWindowX(),
						lowerPanel->getWindowY());
			XResizeWindow(JUCE_NAMESPACE::display,
						  evt->xmaprequest.window,
						  lowerPanel->getWindowWidth(),
						  lowerPanel->getWindowHeight());
			XMapWindow(JUCE_NAMESPACE::display, evt->xmaprequest.window);
			//Don't need to use save sets, because we don't reparent any windows.
			//XAddToSaveSet(JUCE_NAMESPACE::display, evt->xmaprequest.window);
			XSetInputFocus(JUCE_NAMESPACE::display,
						   evt->xmaprequest.window,
						   RevertToPointerRoot,
						   CurrentTime);
			//Make sure we get notified when the window's title changes.
			XSelectInput(JUCE_NAMESPACE::display,
						 evt->xmaprequest.window,
						 PropertyChangeMask);

			if(lowerWindow)
				delete lowerWindow;
			lowerWindow = new TwindyWindow(JUCE_NAMESPACE::display, evt->xmaprequest.window);

			if(XGetWMName(JUCE_NAMESPACE::display, evt->xmaprequest.window, &textProp))
			{
				tempstr = (const char *)(textProp.value);
				lowerPanel->setText(tempstr);
				repaint();
				free(textProp.value); //?
			}

			destinedForLowerPanel = false;
		}
		else
		{
			int workspaceIndex = currentUpperPanelComp;
			if(workspaceIndex < 0)
				workspaceIndex = 0;

			if((upperPanelComps[workspaceIndex]->getCurrentWindow())&&
			   (upperPanelComps[workspaceIndex]->isWorkspaceVisible()))
				upperPanelComps[workspaceIndex]->getCurrentWindow()->hide();

			newTab = new TwindyUpperTab(T("Browser"),
										colours.propertyPanelText,
										colours.propertyPanelBackground);

			//We add all upper windows to the save set, because we might quit
			//unexpectedly while all but the currently mapped window are
			//hidden (unmapped).  This ensures these windows will be mapped if
			//we quit unexpectedly.
			XAddToSaveSet(JUCE_NAMESPACE::display, evt->xmaprequest.window);

			XSetWindowBorderWidth(JUCE_NAMESPACE::display, evt->xmaprequest.window, 0);
			XMoveWindow(JUCE_NAMESPACE::display,
						evt->xmaprequest.window,
						170,
						39);
			XResizeWindow(JUCE_NAMESPACE::display,
						  evt->xmaprequest.window,
						  (getWidth()-190),
						  ((getHeight()-(getHeight()/3))-28));
			XMapWindow(JUCE_NAMESPACE::display, evt->xmaprequest.window);
			XSetInputFocus(JUCE_NAMESPACE::display,
						   evt->xmaprequest.window,
						   RevertToPointerRoot,
						   CurrentTime);
			//Make sure we get notified when the window's title changes.
			XSelectInput(JUCE_NAMESPACE::display,
						 evt->xmaprequest.window,
						 PropertyChangeMask);

			if(XGetWMName(JUCE_NAMESPACE::display, evt->xmaprequest.window, &textProp))
			{
				newTab->setName((const char *)(textProp.value));
				free(textProp.value); //?
			}

			newWin = new TwindyWindow(JUCE_NAMESPACE::display, evt->xmaprequest.window);
			upperPanelComps[workspaceIndex]->addWindow(newWin, newTab);
		}
	}
	else if(evt->xany.type == ConfigureRequest)
	{
		TWINDY_DBG_MESSAGE("ConfigureRequest received.");
		if(lowerWindow)
			tempWin = lowerWindow->getWindow();
		else
			tempWin = 0;

		if(destinedForLowerPanel && (evt->xconfigurerequest.window == tempWin))
		{
			/*if(!tempWin)
			{
				TwindyErrorDisplay::getInstance()->addErrorMessage(T("Error"),
																   T("Received a ConfigureRequest for a window we don't have?"));
				return;
			}*/
			windowChanges.x = lowerPanel->getWindowX();
			windowChanges.y = lowerPanel->getWindowX();
			windowChanges.width = lowerPanel->getWindowWidth();
			windowChanges.height = lowerPanel->getWindowHeight();
			windowChanges.sibling = evt->xconfigurerequest.above;
			windowChanges.stack_mode = evt->xconfigurerequest.detail;
			XConfigureWindow(JUCE_NAMESPACE::display,
							 evt->xconfigurerequest.window,
							 evt->xconfigurerequest.value_mask,
							 &windowChanges);
		}
		else
		{
			/*if(!evt->xconfigurerequest.window)
			{
				TwindyErrorDisplay::getInstance()->addErrorMessage(T("Error"),
																   T("Received a ConfigureRequest for a window we don't have?"));
				return;
			}*/
			windowChanges.x = 170;
			windowChanges.y = 35;
			windowChanges.width = (getWidth()-190);
			windowChanges.height = (getHeight()-280);
			windowChanges.sibling = evt->xconfigurerequest.above;
			windowChanges.stack_mode = evt->xconfigurerequest.detail;
			result = XConfigureWindow(JUCE_NAMESPACE::display,
							 evt->xconfigurerequest.window,
							 evt->xconfigurerequest.value_mask,
							 &windowChanges);
		}
	}
	else if(evt->xany.type == UnmapNotify)
	{
		TWINDY_DBG_MESSAGE("UnmapNotify received.");
		//I don't think we need to handle this...
		//removeWindow(evt->xdestroywindow.window);
	}
	else if(evt->xany.type == DestroyNotify)
	{
		TWINDY_DBG_MESSAGE("DestroyNotify received.");
		removeWindow(evt->xdestroywindow.window);
	}
	else if(evt->xany.type == LeaveNotify)
	{
		TWINDY_DBG_MESSAGE("LeaveNotify received.");
		//Not sure why, but window is always Twindy's window, not the window
		//we've entered, so we have to figure out which window we're supposed to
		//be giving focus to...
		temprect.setBounds(10, 30, (getWidth()-20), (getHeight()-270));
		if(temprect.contains(evt->xcrossing.x_root, evt->xcrossing.y_root))
		{
			if(currentUpperPanelComp > -1)
			{
				if(upperPanelComps[currentUpperPanelComp]->getCurrentWindow())
				{
					XSetInputFocus(JUCE_NAMESPACE::display,
								   upperPanelComps[currentUpperPanelComp]->getCurrentWindow()->getWindow(),
								   RevertToPointerRoot,
								   CurrentTime);
				}
			}
		}

		temprect.setBounds(220, (getHeight()-225), (getWidth()-400), 220);
		if(temprect.contains(evt->xcrossing.x_root, evt->xcrossing.y_root))
		{
			if(lowerWindow)
			{
				XSetInputFocus(JUCE_NAMESPACE::display,
						   	   lowerWindow->getWindow(),
						   	   RevertToPointerRoot,
						   	   CurrentTime);
			}
		}
	}
	else if(evt->xany.type == EnterNotify)
	{
		TWINDY_DBG_MESSAGE("EnterNotify received.");
	}
	else if(evt->xany.type == PropertyNotify)
	{
		TWINDY_DBG_MESSAGE("PropertyNotify received.");
		if(evt->xproperty.atom == XA_WM_NAME)
			redrawWindowName(evt->xproperty.window);
	}
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::closeLowerWindow()
{
	JUCE_NAMESPACE::String tempstr;

	if(lowerWindow)
	{
		lowerWindow->closeWindow();

		delete lowerWindow;
		lowerWindow = 0;

		tempstr = JUCE_NAMESPACE::TRANS("(nothing currently selected)");
		lowerPanel->setText(tempstr);
		repaint();
	}
}

//----------------------------------------------------------------------------------------------
Window TwindyRootWindow::getActualWindow()
{
	return reinterpret_cast<Window>(static_cast<JUCE_NAMESPACE::ComponentPeer *>(getPeer())->getNativeHandle());
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::setupMappingRedirect()
{
	int test;
	struct sigaction act;
    sigset_t maskSet;
	XSetWindowAttributes att;
	//To set the cursor to the usual pointer, rather than that horrible default X...
	Cursor cursor = XCreateFontCursor(JUCE_NAMESPACE::display, XC_left_ptr);

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
	test = XChangeWindowAttributes(JUCE_NAMESPACE::display,
								   RootWindow(JUCE_NAMESPACE::display,
											  DefaultScreen(JUCE_NAMESPACE::display)),
								   CWEventMask|CWCursor,
								   &att);
	if(test == BadAccess)
	{
		//Not sure this part is correct - we quit the program in xErrorHandler.
		TWINDY_DBG_MESSAGE("BadAccess error for XChangeWindowAttributes()");
		TwindyErrorDisplay::getInstance()->addErrorMessage(JUCE_NAMESPACE::TRANS("Error"),
														   JUCE_NAMESPACE::TRANS("Failed: BadAccess error (another window's already redirecting the relevant events)."));
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
	JUCE_NAMESPACE::String instr(cmd);
	JUCE_NAMESPACE::String tempstr;

	if(instr == JUCE_NAMESPACE::String::empty)
	{
		JUCE_NAMESPACE::AlertWindow dlg(JUCE_NAMESPACE::TRANS("Run Executable"),
										JUCE_NAMESPACE::TRANS("Choose an executable to run."),
										JUCE_NAMESPACE::AlertWindow::QuestionIcon);

		dlg.addTextEditor(T("dlgTE"),
						  T(""),
						  JUCE_NAMESPACE::TRANS("executable:"),
						  false);
		dlg.addButton(JUCE_NAMESPACE::TRANS("cancel"), 0);
		dlg.addButton(JUCE_NAMESPACE::TRANS("ok"),
					  1,
					  JUCE_NAMESPACE::KeyPress(JUCE_NAMESPACE::KeyPress::returnKey,
											   JUCE_NAMESPACE::ModifierKeys(),
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
			TwindyErrorDisplay::getInstance()->addErrorMessage(JUCE_NAMESPACE::TRANS("Error"),
														 	   JUCE_NAMESPACE::TRANS("Could not start executable.  Cleaning up."));
			exit(1);
			break;
		case -1: //Parent process - unsuccessful.
			TwindyErrorDisplay::getInstance()->addErrorMessage(JUCE_NAMESPACE::TRANS("Error"),
														 	   JUCE_NAMESPACE::TRANS("Could not start executable."));
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
	JUCE_NAMESPACE::String tempstr;

	if(lowerWindow)
	{
		if(win == lowerWindow->getWindow())
		{
			delete lowerWindow;
			lowerWindow = 0;

			tempstr = JUCE_NAMESPACE::TRANS("(nothing currently selected)");
			lowerPanel->setText(tempstr);
			repaint();

			return;
		}
	}
	else
	{
		//upperPanel->removeWindow(win);
		for(int i=0;i<upperPanelComps.size();++i)
			upperPanelComps[i]->removeWindow(win);
	}
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::giveWindowFocus(TwindyWindow *win)
{
	
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::redrawWindowName(Window win)
{
	int i, j;
	JUCE_NAMESPACE::String tempstr;
	XTextProperty textProp;
	TwindyUpperTab *tempTab;

	//Update the lower window's text if necessary.
	if(lowerWindow)
	{
		if(win == lowerWindow->getWindow())
		{
			if(XGetWMName(JUCE_NAMESPACE::display, win, &textProp))
			{
				tempstr = (const char *)(textProp.value);
				lowerPanel->setText(tempstr);
				repaint();
				free(textProp.value); //?
				return;
			}
		}
	}

	//Update the upper window's text if necessary.
	for(i=0;i<upperPanelComps.size();++i)
	{
		for(j=0;j<upperPanelComps[i]->getNumWindows();++j)
		{
			if(upperPanelComps[i]->getWindow(j)->getWindow() == win)
			{
				if(XGetWMName(JUCE_NAMESPACE::display, win, &textProp))
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
	JUCE_NAMESPACE::ComponentPeer *twindy;

	if(lowerWindow)
		XRaiseWindow(JUCE_NAMESPACE::display, lowerWindow->getWindow());
	if(currentUpperPanelComp > -1)
	{
		if(upperPanelComps[currentUpperPanelComp]->getNumWindows() > 0)
		{
			XRaiseWindow(JUCE_NAMESPACE::display,
						 upperPanelComps[currentUpperPanelComp]->getCurrentWindow()->getWindow());
		}
	}

	//We get crashes if Twindy loses focus after a PopupMenu has been shown.
	//This should fix that.
	twindy = static_cast<JUCE_NAMESPACE::ComponentPeer *>(getPeer());
	XSetInputFocus(JUCE_NAMESPACE::display,
				   RootWindow(JUCE_NAMESPACE::display, DefaultScreen(JUCE_NAMESPACE::display)),
				   RevertToPointerRoot,
				   CurrentTime);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::moveLowerToUpper()
{
	TwindyUpperTab *newTab;
	XTextProperty textProp;
	int workspaceIndex = currentUpperPanelComp;

	if(lowerWindow)
	{
		//Move and resize window.
		XMoveWindow(JUCE_NAMESPACE::display,
					lowerWindow->getWindow(),
					170,
					39);
		XResizeWindow(JUCE_NAMESPACE::display,
					  lowerWindow->getWindow(),
					  (getWidth()-190),
					  ((getHeight()-(getHeight()/3))-28));

		//Add lowerWindow to current upper panel.
		if(workspaceIndex < 0)
			workspaceIndex = 0;

		if((upperPanelComps[workspaceIndex]->getCurrentWindow())&&
		   (upperPanelComps[workspaceIndex]->isWorkspaceVisible()))
			upperPanelComps[workspaceIndex]->getCurrentWindow()->hide();

		newTab = new TwindyUpperTab(T("Browser"),
									colours.propertyPanelText,
									colours.propertyPanelBackground);
		if(XGetWMName(JUCE_NAMESPACE::display, lowerWindow->getWindow(), &textProp))
		{
			newTab->setName((const char *)(textProp.value));
			free(textProp.value); //?
		}
		upperPanelComps[workspaceIndex]->addWindow(lowerWindow, newTab);

		//Clear lowerWindow.
		lowerWindow = 0;
		lowerPanel->setText(JUCE_NAMESPACE::TRANS("(nothing currently selected)"));
		lowerPanel->repaint();

		TWINDY_DBG_MESSAGE("Lower window moved to upper panel.");
	}
	else
		TWINDY_DBG_MESSAGE("No lower window to move to upper panel");
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::moveUpperToLower()
{
	TwindyWindow *tempWin;
	XTextProperty textProp;
	JUCE_NAMESPACE::String tempstr;
	int workspaceIndex = currentUpperPanelComp;

	//Skip if we're on the preferences page.
	if(workspaceIndex < 0)
		return;

	//Make sure there's a window visible.
	tempWin = upperPanelComps[workspaceIndex]->getCurrentWindow();
	if(tempWin)
	{
		//If there's already a lower window, move it to the upper panel.
		if(lowerWindow)
			moveLowerToUpper();

		//Move and resize window.
		XMoveWindow(JUCE_NAMESPACE::display,
					tempWin->getWindow(),
					lowerPanel->getWindowX(),
					lowerPanel->getWindowY());
		XResizeWindow(JUCE_NAMESPACE::display,
					  tempWin->getWindow(),
					  lowerPanel->getWindowWidth(),
					  lowerPanel->getWindowHeight());

		//Add tempWin to lower panel.
		lowerWindow = tempWin;
		if(XGetWMName(JUCE_NAMESPACE::display, lowerWindow->getWindow(), &textProp))
		{
			tempstr = (const char *)(textProp.value);
			lowerPanel->setText(tempstr);
			repaint();
			free(textProp.value); //?
		}

		//Remove original upper panel window.
		upperPanelComps[workspaceIndex]->removeWindowNoDelete(tempWin);
		tempWin->show();

		TWINDY_DBG_MESSAGE("Upper window moved to lower panel.");
	}
	else
		TWINDY_DBG_MESSAGE("No upper window visible.");
}

//----------------------------------------------------------------------------------------------
/*Window JUCE_NAMESPACE::WindowHandle::getHandle() const
{
	return handle;
}*/
