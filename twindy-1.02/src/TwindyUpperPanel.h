//	TwindyUpperPanel.h - Component to hold windows in the upper panel.
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

#ifndef TWINDYUPPERPANEL_H_
#define TWINDYUPPERPANEL_H_

#include "TwindyTabbedComponent.h"
#include "TwindyWindow.h"

class TwindyUpperTab;
class TracktionScheme;

typedef unsigned long XID;
typedef XID Window;

//------------------------------------------------------------------------------
///	TabbedComponent subclass, used to handle windows in the upper panel.
/*!
	Each (currently open) window 'resides' in a tab.  (well, not really, they're
	just shown/hidden according to which tab is currently selected, but it
	should provide the same effect)
 */
class TwindyUpperPanel : public TwindyTabbedComponent
{
  public:
	///	Constructor.
	TwindyUpperPanel();
	///	Destructor.
	~TwindyUpperPanel();

	///	Called when this workspace is made visible.
	void setWorkspaceIsVisible(bool val);
	///	Called from TwindyRootWindow to determine which workspace is currently visible.
	bool isWorkspaceVisible() const {return workspaceVisible;};

	///	Called from TwindyRootWindow to add a window to this workspace.
	/*!
		newWindow will be deleted by TwindyUpperPanel.
	 */
	void addWindow(TwindyWindow *newWindow, TwindyUpperTab *newTab);
	///	Called from TwindyRootWindow to delete a window from this workspace.
	/*!
		If window is not on this workspace, nothing happens.
	 */
	void removeWindow(Window window);
	///	Removes the indexed window, does not delete it.
	/*!
		This is for when the user removes a workspace - any windows on the old
		workspace are copied across to the first valid workspace.
	 */
	void removeIndexedWindow(int index);
	///	Removes window from windows without deleting it.
	void removeWindowNoDelete(TwindyWindow *window);

	///	Returns a pointer to the currently visible window in this workspace.
	/*!
		Returns 0 if this workspace is not visible.
	 */
	TwindyWindow *getCurrentWindow();
	///	Returns a pointer to the indexed window.
	/*!
		Returns 0 if the index is invalid.
	 */
	TwindyWindow *getWindow(int index);
	///	Returns the number of windows in this workspace.
	int getNumWindows() const {return windows.size();};

	///	Sets this workspace's name.
	void setName(const JUCE_NAMESPACE::String& newName);
	///	Returns this workspace's name.
	const JUCE_NAMESPACE::String& getName() const {return name;};

	///	This is called whenever a new tab is selected.
	/*!
		We use this to hide the current window, and show the one associated with
		the newly-selected tab.
	 */
	void tabWasClicked(int newSelectedIndex,
					   bool clickedTabWasAlreadySelected,
  					   const JUCE_NAMESPACE::ModifierKeys &currentModifiers);

	///	We override this to pass the event on to TwindyRootWindow.
	/*!
		We need to pass it on to TwindyRootWindow because we want to give the
		current window focus when the mouse is over it.
	 */
	void mouseMove(const JUCE_NAMESPACE::MouseEvent& e);

	///	Indicates that the user has clicked close on one of our tabs.
	void closeTab(TwindyUpperTab *tab);

	///	Sets this panel's colours.
	/*!
		\param path The path to the tracktionscheme specified in twindyrc.
	 */
	void setSchemePath(const JUCE_NAMESPACE::String& path);
	///	Returns this panel's colours (the path to the tracktionscheme).
	const JUCE_NAMESPACE::String& getSchemePath() const {return coloursPath;};
	///	Returns a pointer to this panel's colour scheme.
	TracktionScheme *getTracktionScheme() const {return colours;};
  private:
	///	Called when the user closes a window using Twindy's close buttons.
	void closeWindow(TwindyUpperTab *tab, int index);

	///	Whether or not this workspace is currently visible.
	bool workspaceVisible;

	///	When this is true, don't show/hide windows when a tab is selected.
	bool dontShowHideWindows;

	///	What windows are in this workspace.
	JUCE_NAMESPACE::OwnedArray<TwindyWindow> windows;
	///	Index to the currently visible window in this workspace.
	int visibleWindow;

	///	This workspace's name (what's displayed on the tab).
	JUCE_NAMESPACE::String name;

	///	Path to this panel's colours.
	JUCE_NAMESPACE::String coloursPath;
	///	The panel's actual colours.
	TracktionScheme *colours;
};

//------------------------------------------------------------------------------
///	Represents a tab in the upper panel.
/*!
	We subclass TabbedComponentBasicTab really just because we want to use a
	*nice*-looking font.
 */
class TwindyUpperTab : public TwindyTabbedComponentBasicTab,
					   public JUCE_NAMESPACE::ButtonListener
{
  public:
	///	Constructor.
	TwindyUpperTab(const JUCE_NAMESPACE::String &text, JUCE_NAMESPACE::Colour textCol, JUCE_NAMESPACE::Colour fill);
	///	Destructor.
	~TwindyUpperTab();

	///	We alter this slightly to use a nicer font.
	void paintTab(JUCE_NAMESPACE::Graphics &g,
  				  bool isTheCurrentTab,
  				  const JUCE_NAMESPACE::TabbedComponent *ownerTabbedComp);

  	///	So the close button gets placed correctly.
	void resized();

	///	So we're notified when the close button is clicked.
	void buttonClicked(JUCE_NAMESPACE::Button* button);
  private:
	///	The button used to close the window associated with this tab.
	JUCE_NAMESPACE::ShapeButton *closeButton;

	///	Needed in paintTab.
	JUCE_NAMESPACE::Colour text;
	///	Needed in paintTab.
	JUCE_NAMESPACE::Colour fill;
};

#endif