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
typedef struct
{
	JUCE_NAMESPACE::Colour mainBackground;
	JUCE_NAMESPACE::Colour tabAreaBackground;
	JUCE_NAMESPACE::Colour menuText;
	JUCE_NAMESPACE::Colour propertyPanelBackground;
	JUCE_NAMESPACE::Colour propertyPanelText;
	JUCE_NAMESPACE::Colour propertyPanelOutline;
	JUCE_NAMESPACE::Colour selectedFilterOutline;
	JUCE_NAMESPACE::Colour yellowButton;
	JUCE_NAMESPACE::Colour blueButton;
} TwindyColours;

class TwindyUpperPanel;
class TwindyUpperTab;
class TwindyLowerPanel;
class TwindyWindow;
class Clock;
class DrawableTextButton;
class TwindyProperties;
class TwindyTabs;
class TwindyPreferences;

typedef unsigned long XID;
typedef XID Window;

///	This is the main part of the program.
/*!
	It forms the root window (?) for the Window Manager, and provides buttons
	and other controls to perform various Window Manager operations (start
	programs, logout etc.).
 */
class TwindyRootWindow : public JUCE_NAMESPACE::Component,
						 public JUCE_NAMESPACE::ActionListener,
						 public JUCE_NAMESPACE::ButtonListener,
						 public JUCE_NAMESPACE::MapRequestCallback
{
  public:
	///	Constructor.
	TwindyRootWindow();
	///	Destructor.
	~TwindyRootWindow();

	///	Draws the background.
	void paint(JUCE_NAMESPACE::Graphics &g);

	///	Called when the window's resized (this shouldn't happend too often).
	void resized();

	///	Handles e.g. button pressed events.
	/*!
		This probably isn't used anymore, now that we're using ButtonListeners.
	 */
	void actionListenerCallback(const JUCE_NAMESPACE::String& message);
	///	Handles any button pressed events.
	void buttonClicked(JUCE_NAMESPACE::Button* button);

	///	Used to set the focus of currently-open windows.
	//void mouseMove(const JUCE_NAMESPACE::MouseEvent& e);

	///	Called from TwindyTabs to get a pointer to it's workspace contents.
	TwindyUpperPanel *getUpperPanel(int index) {return upperPanelComps[index];};
	///	Returns the number of workspaces currently available.
	/*!
		Does not include the preferences page.
	 */
	int getNumUpperPanels() const {return upperPanelComps.size();};
	///	Called from TwindyTabs to tell us which tab is currently visible.
	/*!
		-1 means the preferences tab is visible.
	 */
	void setVisibleUpperPanel(int index);
	///	Returns a pointer to the preferences panel.
	TwindyPreferences *getPreferencesPanel() const {return preferences;};

	///	Called when the close button of the lower panel has been clicked.
	/*!
		We check if there's currently a window open, and close it if there is.
	 */
	void closeLowerWindow();

	///	Returns the TwindyProperties instance used to set various settings.
	TwindyProperties *getProperties() const {return properties;};
	///	Returns the colour scheme for the indexed workspace.
	const TwindyColours& getColours(int index) {return colours;};
	///	Returns a pointer to the clock, so we can change it's properties.
	Clock *getClock() const {return clock;};

	///	Called to update the various user-controllable preferences.
	void prefsChanged();
	///	Called to update the colours of the various (visible) Components.
	void updateColours();

	///	Returns the XServer Window handle to this window.
	Window getActualWindow();

	///	Called whenever JUCE receives a MapRequest, ConfigureRequest or CirculateRequest event.
	/*!
		Should probably have a more descriptive name...
	 */
	void callbackFunction(void *event);
  private:
	///	Tells the Xserver to redirect all MapRequest events to this window.
	void setupMappingRedirect();
	///	Launches an executable.
	void launchExecutable(const char *cmd, bool storePid = false);
	///	Wrapper to launchExecutable() for lower panel programs.
	void launchLowerPanel(JUCE_NAMESPACE::String cmd);
	///	Finds the TwindyWindow (& it's associated tab) with win, and removes/deletes it.
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
	///	Moves the lower window to the top panel.
	void moveLowerToUpper();
	///	Moves the currently-selected upper window to the lower panel.
	void moveUpperToLower();

	///	Loads all the colours we use from a .tracktionscheme file.
	void loadColours(const JUCE_NAMESPACE::String& file);
	///	Launches all the programs specified in twindyrc.
	void launchStartupPrograms();

	//	The TabbedComponent that takes up the upper panel.
	//TwindyUpperPanel *upperPanel;
	///	TabbedComponent holding all the workspace Components.
	TwindyTabs *workspaces;
	///	Array of workspace Components.
	JUCE_NAMESPACE::OwnedArray<TwindyUpperPanel> upperPanelComps;
	///	The preferences panel.
	TwindyPreferences *preferences;
	///	Index of the currently visible workspace Component.
	int currentUpperPanelComp;
	///	The lower panel (takes the place of Tracktion's properties panel).
	TwindyLowerPanel *lowerPanel;

	///	Upper panel buttons label.
	JUCE_NAMESPACE::Label *upperLabel;
	///	Lower panel buttons label.
	JUCE_NAMESPACE::Label *lowerLabel;

	///	Button to move a lower panel app to the upper panel.
	JUCE_NAMESPACE::DrawableButton *moveUp;
	///	And vice-versa.
	JUCE_NAMESPACE::DrawableButton *moveDown;

	///	Exit button.
	//JUCE_NAMESPACE::TextButton *exitButton;
	DrawableTextButton *exitButton;
	///	Top left button in the lower left.
	//JUCE_NAMESPACE::TextButton *leftButton1;
	DrawableTextButton *leftButton1;
	///	Top left button in the lower left.
	//JUCE_NAMESPACE::TextButton *leftButton2;
	DrawableTextButton *leftButton2;
	///	Top left button in the lower left.
	//JUCE_NAMESPACE::TextButton *leftButton3;
	DrawableTextButton *leftButton3;
	///	Top left button in the lower left.
	//JUCE_NAMESPACE::TextButton *leftButton4;
	DrawableTextButton *leftButton4;
	///	Top left button in the lower left.
	//JUCE_NAMESPACE::TextButton *leftButton5;
	DrawableTextButton *leftButton5;
	///	Top left button in the lower left.
	//JUCE_NAMESPACE::TextButton *leftButton6;
	DrawableTextButton *leftButton6;
	///	Top left button in the lower left.
	//JUCE_NAMESPACE::TextButton *leftButton7;
	DrawableTextButton *leftButton7;

	///	Top right button in the lower left.
	//JUCE_NAMESPACE::TextButton *rightButton1;
	DrawableTextButton *rightButton1;
	///	Top right button in the lower left.
	//JUCE_NAMESPACE::TextButton *rightButton2;
	DrawableTextButton *rightButton2;
	///	Top right button in the lower left.
	//JUCE_NAMESPACE::TextButton *rightButton3;
	DrawableTextButton *rightButton3;
	///	Top right button in the lower left.
	//JUCE_NAMESPACE::TextButton *rightButton4;
	DrawableTextButton *rightButton4;
	///	Top right button in the lower left.
	//JUCE_NAMESPACE::TextButton *rightButton5;
	DrawableTextButton *rightButton5;
	///	Top right button in the lower left.
	//JUCE_NAMESPACE::TextButton *rightButton6;
	DrawableTextButton *rightButton6;
	///	Top right button in the lower left.
	//JUCE_NAMESPACE::TextButton *rightButton7;
	DrawableTextButton *rightButton7;

	///	Clock to display the current time.
	Clock *clock;

	///	Object used to read/write various Twindy properties.
	TwindyProperties *properties;
	///	The colours we use in this window.
	TwindyColours colours;

	///	The window currently displayed 'in' the lower panel.
	/*!
		0 if none is displayed.
	 */
	TwindyWindow *lowerWindow;
	///	This is set to true whenever a program destined for the lowerPanel has just been launched.
	bool destinedForLowerPanel;

	///	Which panel currently has focus: upper(1), or lower(0).
	bool currentlyInFocus;

	///	An array of the pids for all the programs we started at startup.
	/*!
		If we start a program in the startup sequence, we need to keep a hold of
		it's pid so we can kill it when we quit (only necessary if it's a
		non-gui app?).
	 */
	JUCE_NAMESPACE::Array<pid_t> pidArray;
};

#endif
