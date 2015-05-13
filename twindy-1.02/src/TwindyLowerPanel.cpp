//	TwindyLowerPanel.cpp - Component to hold a window in the lower panel.
//						   Copied from memory from Tracktion's properties panel.
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
#include "TwindyLowerPanel.h"
#include "TwindyRootWindow.h"

//------------------------------------------------------------------------------
TwindyLowerPanel::TwindyLowerPanel():
Component(T("Twindy Lower Panel")),
text(TRANS("(nothing currently selected)")),
textEnd(220.0f)
{
	addAndMakeVisible(closeButton = new ShapeButton(T("closeButton"),
													Colours::white,
													Colours::white,
													Colours::black));
	closeButton->addButtonListener(this);
	Path x;
	x.addLineSegment(0.0f, 0.0f, 10.0f, 10.0f, 3.0f);
	x.addLineSegment(10.0f, 0.0f, 0.0f, 10.0f, 3.0f);
	closeButton->setShape(x, false, true, true);
}

///-----------------------------------------------------------------------------
TwindyLowerPanel::~TwindyLowerPanel()
{
	deleteAllChildren();
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::paint(Graphics& g)
{
	Path patty;
	Font tempFont(13.0f, Font::bold);

	g.setFont(tempFont);

	patty.startNewSubPath(7.0f, 1.0f);
	patty.lineTo(textEnd, 1.0f);
	patty.quadraticTo((textEnd+5.0f),
					  0.0f,
					  (textEnd+5.0f),
					  7.5f);
	patty.quadraticTo((textEnd+5.0f),
					  15.0f,
					  (textEnd+10.0f),
					  15.0f);
	patty.lineTo((getWidth()-8), 15.0f);
	patty.quadraticTo((getWidth()-1),
					  15.0f,
					  (getWidth()-1),
					  23.0f);
	patty.lineTo((getWidth()-1), (getHeight()-8));
	patty.quadraticTo((getWidth()-1),
					  (getHeight()-1),
					  (getWidth()-8),
					  (getHeight()-1));
	patty.lineTo(7, (getHeight()-1));
	patty.quadraticTo(1,
					  (getHeight()-1),
					  1,
					  (getHeight()-8));
	patty.lineTo(1, 7);
	patty.quadraticTo(0,
					  0,
					  7,
					  1);
	patty.closeSubPath();

	g.setColour(backgroundColour);
    g.fillPath(patty);
	g.setColour(outlineColour);
    g.strokePath(patty, PathStrokeType(1.0f,
									   PathStrokeType::curved,
									   PathStrokeType::rounded));

	g.setColour(textColour);
	g.drawSingleLineText(text, 20, 15);
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::resized()
{
	closeButton->setBounds(6, 6, 15, 15);
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::actionListenerCallback(const String& message)
{
	TwindyRootWindow *parent = static_cast<TwindyRootWindow *>(getParentComponent());

	if(message == closeButton->getName())
		parent->closeLowerWindow();
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::buttonClicked(Button* button)
{
	TwindyRootWindow *parent = static_cast<TwindyRootWindow *>(getParentComponent());

	if(button == closeButton)
		parent->closeLowerWindow();
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::mouseMove(const MouseEvent& e)
{
	getParentComponent()->mouseMove(e);
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::setOutlineColour(Colour& col)
{
	outlineColour = col;
	closeButton->setColours(textColour, textColour, outlineColour);
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::setTextColour(Colour& col)
{
	textColour = col;
	closeButton->setColours(textColour, textColour, outlineColour);
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::setBackgroundColour(Colour& col)
{
	backgroundColour = col;
}

//----------------------------------------------------------------------------------------------
void TwindyLowerPanel::setText(const String& str)
{
	float x, y, width, height;
	GlyphArrangement glyphs;

	Font tempFont(13, Font::bold);

	text = str;

	//All this is just so we can find out how long the window's title is, in
	//pixels.
	glyphs.addLineOfText(tempFont,
						 text,
						 0,
						 0);
	glyphs.getBoundingBox(0, text.length(), x, y, width, height, true);

	textEnd = width + 25;
}

//----------------------------------------------------------------------------------------------
int TwindyLowerPanel::getWindowWidth()
{
	return (getWidth()-19);
}

//----------------------------------------------------------------------------------------------
int TwindyLowerPanel::getWindowHeight()
{
	return (getHeight()-28);
}

//----------------------------------------------------------------------------------------------
int TwindyLowerPanel::getWindowX()
{
	return (getX() + 8);
}

//----------------------------------------------------------------------------------------------
int TwindyLowerPanel::getWindowY()
{
	return (getY() + 21);
}
