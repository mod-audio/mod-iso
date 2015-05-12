//	TwindyToggleButton.cpp - ToggleButton subclass which lets you set the colour
//						     of the text.
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
#include "TwindyToggleButton.h"

//------------------------------------------------------------------------------
TwindyToggleButton::TwindyToggleButton(const String& text):
ToggleButton(text)
{
	
}

//------------------------------------------------------------------------------
TwindyToggleButton::~TwindyToggleButton()
{
	
}

//------------------------------------------------------------------------------
void TwindyToggleButton::setColours(const Colour& backCol,
									const Colour& textCol)
{
	backgroundColour = backCol;
	textColour = textCol;
	repaint();
}

//------------------------------------------------------------------------------
void TwindyToggleButton::paintButton(Graphics& g,
									 bool isMouseOverButton,
									 bool isButtonDown)
{
	const float tickWidth = (float) jmin (20, getHeight() - 4);
	Colour rectCol = backgroundColour.contrasting().withAlpha(0.25f);

    Path box;
    box.addRoundedRectangle (0.0f, 2.0f, 6.0f, 6.0f, 1.0f);

    if (isEnabled())
        g.setColour (rectCol.withAlpha (isDown() ? 0.3f : 0.1f));
    else
        g.setColour (Colours::lightgrey.withAlpha (0.1f));

    AffineTransform trans (AffineTransform::identity.scaled (tickWidth / 9.0f,
                                                             tickWidth / 9.0f)
                                                    .translated (4.0f,
                                                                 (getHeight() - tickWidth) * 0.5f));

    g.fillPath (box, trans);

    g.setColour (rectCol.darker(0.5f));
    g.strokePath (box, PathStrokeType (0.9f), trans);

    if (getToggleState())
    {
        Path tick;
        tick.startNewSubPath (1.5f, 3.0f);
        tick.lineTo (3.0f, 6.0f);
        tick.lineTo (6.0f, 0.0f);

        if (isEnabled())
            g.setColour (textColour);
        else
            g.setColour (Colours::grey);

        g.strokePath (tick, PathStrokeType (2.5f), trans);
    }

    g.setColour (textColour);
    g.setFont (jmin (15.0f, getHeight() * 0.6f));

    if (! isEnabled())
        g.setOpacity (0.5f);

    const int textX = roundFloatToInt (tickWidth + 4);

    g.drawFittedText (getButtonText(),
                      textX, 6,
                      getWidth() - textX, getHeight() - 8,
                      Justification::centredLeft, 10);
}
