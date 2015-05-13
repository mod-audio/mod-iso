//	MiscPreferences.cpp - Preferences page for miscellaneous options.
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
#include "MiscPreferences.h"
#include "TwindyApp.h"
#include "TwindyProperties.h"
#include "TwindyRootWindow.h"
#include "TwindyToggleButton.h"
#include "DrawableTextButton.h"
#include "Clock.h"
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
MiscPreferences::MiscPreferences():
Component()
{
	int i;
	TwindyProperty tempProp;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();
	TwindyProperties *prop = root->getProperties();

	Font::findFonts(fonts);

	//Fonts list.
	addAndMakeVisible(fontListLabel = new Label(T("fL"),
												TRANS("Global Twindy Font (restart req.):")));
	fontListLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(fontList = new ListBox(T("Fonts"), this));

	tempProp = prop->getProperty(T("GlobalFont"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("Bitstream Vera Sans");
		tempProp.value = T("Bitstream Vera Sans");
		prop->setProperty(T("GlobalFont"), tempProp);
	}

	for(i=0;i<fonts.size();++i)
	{
		if(fonts[i]->getTypefaceName() == tempProp.name)
			break;
	}
	if(i < fonts.size())
	{
		fontList->updateContent();
		fontList->selectRow(i);
	}

	//Clock options.
	addAndMakeVisible(includeSeconds = new TwindyToggleButton(TRANS("Time includes seconds")));
	includeSeconds->addButtonListener(this);
	addAndMakeVisible(use24HourClock = new TwindyToggleButton(TRANS("24 Hour time")));
	use24HourClock->addButtonListener(this);
	addAndMakeVisible(monthDisplayedFirst = new TwindyToggleButton(TRANS("Month displayed first")));
	monthDisplayedFirst->addButtonListener(this);
	addAndMakeVisible(dateSepLabel = new Label(T("dSL"), TRANS("Date Separator:")));
	dateSepLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(dateSeparator = new TextEditor(T("dS")));
	dateSeparator->addListener(this);

	//includeSeconds property.
	tempProp = prop->getProperty(T("ClockIncludeSeconds"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("ClockIncludeSeconds");
		tempProp.value = T("TRUE");
		prop->setProperty(T("ClockIncludeSeconds"), tempProp);
	}
	if(tempProp.value == T("TRUE"))
		includeSeconds->setToggleState(true, false);
	else
		includeSeconds->setToggleState(false, false);
	//use24HourClock property.
	tempProp = prop->getProperty(T("ClockUse24HourClock"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("ClockUse24HourClock");
		tempProp.value = T("FALSE");
		prop->setProperty(T("ClockUse24HourClock"), tempProp);
	}
	if(tempProp.value == T("TRUE"))
		use24HourClock->setToggleState(true, false);
	else
		use24HourClock->setToggleState(false, false);
	//monthDisplayedFirst property.
	tempProp = prop->getProperty(T("ClockMonthDisplayedFirst"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("ClockMonthDisplayedFirst");
		tempProp.value = T("FALSE");
		prop->setProperty(T("ClockMonthDisplayedFirst"), tempProp);
	}
	if(tempProp.value == T("TRUE"))
		monthDisplayedFirst->setToggleState(true, false);
	else
		monthDisplayedFirst->setToggleState(false, false);
	//dateSeparator property.
	tempProp = prop->getProperty(T("ClockMonthDateSeparator"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("ClockMonthDateSeparator");
		tempProp.value = T("/");
		prop->setProperty(T("ClockMonthDateSeparator"), tempProp);
	}
	dateSeparator->setText(tempProp.value);

	//Localisation file controls.
	addAndMakeVisible(localisationLabel = new Label(T("lL"),
												TRANS("Localisation file (restart req.):")));
	localisationLabel->setFont(Font(16.0f, Font::bold));

	addAndMakeVisible(localisationFile = new TextEditor(T("lF")));
	localisationFile->addListener(this);

	addAndMakeVisible(localisationDialog = new DrawableTextButton(T("..."),
																  T("...")));
	localisationDialog->addButtonListener(this);

	//localisationFile property.
	tempProp = prop->getProperty(T("LocalisationFile"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("LocalisationFile");
		tempProp.value = T("");
		prop->setProperty(T("LocalisationFile"), tempProp);
	}
	localisationFile->setText(tempProp.value);
}

//------------------------------------------------------------------------------
MiscPreferences::~MiscPreferences()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void MiscPreferences::resized()
{
	fontListLabel->setBounds(10, 0, 250, 20);
	fontList->setBounds(10, 25, 250, 280);
	includeSeconds->setBounds(275, 20, 175, 25);
	use24HourClock->setBounds(275, 45, 175, 25);
	monthDisplayedFirst->setBounds(275, 70, 175, 25);
	dateSepLabel->setBounds(275, 100, 125, 20);
	dateSeparator->setBounds(405, 100, 50, 20);
	localisationLabel->setBounds(10, 315, 210, 20);
	localisationFile->setBounds(225, 315, 200, 20);
	localisationDialog->setBounds(430, 313, 30, 25);
}

//------------------------------------------------------------------------------
void MiscPreferences::buttonClicked(Button *button)
{
	TwindyProperty tempProp;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();
	TwindyProperties *prop = root->getProperties();

	if(button == includeSeconds)
	{
		tempProp.name = T("ClockIncludeSeconds");
		if(button->getToggleState())
		{
			tempProp.value = T("TRUE");
			root->getClock()->setIncludeSeconds(true);
		}
		else
		{
			tempProp.value = T("FALSE");
			root->getClock()->setIncludeSeconds(false);
		}
		prop->setProperty(T("ClockIncludeSeconds"), tempProp);
	}
	else if(button == use24HourClock)
	{
		tempProp.name = T("ClockUse24HourClock");
		if(button->getToggleState())
		{
			tempProp.value = T("TRUE");
			root->getClock()->setUse24HourClock(true);
		}
		else
		{
			tempProp.value = T("FALSE");
			root->getClock()->setUse24HourClock(false);
		}
		prop->setProperty(T("ClockUse24HourClock"), tempProp);
	}
	else if(button == monthDisplayedFirst)
	{
		tempProp.name = T("ClockMonthDisplayedFirst");
		if(button->getToggleState())
		{
			tempProp.value = T("TRUE");
			root->getClock()->setMonthDisplayedFirst(true);
		}
		else
		{
			tempProp.value = T("FALSE");
			root->getClock()->setMonthDisplayedFirst(false);
		}
		prop->setProperty(T("ClockMonthDisplayedFirst"), tempProp);
	}
	else if(button == localisationDialog)
	{
		FileChooser dlg(TRANS("Open localisation file..."),
						File::nonexistent,
						String::empty,
						false);

		if(dlg.browseForFileToOpen())
		{
			LocalisedStrings *tempStrings = new LocalisedStrings(dlg.getResult());
			localisationFile->setText(dlg.getResult().getFullPathName());
			LocalisedStrings::setCurrentMappings(tempStrings);
			saveChanges();

			cout << "  loc: Setting text to " << (const char *)dlg.getResult().getFullPathName() << endl;
			cout << "  loc: language = " << (const char *)tempStrings->getLanguageName() << endl;
			cout << "  loc: current language = " << (const char *)LocalisedStrings::getCurrentMappings()->getLanguageName() << endl;
		}
	}

	root->getClock()->repaint();
}

//------------------------------------------------------------------------------
void MiscPreferences::textEditorReturnKeyPressed(TextEditor &editor)
{
	if(editor.getName() == T("lF"))
	{
		if(localisationFile->getText() == T(""))
		{
			cout << "  loc: Resetting text." << endl;
			LocalisedStrings::setCurrentMappings(0);
		}
		else
		{
			LocalisedStrings *tempStrings = new LocalisedStrings(File(localisationFile->getText()));
			LocalisedStrings::setCurrentMappings(tempStrings);
			cout << "  loc: Setting text to" << (const char *)localisationFile->getText() << endl;
		}
	}
	saveChanges();
}

//------------------------------------------------------------------------------
void MiscPreferences::paintListBoxItem(int rowNumber,
									   Graphics &g,
									   int width,
									   int height,
									   bool rowIsSelected)
{
	if(rowIsSelected)
	{
		Colour tempCol = backgroundColour.contrasting();
		if(tempCol.brighter(0.5f) == Colours::white)
			tempCol = backgroundColour;
		g.setColour(tempCol.withAlpha(0.2f));
		g.fillRect(0, 0, width, 25);
	}

	Font *f = fonts[rowNumber];

	if(f != 0)
	{
		f->setHeight(height * 0.7f);
		f->setBold(false);
		f->setItalic(false);

		g.setFont(*f);
		g.setColour(Colours::black);

		g.drawText(f->getTypefaceName(), 
                   4,
				   0,
				   (width-4),
				   height,
				   Justification::centredLeft,
				   true);
	}
}

//------------------------------------------------------------------------------
void MiscPreferences::selectedRowsChanged(int lastRowselected)
{
	TwindyProperty tempProp;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();
	TwindyProperties *prop = root->getProperties();

	tempProp.name = fonts[lastRowselected]->getTypefaceName();
	tempProp.value = fonts[lastRowselected]->getTypefaceName();
	prop->setProperty(T("GlobalFont"), tempProp);
	//Font::setDefaultSansSerifFontName(tempProp.name);
	LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(tempProp.name);
	root->repaint();
}

//------------------------------------------------------------------------------
void MiscPreferences::paintOverChildren(Graphics &g)
{
	//Fonts.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(8, 23, 254, 284, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(9, 24, 252, 282, 1);

	//Clock group.
	g.setOrigin(270, 3);
	drawGroupComponentOutline(g,
							  200,
							  130,
							  TRANS("Clock Options"),
							  Justification::left);
	g.setOrigin(-270, -3);

	//Date separator.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(403, 98, 54, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(404, 99, 52, 22, 1);

	//Localisation File.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(223, 313, 204, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(224, 314, 202, 22, 1);
}

//------------------------------------------------------------------------------
void MiscPreferences::setColours(const Colour& backCol,
								 const Colour& textCol,
								 const Colour& buttonCol)
{
	backgroundColour = backCol;
	textColour = textCol;

	fontListLabel->setColour(Label::textColourId, textColour);
	includeSeconds->setColours(backgroundColour, textColour);
	use24HourClock->setColours(backgroundColour, textColour);
	monthDisplayedFirst->setColours(backgroundColour, textColour);
	dateSepLabel->setColour(Label::textColourId, textColour);
	localisationLabel->setColour(Label::textColourId, textColour);
}

//------------------------------------------------------------------------------
void MiscPreferences::saveChanges()
{
	String tempstr;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if(root)
	{
		TwindyProperties *prop = root->getProperties();

		TwindyProperty tempProp;
		tempProp.name = T("ClockMonthDateSeparator");
		tempProp.value = dateSeparator->getText();
		prop->setProperty(T("ClockMonthDateSeparator"), tempProp);
		root->getClock()->setDateSeparator(tempProp.value);
		root->getClock()->repaint();

		tempProp.name = T("LocalisationFile");
		tempProp.value = localisationFile->getText();
		prop->setProperty(T("LocalisationFile"), tempProp);
	}
}

//------------------------------------------------------------------------------
void MiscPreferences::drawGroupComponentOutline(Graphics& g,
												int width,
												int height,
                                             	const String& text,
                                             	const Justification& position)
{
    const float textH = 15.0f;
    const float indent = 3.0f;
    const float textEdgeGap = 4.0f;
    float cs = 5.0f;

    Font f (textH, Font::bold);

    Path p;
    float x = indent;
    float y = f.getAscent() - 3.0f;
    float w = width - x * 2.0f;
    float h = height - y  - indent;
    cs = jmin (cs, w * 0.5f, h * 0.5f);
    const float cs2 = 2.0f * cs;

    float textW = jlimit (0.0f, f.getStringWidth (text) + textEdgeGap * 2.0f, w - cs2);
    float textX = cs + textEdgeGap;

    if (position.testFlags (Justification::verticallyCentred))
        textX = cs + (w - cs2 - textW) * 0.5f;
    else if (position.testFlags (Justification::right))
        textX = w - cs - textW - textEdgeGap;

    p.startNewSubPath (x + textX + textW, y);
    p.lineTo (x + w - cs, y);

    p.addArc (x + w - cs2, y, cs2, cs2, 0, float_Pi * 0.5f);
    p.lineTo (x + w, y + h - cs);

    p.addArc (x + w - cs2, y + h - cs2, cs2, cs2, float_Pi * 0.5f, float_Pi);
    p.lineTo (x + cs, y + h);

    p.addArc (x, y + h - cs2, cs2, cs2, float_Pi, float_Pi * 1.5f);
    p.lineTo (x, y + cs);

    p.addArc (x, y, cs2, cs2, float_Pi * 1.5f, float_Pi * 2.0f);
    p.lineTo (x + textX, y);

    g.setColour (textColour);
    g.strokePath (p, PathStrokeType (2.0f));

    g.setColour (textColour);
    g.setFont (f);
    g.drawText (text,
                roundFloatToInt (x + textX), 0,
                roundFloatToInt (textW),
                roundFloatToInt (textH),
                Justification::centred, true);
}
