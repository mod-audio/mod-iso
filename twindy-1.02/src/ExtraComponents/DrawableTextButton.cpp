//	DrawableTextButton.cpp - Simple wrapper class which makes a DrawableButton
//						     with the same characteristics as the old TextButtons.
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

#include "juce_amalgamated.h"
#include "DrawableTextButton.h"

#include <iostream>

//------------------------------------------------------------------------------
DrawableTextButton::DrawableTextButton(const String& buttonName,
									   const String& buttonText):
DrawableButton(buttonName, ImageOnButtonBackground),
textColour(Colours::black)
{
	this->buttonText = buttonText;
	drawableText.setText(buttonText, buttonFont);
	drawableText.setColour(textColour);

	setImages(&drawableText);
}

//------------------------------------------------------------------------------
DrawableTextButton::~DrawableTextButton()
{
	
}

//------------------------------------------------------------------------------
void DrawableTextButton::setButtonText(const String& newText)
{
	buttonText = newText;
	drawableText.setText(buttonText, buttonFont);
	drawableText.setColour(textColour);

	setImages(&drawableText);
}

//------------------------------------------------------------------------------
void DrawableTextButton::setBold(bool val)
{
	buttonFont.setBold(val);
	drawableText.setText(buttonText, buttonFont);
	drawableText.setColour(textColour);

	setImages(&drawableText);
}



//------------------------------------------------------------------------------
void DrawableTextButton::setTextColour(const Colour& col)
{
	textColour = col;
	drawableText.setText(buttonText, buttonFont);
	drawableText.setColour(textColour);

	setImages(&drawableText);
}
