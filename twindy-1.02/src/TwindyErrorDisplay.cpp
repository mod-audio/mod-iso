//	TwindyErrorDisplay.cpp - Singleton Component to display any error messages
//						 	 in the lower right corner of the 'root' window.
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
#include "TwindyErrorDisplay.h"
#include "DrawableTextButton.h"

#include <iostream>

//------------------------------------------------------------------------------
TwindyErrorDisplay *TwindyErrorDisplay::getInstance()
{
	static TwindyErrorDisplay retval;

	return &retval;
}

//------------------------------------------------------------------------------
void TwindyErrorDisplay::buttonClicked(Button *button)
{
	if(button == ok)
	{
		errorArray.removeRange(0, 1);
		if(errorArray.size() == 0)
			ok->setVisible(false);
		repaint();
	}
}

//------------------------------------------------------------------------------
void TwindyErrorDisplay::paint(Graphics &g)
{
	//Font tempFont(Typeface(T("Bitstream Vera Sans"), true, false));
	//tempFont.setHeight(14.0f);
	Font tempFont(14.0f, Font::bold);

	if(errorArray.size() > 0)
	{
		//Draw background rectangle.
		g.setColour(backColour);
		g.fillRoundedRectangle(2, 2, (getWidth()-4), (getHeight()-4), 5);
		g.setColour(backColour.darker(0.4f));
		g.drawRoundedRectangle(2, 2, (getWidth()-4), (getHeight()-4), 5, 2);

		//Draw heading.
		g.setColour(textColour);
		g.setFont(tempFont);
		g.drawFittedText(errorArray[0]->heading,
				   		 4,
						 18,
						 (getWidth()-8),
						 25,
						 Justification::horizontallyCentred,
						 1);

		//Draw message.
		tempFont.setBold(false);
		g.setFont(tempFont);
		g.drawMultiLineText(errorArray[0]->message,
						    5,
						    48,
						    (getWidth()-12));
	}
}

//------------------------------------------------------------------------------
void TwindyErrorDisplay::resized()
{
	int xPos;

	xPos = ((getWidth()-100)/2);
	ok->setBounds(xPos, (getHeight()-30), 100, 25);
}

//------------------------------------------------------------------------------
void TwindyErrorDisplay::addErrorMessage(const String& heading,
										 const String& message)
{
	ErrorMessage *tempErr = new ErrorMessage;

	tempErr->heading = heading;
	tempErr->message = message;

	errorArray.add(tempErr);
	std::cout << "TwindyErrorDisplay: ";
	std::cout << static_cast<const char *>(heading) << ": ";
	std::cout << static_cast<const char *>(message) << std::endl;

	repaint();

	if(!ok->isVisible())
		ok->setVisible(true);
}

//------------------------------------------------------------------------------
TwindyErrorDisplay::TwindyErrorDisplay():
Component(T("Error Display"))
{
	addAndMakeVisible(ok = new DrawableTextButton(T("okButton"), TRANS("Ok!")));
	ok->setVisible(false);
	ok->addButtonListener(this);
}

//------------------------------------------------------------------------------
TwindyErrorDisplay::~TwindyErrorDisplay()
{
	
}