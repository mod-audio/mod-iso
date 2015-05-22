//	TwindyRootWindow.h - The Component which makes up the root window of the
//						 Window Manager.
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

#ifndef TWINDYROOTWINDOW_H_
#define TWINDYROOTWINDOW_H_

#include "juce_amalgamated.h"

///	A struct containing all the colours we use in the window.
/*!
	These should correspond directly to the ones used in tracktionscheme files.
 */
struct TwindyColours {
	Colour mainBackground;
	Colour tabAreaBackground;
	Colour menuText;
	Colour propertyPanelBackground;
	Colour propertyPanelText;
	Colour propertyPanelOutline;
	Colour selectedFilterOutline;
	Colour yellowButton;
	Colour blueButton;
};

class TwindyUpperPanel;
class TwindyUpperTab;
class TwindyWindow;
class Clock;
class DrawableTextButton;
class TwindyProperties;
class TwindyTabs;
class TwindyPreferences;

typedef unsigned long XID;
typedef XID Window;

static const uint upperPanelCompSize = 2;

///	This is the main part of the program.
/*!
	It forms the root window (?) for the Window Manager, and provides buttons
	and other controls to perform various Window Manager operations (start
	programs, logout etc.).
 */
class TwindyRootWindow : public Component,
                         public ButtonListener,
                         public MapRequestCallback
{
  public:
	///	Constructor.
	TwindyRootWindow();
	///	Destructor.
	~TwindyRootWindow();

	///	Draws the background.
	void paint(Graphics &g);

	///	Called when the window's resized (this shouldn't happend too often).
	void resized();

	///	Handles any button pressed events.
	void buttonClicked(Button* button);

	///	Used to set the focus of currently-open windows.
	//void mouseMove(const MouseEvent& e);

	///	Called from TwindyTabs to get a pointer to it's workspace contents.
	TwindyUpperPanel *getUpperPanel(int index) {return upperPanelComps[index];};

	///	Called from TwindyTabs to tell us which tab is currently visible.
	/*!
		-1 means the preferences tab is visible.
	 */
	void setVisibleUpperPanel(uint index);
	///	Returns a pointer to the preferences panel.
	TwindyPreferences *getPreferencesPanel() const {return preferences;};

	///	Returns the TwindyProperties instance used to set various settings.
	TwindyProperties *getProperties() const {return properties;};
	///	Returns the colour scheme for the indexed workspace.
	const TwindyColours& getColours() {return colours;};
	///	Returns a pointer to the clock, so we can change it's properties.
	Clock *getClock() const {return clock;};

	///	Called to update the colours of the various (visible) Components.
	void updateColours();

	///	Returns the XServer Window handle to this window.
	Window getActualWindow();

	///	Called whenever JUCE receives a MapRequest, ConfigureRequest or CirculateRequest event.
	/*!
		Should probably have a more descriptive name...
	 */
	void callbackFunction(void *event);

        // Hide or show the dev buttons.
        void setButtonsVisible(const bool visible);

        // Go to mod-app tab.
        void changeToAppTab();

  private:
	///	Tells the Xserver to redirect all MapRequest events to this window.
	void setupMappingRedirect();
	///	Wrapper to launchExecutable() for lower panel programs.
	void removeWindow(Window win);
	///	Gives win focus.
	void giveWindowFocus(TwindyWindow *win);
	///	Redraws the window's name when it changes.
	void redrawWindowName(Window win);
	///	Raises the 2 currently-open windows.
	/*!
		We need this because any open windows are sent behind Twindy when a
		PopupMenu has been displayed.
	 */
	void raiseAllWindows();

	///	Loads all the colours we use from a .tracktionscheme file.
	void loadColours(const String& file);

	///	TabbedComponent holding all the workspace Components.
	TwindyTabs *workspaces;
	///	Array of workspace Components.
	TwindyUpperPanel* upperPanelComps[2];
	///	The preferences panel.
	TwindyPreferences* preferences;
	///	Index of the currently visible workspace Component.
	uint currentUpperPanelComp;

	/// Exit button.
	DrawableTextButton* exitButton;
	/// Top left button in the lower left.
	DrawableTextButton* leftButton1;
	/// Top left button in the lower left.
	DrawableTextButton* leftButton2;
	/// Top left button in the lower left.
	DrawableTextButton* leftButton3;
	/// Top left button in the lower left.
	DrawableTextButton* leftButton4;
	/// Top left button in the lower left.
	DrawableTextButton* leftButton5;
	/// Top left button in the lower left.
	DrawableTextButton* leftButton6;
	/// Top left button in the lower left.
	DrawableTextButton* leftButton7;

	/// Clock to display the current time.
	Clock* clock;

	/// Object used to read/write various Twindy properties.
	TwindyProperties* properties;
	/// The colours we use in this window.
	TwindyColours colours;

        /// An array of the pids for the programs we started via Dev buttons.
	Array<pid_t> pidArray;
};

#endif
