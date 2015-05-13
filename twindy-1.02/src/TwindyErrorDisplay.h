//	TwindyErrorDisplay.h - Singleton Component to display any error messages in
//						   the lower right corner of the 'root' window.
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

#ifndef TWINDYERRORDISPLAY_H_
#define TWINDYERRORDISPLAY_H_

class DrawableTextButton;

///	Simple struct to hold an error message (heading and message).
typedef struct
{
	JUCE_NAMESPACE::String heading;
	JUCE_NAMESPACE::String message;
} ErrorMessage;

///	Singleton Component to display any error messages in the lower right corner of the 'root' window.
class TwindyErrorDisplay : public JUCE_NAMESPACE::Component,
								  JUCE_NAMESPACE::ButtonListener
{
  public:
	///	Returns the single instance of the class, creates it if it doesn't exist.
	static TwindyErrorDisplay *getInstance();

	///	To handle the Ok button being clicked.
	void buttonClicked(JUCE_NAMESPACE::Button *button);

	///	Draws the current error message.
	void paint(JUCE_NAMESPACE::Graphics &g);

	///	We use this to place the Ok button correctly.
	void resized();

	///	Called to display an error message.
	/*!
		Error messages are essentially kept in a stack, with the earliest one
		being the one that is currently displayed.  When the user clicks the Ok
		button, the earliest message is removed from the stack, and the next one
		is displayed.
	 */
	void addErrorMessage(const JUCE_NAMESPACE::String& heading, const JUCE_NAMESPACE::String& message);

	///	Sets the colour of the background.
	void setBackgroundColour(const JUCE_NAMESPACE::Colour& col) {backColour = col;};
	///	Sets the colour of the text.
	void setTextColour(const JUCE_NAMESPACE::Colour& col) {textColour = col;};

  private:
	///	Constructor.
	TwindyErrorDisplay();
	///	Destructor.
	~TwindyErrorDisplay();

	///	Ok button used to dismiss an error.
	DrawableTextButton *ok;

	///	Array of current error messages.
	JUCE_NAMESPACE::OwnedArray<ErrorMessage> errorArray;

	///	Colour to paint the background of the display.
	JUCE_NAMESPACE::Colour backColour;
	///	Colour to paint the text.
	JUCE_NAMESPACE::Colour textColour;
};

#endif
