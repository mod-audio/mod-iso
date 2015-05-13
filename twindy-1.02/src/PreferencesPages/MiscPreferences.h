//	MiscPreferences.h - Preferences page for miscellaneous options.
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

#ifndef MISCPREFERENCES_H_
#define MISCPREFERENCES_H_

class TwindyToggleButton;
class DrawableTextButton;

#include "../juce_amalgamated.h"

///	Preferences page for miscellaneous options.
class MiscPreferences : public Component,
						public ListBoxModel,
						public ButtonListener,
						public TextEditorListener
{
  public:
	///	Constructor.
	MiscPreferences();
	///	Destructor.
	~MiscPreferences();

	///	Places and sizes the various components.
	void resized();

	///	Called when a button is clicked.
	void buttonClicked(Button *button);
	///	Over-ridden to do nothing.
	void textEditorTextChanged(TextEditor &editor) {};
	///	So we know when to update the month separator.
	void textEditorReturnKeyPressed(TextEditor &editor);
	///	Over-ridden to do nothing.
	void textEditorEscapeKeyPressed(TextEditor &editor) {};
	///	Over-ridden to do nothing.
	void textEditorFocusLost(TextEditor &editor) {};

	///	The number of menu items.
	int getNumRows() {return fonts.size();};
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
  private:
	///	Stolen from LookAndFeel so we can have control over the colour.
	void drawGroupComponentOutline(Graphics& g,
								   int width,
								   int height,
                                   const String& text,
                                   const Justification& position);

	///	Fonts listbox label.
	Label *fontListLabel;
	///	Fonts listbox.
	ListBox *fontList;
	///	Clock includeSeconds ToggleButton.
	TwindyToggleButton *includeSeconds;
	///	Clock use24HourClock ToggleButton.
	TwindyToggleButton *use24HourClock;
	///	Clock monthDisplayedFirst ToggleButton.
	TwindyToggleButton *monthDisplayedFirst;
	///	Clock dateSeparator Label.
	Label *dateSepLabel;
	///	Clock dateSeparator TextEditor.
	TextEditor *dateSeparator;
	///	Localisation file Label.
	Label *localisationLabel;
	///	Localisation file TextEditor.
	TextEditor *localisationFile;
	///	Localistion file dialog button.
	DrawableTextButton *localisationDialog;

	///	List of all the fonts on the system.
	OwnedArray<Font> fonts;

	///	The colour of the background.
	Colour backgroundColour;
	///	The colour of the text.
	Colour textColour;
};

#endif
