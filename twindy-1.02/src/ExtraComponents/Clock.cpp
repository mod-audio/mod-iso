//	Clock.cpp - Simple clock Component.
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

#include "Clock.h"
#include "../Utils.h"

#include <iostream>

//------------------------------------------------------------------------------
Clock::Clock()
    : Component(),
      text(T("...")),
      colour(Colours::white),
      displayingDate(false),
      includeSeconds(true),
      use24HourClock(false),
      monthDisplayedFirst(false),
      dateSeparator(T("/"))
{
    const Coordinates& coords(getCoordinates());

    font.setBold(true);
    font.setHeight(coords.topTabBarHeight*5/8);
    font.setTypefaceName(Font::getDefaultSansSerifFontName());

    startTimer(1000); // 1000 = 1 second.
}

//------------------------------------------------------------------------------
Clock::~Clock()
{
    stopTimer();
}

//------------------------------------------------------------------------------
void Clock::timerCallback()
{
    //Update text.
    if (! displayingDate)
    {
        //For some reason this doesn't work properly with the current version of JUCE?
        text = Time::getCurrentTime().toString(false, true, includeSeconds, use24HourClock);
    }
    else
    {
        text = T("");
        if (! monthDisplayedFirst)
        {
            text << Time::getCurrentTime().getDayOfMonth();
            text << dateSeparator;
            text << Time::getCurrentTime().getMonth()+1;
        }
        else
        {
            text << Time::getCurrentTime().getMonth()+1;
            text << dateSeparator;
            text << Time::getCurrentTime().getDayOfMonth();
        }
        text << dateSeparator;
        text << Time::getCurrentTime().getYear();
    }

    //Draw new time.
    repaint();
}

//------------------------------------------------------------------------------
void Clock::paint(Graphics& g)
{
    g.setFont(font);
    g.setColour(colour);
    g.drawFittedText(text, 0, getHeight()/16, getWidth()-getWidth()/16, getHeight(), Justification::centred, 1);
    //g.drawSingleLineText(text, 0, getHeight()/16 + getHeight()*5/8);
}

//------------------------------------------------------------------------------
void Clock::mouseDown(const MouseEvent& e)
{
    displayingDate = !displayingDate;

    // Update text.
    timerCallback();
}

//------------------------------------------------------------------------------
void Clock::setFont(const Font& fnt)
{
    font = fnt;
}

//------------------------------------------------------------------------------
void Clock::setColour(const Colour& col)
{
    colour = col;
}

//------------------------------------------------------------------------------
void Clock::setIncludeSeconds(bool val)
{
    includeSeconds = val;
}

//------------------------------------------------------------------------------
void Clock::setUse24HourClock(bool val)
{
    use24HourClock = val;
}

//------------------------------------------------------------------------------
void Clock::setMonthDisplayedFirst(bool val)
{
    monthDisplayedFirst = val;
}

//------------------------------------------------------------------------------
void Clock::setDateSeparator(const String& val)
{
    dateSeparator = val;
}
