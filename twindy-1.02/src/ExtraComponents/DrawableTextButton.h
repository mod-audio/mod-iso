//	DrawableTextButton.h - Simple wrapper class which makes a DrawableButton
//						   with the same characteristics as the old TextButtons.
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

#ifndef DRAWABLETEXTBUTTON_H_
#define DRAWABLETEXTBUTTON_H_

#include "../juce_amalgamated.h"

///	A TextButton made out of Drawables.
/*!
	This is a simple wrapper class which makes a DrawableButton with similar
	characteristics to the old TextButtons.
 */
class DrawableTextButton : public DrawableButton
{
  public:
	///	Constructor.
	/*!
		\param buttonName The button's name.
		\param buttonText The text displayed on the button.
	 */
	DrawableTextButton(const String& buttonName, const String& buttonText);
	///	Destructor.
	~DrawableTextButton();

	///	Sets the button's text.
	void setButtonText(const String& newText);
	///	Sets whether to make the button's text bold or not.
	void setBold(bool val = true);
	///	Sets the colour to draw the text as.
	void setTextColour(const Colour& col);
  private:
	///	The drawable representing the button's text.
	DrawableText drawableText;
	///	The actual text the button displays.
	String buttonText;

	///	The font to use to draw the text.
	Font buttonFont;
	///	The colour to use for the text.
	Colour textColour;
};

#endif
