//	ButtonPreferences.h - Preferences page for the buttons.
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

#ifndef BUTTONPREFERENCES_H_
#define BUTTONPREFERENCES_H_

#include "../TwindyProperties.h"

class TwindyToggleButton;

//------------------------------------------------------------------------------
class ButtonMenuComponent;
///	The button preferences.
class TwindyButtonPreferences : public Component,
								public ListBoxModel,
								public ButtonListener,
								public TextEditorListener
{
  public:
	///	Constructor.
	TwindyButtonPreferences();
	///	Destructor.
	~TwindyButtonPreferences();

	///	Used to place and size the various Components correctly.
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

	///	The number of buttons.
	int getNumRows() {return 14;};
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
		This gets called every time a different button is selected, and every
		time a different workspace tab is selected.
	 */
	void saveChanges();
  private:
	///	Listbox label.
	Label *buttonsLabel;
	///	The list box containing all the buttons.
	ListBox *buttons;
	///	Button text label.
	Label *textLabel;
	///	Button text editor.
	TextEditor *textEditor;
	///	Menu toggle button.
	TwindyToggleButton *buttonIsMenu;
	///	Button executable label.
	Label *execLabel;
	///	Button executable editor.
	TextEditor *execEditor;
	///	Component holding the menu Components.
	ButtonMenuComponent *menuStuff;

	///	The colour of the background.
	Colour backgroundColour;
	///	The colour of the text.
	Colour textColour;

	///	The last button that was selected in the buttons list.
	int lastButton;
	///	Used to determine whether to save changes in selectedRowsChanged().
	bool justConstructed;
};

//------------------------------------------------------------------------------
class DrawableTextButton;
///	Class holding all the menu stuff for the TwindyButtonPreferences Component.
/*!
	Had to make this a separate Component because I need 2 SimpleListBoxes, and
	this seemed like the simplest way to do that (because each list box needs
	to override paintListBoxItem() according to it's needs).
 */
class ButtonMenuComponent : public Component,
							public ListBoxModel,
							public ButtonListener,
							public TextEditorListener
{
  public:
	///	Constructor.
	ButtonMenuComponent();
	///	Destructor.
	~ButtonMenuComponent();

	///	Used to place and size the various Components correctly.
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
	int getNumRows() {return menu.size();};
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

	///	Clears the current list of menu items.
	void clearMenuItems();
	///	Adds a menu item to the list.
	void addMenuItem(TwindyProperty *newItem);
	///	Removes the indexed item from the list.
	void removeMenuItem(int index);
	///	Returns the current menu.
	const OwnedArray<TwindyProperty> * const getMenuItems() const {return &menu;};

	///	Called to set the background colour and text colour.
	void setColours(const Colour& backCol,
					const Colour& textCol,
					const Colour& buttonCol);
  private:
	///	Menu item list box label.
	Label *menuItemsLabel;
	///	Menu item list box.
	ListBox *menuItems;
	///	Menu text label.
	Label *textLabel;
	///	Menu text editor.
	TextEditor *textEditor;
	///	Menu executable label.
	Label *execLabel;
	///	Menu executable editor.
	TextEditor *execEditor;
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

	///	All the menu items for the currently selected button.
	OwnedArray<TwindyProperty> menu;

	///	The colour of the background.
	Colour backgroundColour;
	///	The colour of the text.
	Colour textColour;

	//The index of the last selected row.
	int lastRow;
};

#endif
