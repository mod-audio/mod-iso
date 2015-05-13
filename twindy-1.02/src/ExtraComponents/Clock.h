//	Clock.h - Simple clock Component.
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

#ifndef CLOCK_H_
#define CLOCK_H_

#include "../juce_amalgamated.h"

///	Simple clock Component.
class Clock : public Component,
              private Timer
{
  public:
	///	Constructor.
	Clock();
	///	Destructor.
	~Clock();

	///	The timer callback used to update the time and the display.
	void timerCallback();

	///	Draws the time.
	void paint(Graphics& g);

	///	When the user clicks on the clock, we switch between displaying the time or the date.
	void mouseDown(const MouseEvent& e);

	///	Sets the font to use.
	void setFont(const Font& fnt);
	///	Sets the colour to draw the text (the background is always transparent).
	void setColour(const Colour& col);
	///	Sets whether the clock should display the time with seconds or not.
	void setIncludeSeconds(bool val);
	///	Sets wether the time should be displayed as 24hr or 12hr.
	void setUse24HourClock(bool val);
	///	Sets whether the month should be displayed first on the date.
	void setMonthDisplayedFirst(bool val);
	///	Sets the date display separator.
	void setDateSeparator(const String& val);

	///	Returns the font we're using.
	const Font& getFont() const {return font;};
	///	Returns the colour of the text we're using.
	const Colour& getColour() const {return colour;};
	///	Returns the current text.
	const String& getCurrentText() const {return text;};
	///	Returns whether the clock displays seconds or not.
	bool getIncludeSeconds() const {return includeSeconds;};
	///	Returns whether the time should be displayed as 24hr or 12hr.
	bool getUse24HourClock() const {return use24HourClock;};
	///	Returns whether the month should be displayed first on the date.
	bool getMonthDisplayedFirst() const {return monthDisplayedFirst;};
	///	Returns the date display separator.
	const String& getDateSeparator() const {return dateSeparator;};
  private:
	///	The text displaying the current time.
	String text;
	///	The font we draw the text with.
	Font font;
	///	The colour we draw the text as.
	Colour colour;

	///	Whether we're currently displaying the date (true) or the time (false).
	bool displayingDate;
	///	Whether to display seconds on the time display.
	bool includeSeconds;
	///	Whether the time should be displayed as 24hr or 12hr.
	bool use24HourClock;
	///	Whether the month should be displayed first on the date.
	bool monthDisplayedFirst;
	///	The date display separator.
	String dateSeparator;
};

#endif
