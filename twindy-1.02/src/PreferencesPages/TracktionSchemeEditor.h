//	TracktionSchemeEditor.h - Preferences page to edit .tracktionscheme files.
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

#ifndef TRACKTIONSCHEMEEDITOR_H_
#define TRACKTIONSCHEMEEDITOR_H_

#include "TwindyRootWindow.h"

class DrawableTextButton;

///	Preferences page to edit .tracktionscheme files.
class TracktionSchemeEditor : public Component,
							  public ListBoxModel,
							  public ChangeListener,
							  public ButtonListener
{
  public:
	///	Constructor.
	TracktionSchemeEditor();
	///	Destructor.
	~TracktionSchemeEditor();

	///	Places and sizes the various components.
	void resized();

	///	So we get called when the user changes the current colour.
	void changeListenerCallback(void *objectThatHasChanged);
	///	Called when the user clicks a button.
	void buttonClicked(Button *button);

	///	The number of menu items.
	int getNumRows() {return 9;};
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
  private:
	///	Helper method.  Loads the relevant colours from a tracktionscheme file.
	void loadColours(const File& file);
	///	Helper method.  Saves the current colours to the file in pathEditor.
	void saveColours();
	///	Helper method.  Creates a default tracktionscheme file.
	/*!
		Called from saveColours() if the file in pathEditor doesn't exist yet.
	 */
	void writeNewTracktionScheme(const String& filepath);

	///	Colours list box label.
	Label *coloursLabel;
	///	Colours list box.
	ListBox *colours;
	///	Colour selector.
	ColourSelector *colourSelector;
	///	New tracktionscheme button.
	DrawableTextButton *newButton;
	///	Open tracktionscheme button.
	DrawableTextButton *openButton;
	///	Save tracktionscheme button.
	DrawableTextButton *saveButton;
	///	Save as tracktionscheme button.
	DrawableTextButton *saveAsButton;
	///	Tracktionscheme path label.
	Label *pathLabel;
	///	Tracktionscheme path editor.
	TextEditor *pathEditor;

	///	The colour of the background.
	Colour backgroundColour;
	///	The colour of the text.
	Colour textColour;

	///	The current set of colours.
	TwindyColours currentColours;
};

#endif
