//	DocsPage.h - Simple preferences page displaying the Twindy docs.
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

#ifndef DOCSPAGE_H_
#define DOCSPAGE_H_

///	Simple preferences page displaying the Twindy docs.
class DocsPage : public Component,
				 public ActionListener
{
  public:
	///	Constructor.
	DocsPage();
	///	Destructor.
	~DocsPage();

	///	Places and sizes the various components.
	void resized();
	///	To draw a border around mainText.
	void paintOverChildren(Graphics &g);

	///	Called when the user selects a doc file to view.
	void actionListenerCallback(const String& message);

	///	Called to set the background colour and text colour.
	void setColours(const Colour& backCol,
					const Colour& textCol,
					const Colour& buttonCol);
  private:
	///	The main TextEditor.
	TextEditor *mainText;
	///	The ComboBox to select which doc file to display.
	ComboBox *docSelector;

	///	The colour of the background.
	Colour backgroundColour;
	///	The colour of the text.
	Colour textColour;
};

#endif
