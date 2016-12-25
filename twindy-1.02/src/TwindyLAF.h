//	TwindyLAF.h - Custom LookAndFeel, mainly just to use a nicer typeface at the
//				  moment.
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

#ifndef TWINDYLAF_H_
#define TWINDYLAF_H_

#include "juce_amalgamated.h"

///	Custom LookAndFeel, mainly so we can use a nicer Typeface.
class TwindyLAF : public LookAndFeel
{
  public:
	///	Constructor.
	TwindyLAF();

	///	Over-ridden so we can control the colour of our DrawableTextButton's backgrounds.
	void drawButtonBackground(Graphics &g,
							  Button &button,
							  const Colour &backgroundColour,
							  bool isMouseOverButton,
							  bool isButtonDown);

	///	Over-ridden so we can have a decent-sized font.
	void drawPopupMenuItem(Graphics& g,
                           int width, int height,
                           const bool isSeparator,
                           const bool isActive,
                           const bool isHighlighted,
                           const bool isTicked,
                           const bool hasSubMenu,
                           const String& text,
                           const String& shortcutKeyText,
                           Image* image,
                           const Colour* const textColourToUse);

	///	Used to draw an old-style TabbedComponent.
	void createTabButtonShape(Path &p,
							  int width,
							  int height,
							  int tabIndex,
							  const String &text,
							  Button &button,
							  TabbedButtonBar::Orientation orientation,
							  const bool isMouseOver,
							  const bool isMouseDown,
							  const bool isFrontTab);

	///	Used to draw an old-style TabbedComponent.
	void drawTabButtonText(Graphics &g,
						   int x,
						   int y,
						   int w,
						   int h,
						   const Colour &preferredBackgroundColour,
						   int tabIndex,
						   const String &text,
						   Button &button,
						   TabbedButtonBar::Orientation orientation,
						   const bool isMouseOver,
						   const bool isMouseDown,
						   const bool isFrontTab);

	///	Used to draw an old-style TabbedComponent.
	void drawTabButton(Graphics &g,
					   int w,
					   int h,
					   const Colour &preferredColour,
					   int tabIndex,
					   const String &text,
					   Button &button,
					   TabbedButtonBar::Orientation orientation,
					   const bool isMouseOver,
					   const bool isMouseDown,
					   const bool isFrontTab);

	///	Used to draw a non-gradiented title bar on the file dialogs.
	void drawDocumentWindowTitleBar(DocumentWindow& window,
                                    Graphics& g,
									int w,
									int h,
                                    int titleSpaceX,
									int titleSpaceW,
                                    const Image* icon,
                                    bool drawTitleTextOnLeft);

    const Font getAlertWindowFont();
    const Font getComboBoxFont(ComboBox& box);
    const Font getPopupMenuFont();


private:
      const double scaling;
};

#endif
