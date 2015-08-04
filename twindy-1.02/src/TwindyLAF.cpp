//	TwindyLAF.cpp - Custom LookAndFeel, mainly just to use a nicer typeface at
//				    the moment.
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
#include "TwindyLAF.h"

//------------------------------------------------------------------------------
TwindyLAF::TwindyLAF():
LookAndFeel()
{

}

//------------------------------------------------------------------------------
void TwindyLAF::drawButtonBackground(Graphics &g,
							  Button &button,
							  const Colour &backgroundColour,
							  bool isMouseOverButton,
							  bool isButtonDown)
{
	const int width = button.getWidth();
    const int height = button.getHeight();

    const float indent = 2.0f;
    const int cornerSize = jmin (roundFloatToInt (width * 0.4f),
                                 roundFloatToInt (height * 0.4f));

    Path p;
    p.addRoundedRectangle (indent, indent,
                           width - indent * 2.0f,
                           height - indent * 2.0f,
                           (float) cornerSize);

    //Colour bc (backgroundColour.withMultipliedSaturation (0.3f));
    Colour bc(backgroundColour);

    if (isMouseOverButton)
    {
        if (isButtonDown)
            bc = bc.brighter();
        else if (bc.getBrightness() > 0.5f)
            bc = bc.darker (0.1f);
        else
            bc = bc.brighter (0.1f);
    }

    g.setColour (bc);

    g.fillPath (p);

    g.setColour (bc.contrasting().withAlpha ((isMouseOverButton) ? 0.6f : 0.4f));
    g.strokePath (p, PathStrokeType ((isMouseOverButton) ? 2.0f : 1.4f));
}

//------------------------------------------------------------------------------
void TwindyLAF::drawPopupMenuItem(Graphics& g,
                                  int width, int height,
                                  const bool isSeparator,
                                  const bool isActive,
                                  const bool isHighlighted,
                                  const bool isTicked,
                                  const bool hasSubMenu,
                                  const String& text,
                                  const String& shortcutKeyText,
                                  Image* image,
                                  const Colour* const textColourToUse)
{
	const float halfH = height * 0.5f;

    if (isSeparator)
    {
        const float separatorIndent = 5.5f;

        g.setColour (Colours::black.withAlpha (0.2f));
        g.drawLine (separatorIndent, halfH, width - separatorIndent, halfH);

        g.setColour (Colours::white.withAlpha (0.4f));
        g.drawLine (separatorIndent, halfH + 1.0f, width - separatorIndent, halfH + 1.0f);
    }
    else
    {
        Colour textColour (findColour (PopupMenu::textColourId));

        if (textColourToUse != 0)
            textColour = *textColourToUse;

        if (isHighlighted)
        {
            g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
            g.fillRect (1, 1, width - 2, height - 2);
            g.setColour (findColour (PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour);
        }

        if (! isActive)
            g.setOpacity (0.3f);

        g.setFont (getPopupMenuFont());
        const int leftBorder = (height * 5) / 4;
        const int rightBorder = 4;

		g.setFont(14.0f);

        if (image != 0)
        {
            g.drawImageWithin (image,
                               2, 1, leftBorder - 4, height - 2,
                               RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, false);
        }
        else if (isTicked)
        {
            const Path tick (getTickShape (1.0f));
            const float th = g.getCurrentFont().getAscent();
            const float ty = halfH - th * 0.5f;

            g.fillPath (tick, tick.getTransformToScaleToFit (2.0f, ty, (float) (leftBorder - 4),
                                                             th, true));
        }

        g.drawText (text,
                    leftBorder,
                    0,
                    width - (leftBorder + rightBorder),
                    height,
                    Justification::centredLeft,
                    true);

        if (shortcutKeyText.isNotEmpty())
        {
            Font f2 (g.getCurrentFont());
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (shortcutKeyText,
                        leftBorder,
                        0,
                        width - (leftBorder + rightBorder + 4),
                        height,
                        Justification::centredRight,
                        true);
        }

        if (hasSubMenu)
        {
            const float x = width - height * 0.6f;

            Path p;
            p.addTriangle (x, height * 0.3f,
                           x, height * 0.7f,
                           x + height * 0.3f,
                           halfH);

            g.fillPath (p);
        }
    }
}

//------------------------------------------------------------------------------
void TwindyLAF::createTabButtonShape(Path &p,
								 	 int width,
							 		 int height,
							 		 int tabIndex,
							 		 const String &text,
							 		 Button &button,
							 		 TabbedButtonBar::Orientation orientation,
							 		 const bool isMouseOver,
							 		 const bool isMouseDown,
							 		 const bool isFrontTab)
{
	const float w = (float) width;
    const float h = (float) height;

    //float length = w;
    float depth = h;

    /*if (orientation == TabbedButtonBar::TabsAtLeft
         || orientation == TabbedButtonBar::TabsAtRight)
    {
        swapVariables (length, depth);
    }*/

    const float indent = (float) getTabButtonOverlap ((int) depth);
    const float overhang = 4.0f;

    /*if (orientation == TabbedButtonBar::TabsAtLeft)
    {
        p.startNewSubPath (w, 0.0f);
        p.lineTo (0.0f, indent);
        p.lineTo (0.0f, h - indent);
        p.lineTo (w, h);
        p.lineTo (w + overhang, h + overhang);
        p.lineTo (w + overhang, -overhang);
    }
    else */if (orientation == TabbedButtonBar::TabsAtRight)
    {
        p.startNewSubPath (0.0f, 0.0f);
        p.lineTo (w, indent);
        p.lineTo (w, h - indent);
        p.lineTo (0.0f, h);
        p.lineTo (-overhang, h + overhang);
        p.lineTo (-overhang, -overhang);
    }
    else if (orientation == TabbedButtonBar::TabsAtBottom)
    {
        p.startNewSubPath (0.0f, 0.0f);
        p.lineTo (indent, h);
        p.lineTo (w - indent, h);
        p.lineTo (w, 0.0f);
        p.lineTo (w + overhang, -overhang);
        p.lineTo (-overhang, -overhang);
    }
    else
    {
        p.startNewSubPath (0.0f, h);
        p.lineTo (indent, 0.0f);
        p.lineTo (w - indent, 0.0f);
        p.lineTo (w, h);
        p.lineTo (w + overhang, h + overhang);
        p.lineTo (-overhang, h + overhang);
    }

    p.closeSubPath();

    p = p.createPathWithRoundedCorners (3.0f);
}

//------------------------------------------------------------------------------
void TwindyLAF::drawTabButtonText(Graphics &g,
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
						  		  const bool isFrontTab)
{
	int length = w;
    int depth = h;

    /*if (orientation == TabbedButtonBar::TabsAtLeft
            || orientation == TabbedButtonBar::TabsAtRight)
    {
        swapVariables (length, depth);
    }*/

    GlyphArrangement textLayout;
    textLayout.addFittedText (Font (depth * 0.6f),
                              text,
                              0.0f, 0.0f, (float) length, (float) depth,
                              Justification::centred,
                              jmax (1, depth / 12));

    AffineTransform transform;

    /*if (orientation == TabbedButtonBar::TabsAtLeft)
    {
        transform = transform.rotated (float_Pi * -0.5f)
                             .translated ((float) x, (float) (y + h));
    }
    else */if (orientation  == TabbedButtonBar::TabsAtRight)
    {
        transform = transform.rotated (float_Pi * 0.5f)
                             .translated ((float) (x + w), (float) y);
    }
    else
    {
        transform = transform.translated ((float) x, (float) y);
    }

    g.setColour (preferredBackgroundColour.contrasting());

    if (! (isMouseOver || isMouseDown))
        g.setOpacity (0.8f);

    if (! button.isEnabled())
        g.setOpacity (0.3f);

    textLayout.draw (g, transform);
}

//------------------------------------------------------------------------------
void TwindyLAF::drawTabButton(Graphics &g,
					   		  int w,
					   		  int h,
					 		  const Colour &preferredColour,
					 		  int tabIndex,
					 		  const String &text,
					 		  Button &button,
					 		  TabbedButtonBar::Orientation orientation,
					 		  const bool isMouseOver,
					 		  const bool isMouseDown,
					 		  const bool isFrontTab)
{
	//int length = w;
    int depth = h;

    /*if (orientation == TabbedButtonBar::TabsAtLeft
            || orientation == TabbedButtonBar::TabsAtRight)
    {
        swapVariables (length, depth);
    }*/

    Path tabShape;

    createTabButtonShape (tabShape, w, h,
                          tabIndex, text, button, orientation,
                          isMouseOver, isMouseDown, isFrontTab);

    fillTabButtonShape (g, tabShape, preferredColour,
                        tabIndex, text, button, orientation,
                        isMouseOver, isMouseDown, isFrontTab);

    const int indent = getTabButtonOverlap (depth);
    int x = 0, y = 0;

    if (orientation == TabbedButtonBar::TabsAtLeft
         || orientation == TabbedButtonBar::TabsAtRight)
    {
        y += indent;
        h -= indent * 2;
    }
    else
    {
        x += indent;
        w -= indent * 2;
    }

    drawTabButtonText (g, x, y, w, h, preferredColour,
                       tabIndex, text, button, orientation,
                       isMouseOver, isMouseDown, isFrontTab);
}



//------------------------------------------------------------------------------
void TwindyLAF::drawDocumentWindowTitleBar(DocumentWindow& window,
                                           Graphics& g,
										   int w,
										   int h,
                                           int titleSpaceX,
										   int titleSpaceW,
                                           const Image* icon,
                                           bool drawTitleTextOnLeft)
{
    const bool isActive = window.isActiveWindow();

    /*GradientBrush gb (window.getBackgroundColour(),
                      0.0f, 0.0f,
                      window.getBackgroundColour().contrasting (isActive ? 0.15f : 0.05f),
                      0.0f, (float) h, false);
    g.setBrush (&gb);
    g.fillAll();*/
	g.drawLine(2, h, (window.getWidth()-4), h, 1);

    g.setFont (h * 0.65f, Font::bold);

    int textW = g.getCurrentFont().getStringWidth (window.getName());
    int iconW = 0;
    int iconH = 0;

    if (icon != 0)
    {
        iconH = (int) g.getCurrentFont().getHeight();
        iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
    }

    textW = jmin (titleSpaceW, textW + iconW);
    int textX = drawTitleTextOnLeft ? titleSpaceX
                                    : jmax (titleSpaceX, (w - textW) / 2);

    if (textX + textW > titleSpaceX + titleSpaceW)
        textX = titleSpaceX + titleSpaceW - textW;

    if (icon != 0)
    {
        g.setOpacity (isActive ? 1.0f : 0.6f);
        g.drawImageWithin (icon, textX, (h - iconH) / 2, iconW, iconH,
                           RectanglePlacement::centred, false);
        textX += iconW;
        textW -= iconW;
    }

    g.setColour (window.getBackgroundColour().contrasting (isActive ? 0.7f : 0.4f));
    g.drawText (window.getName(), textX, 0, textW, h, Justification::centredLeft, true);
}
