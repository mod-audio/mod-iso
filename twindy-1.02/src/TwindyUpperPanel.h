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
#include "TracktionScheme.h"

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
	TwindyUpperPanel(bool isMOD);
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
	TwindyWindow* getCurrentWindow() const;

	///	Returns a pointer to the indexed window.
	/*!
		Returns 0 if the index is invalid.
	 */
	TwindyWindow* getWindow(int index) const;

	///	Returns the number of windows in this workspace.
	int getNumWindows() const { return windows.size(); }

	///	Sets this workspace's name.
	void setName(const String& newName);

	///	Returns this workspace's name.
	const String& getName() const { return name; }

	///	This is called whenever a new tab is selected.
	/*!
		We use this to hide the current window, and show the one associated with
		the newly-selected tab.
	 */
	void tabWasClicked(int newSelectedIndex,
	                   bool clickedTabWasAlreadySelected,
	                   const ModifierKeys &currentModifiers);

	///	We override this to pass the event on to TwindyRootWindow.
	/*!
		We need to pass it on to TwindyRootWindow because we want to give the
		current window focus when the mouse is over it.
	 */
	void mouseMove(const MouseEvent& e);

	///	Indicates that the user has clicked close on one of our tabs.
	void closeTab(TwindyUpperTab *tab);

  private:
	///	Called when the user closes a window using Twindy's close buttons.
	void closeWindow(TwindyUpperTab *tab, int index);

	///	Whether or not this workspace is currently visible.
	bool workspaceVisible;

	///	When this is true, don't show/hide windows when a tab is selected.
	bool dontShowHideWindows;

	///	What windows are in this workspace.
	OwnedArray<TwindyWindow> windows;
	///	Index to the currently visible window in this workspace.
	int visibleWindow;

	///	This workspace's name (what's displayed on the tab).
	String name;

        // This is the 1st MOD tab
        bool isMOD;

	///	The panel's actual colours.
	TracktionScheme colours;
};

//------------------------------------------------------------------------------
///	Represents a tab in the upper panel.
/*!
	We subclass TabbedComponentBasicTab really just because we want to use a
	*nice*-looking font.
 */
class TwindyUpperTab : public TwindyTabbedComponentBasicTab,
                       public ButtonListener
{
public:
    /// Constructor.
    TwindyUpperTab(const String& text, Colour textCol, Colour fillCol);
    /// Destructor.
    ~TwindyUpperTab() override;

    /// We alter this slightly to use a nicer font.
    void paintTab(Graphics& g, bool isTheCurrentTab, const TwindyTabbedComponent* ownerTabbedComp) override;

    /// So the close button gets placed correctly.
    void resized() override;

    /// So we're notified when the close button is clicked.
    void buttonClicked(Button* button) override;

private:
    /// The button used to close the window associated with this tab.
    ShapeButton* closeButton;

    /// Needed in paintTab.
    Colour textColor;
    /// Needed in paintTab.
    Colour fillColor;
};

#endif
