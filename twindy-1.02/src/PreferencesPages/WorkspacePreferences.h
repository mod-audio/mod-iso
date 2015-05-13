//	WorkspacePreferences.h - Preferences page for the workspaces.
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

#ifndef WORKSPACEPREFERENCES_H_
#define WORKSPACEPREFERENCES_H_

#include "TwindyProperties.h"

class DrawableTextButton;

///	Preferences page for the workspaces.
class WorkspacePreferences : public Component,
							 public ListBoxModel,
							 public ButtonListener,
							 public TextEditorListener
{
  public:
	///	Constructor.
	WorkspacePreferences();
	///	Destructor.
	~WorkspacePreferences();

	///	Places and sizes the various components.
	void resized();

	///	Called when one of the buttons is clicked.
	void buttonClicked(Button *button);

	///	Over-ridden to do nothing.
	void textEditorTextChanged(TextEditor &editor) {};
	///	So we know when to update workspaces.
	void textEditorReturnKeyPressed(TextEditor &editor);
	///	Over-ridden to do nothing.
	void textEditorEscapeKeyPressed(TextEditor &editor) {};
	///	Over-ridden to do nothing.
	void textEditorFocusLost(TextEditor &editor) {};

	///	The number of menu items.
	int getNumRows() {return workspaces.size();};
	///	Draws the list box items.
	void paintListBoxItem(int rowNumber,
						  Graphics &g,
						  int width,
						  int height,
						  bool rowIsSelected);
	///	So we get informed when the selected row is changed.
	void selectedRowsChanged(int lastRowselected);
	///	To draw a border around the listbox etc.
	void paintOverChildren(Graphics &g);

	///	Called to set the background colour and text colour.
	void setColours(const Colour& backCol,
					const Colour& textCol,
					const Colour& buttonCol);
	///	Called to save any changes made to the TwindyProperties instance.
	/*!
		This gets called by TwindyPreferences when a preferences tab is changed,	
		and TwindyTabs when a workspace tab is changed.
	 */
	void saveChanges();
	///	Called from TwindyPreferences to tell us when to update our list of workspaces.
	void updateWorkspaces();
  private:
	///	Workspaces list box label.
	Label *workspacesLabel;
	///	Workspaces list box.
	ListBox *workspacesList;
	///	Workspace name label.
	Label *textLabel;
	///	Workspace name editor.
	TextEditor *textEditor;
	///	Tracktionscheme label.
	Label *schemeLabel;
	///	Tracktionscheme editor.
	TextEditor *schemeEditor;
	///	Tracktionscheme load button.
	DrawableTextButton *schemeLoad;
	///	Add item button.
	DrawableTextButton *addItem;
	///	Remove item button.
	DrawableTextButton *removeItem;
	///	Clear items button.
	DrawableTextButton *clearItems;
	///	Move item up button.
	DrawableTextButton *itemUp;
	///	Move item down button.
	DrawableTextButton *itemDown;

	///	Array of workspaces.
	OwnedArray<TwindyProperty> workspaces;

	///	The colour of the background.
	Colour backgroundColour;
	///	The colour of the text.
	Colour textColour;
};

#endif
