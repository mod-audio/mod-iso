//	TwindyLowerPanel.h - Component to hold a window in the lower panel.  Copied
//						 from memory from Tracktion's properties panel.
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

#ifndef TWINDYLOWERPANEL_H_
#define TWINDYLOWERPANEL_H_

///	Component to hold a window in the lower panel.
/*!
	Based on Tracktion's properties panel.
 */
class TwindyLowerPanel : public JUCE_NAMESPACE::Component,
						 public JUCE_NAMESPACE::ActionListener,
						 public JUCE_NAMESPACE::ButtonListener
{
  public:
	///	Constructor.
	TwindyLowerPanel();
	///	Destructor.
	~TwindyLowerPanel();

	///	Draws the panel.
	void paint(JUCE_NAMESPACE::Graphics& g);

	///	Overridden to place the close button correctly.
	void resized();

	///	So we're notified when the close button is clicked.
	/*!
		This probably isn't used anymore, now that we're using ButtonListeners.
	 */
	void actionListenerCallback(const JUCE_NAMESPACE::String& message);
	///	So we're notified when the close button is clicked.
	void buttonClicked(JUCE_NAMESPACE::Button* button);

	///	We override this to pass the event on to TwindyRootWindow.
	/*!
		We need to pass it on to TwindyRootWindow because we want to give the
		current window focus when the mouse is over it.
	 */
	void mouseMove(const JUCE_NAMESPACE::MouseEvent& e);

	///	Sets the Outline colour.
	void setOutlineColour(JUCE_NAMESPACE::Colour& col);
	///	Sets the Text colour.
	void setTextColour(JUCE_NAMESPACE::Colour& col);
	///	Sets the Background colour.
	void setBackgroundColour(JUCE_NAMESPACE::Colour& col);
	///	Sets the text at the top of the panel.
	void setText(const JUCE_NAMESPACE::String& str);

	///	Returns the area within which the window should be displayed (width).
	int getWindowWidth();
	///	Returns the area within which the window should be displayed (height).
	int getWindowHeight();
	///	Returns the top left position at which the window should be displayed (x).
	int getWindowX();
	///	Returns the top left position at which the window should be displayed (y).
	int getWindowY();
	///	Returns the title text.
	const JUCE_NAMESPACE::String& getTitle() const {return text;};
  private:
	///	The button used to close the window in the lower panel.
	JUCE_NAMESPACE::ShapeButton *closeButton;

	///	Colour of the outline.
	JUCE_NAMESPACE::Colour outlineColour;
	///	Colour of the text.
	JUCE_NAMESPACE::Colour textColour;
	///	Colour of the background.
	JUCE_NAMESPACE::Colour backgroundColour;

	///	The text to display (usually the name of the program running).
	JUCE_NAMESPACE::String text;

	///	The furthest right point of the 'tab' at the top of the panel.
	float textEnd;
};

#endif
