//	AboutPage.cpp - All about Twindy.
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
#include "AboutPage.h"

const String AboutPage::version = T("1.01");

//------------------------------------------------------------------------------
AboutPage::AboutPage():
Component()
{
	String tempstr;
	Font tempFont(26.0f);

	tempstr = T("Twindy v") + version;
	addAndMakeVisible(title = new Label(T("title"), tempstr));
	tempFont.setHeight(28.0f);
	title->setFont(tempFont);
	/*title->setColours(textColour,
					  Colours::transparentBlack,
					  Colours::transparentBlack,
					  Colours::transparentBlack,
					  Colours::transparentBlack,
					  Colours::transparentBlack);*/
	title->setColour(Label::textColourId, textColour);

	tempstr = SystemStats::getJUCEVersion();
	addAndMakeVisible(juceVersionLabel = new Label(T("juceVersion"), tempstr));
	tempFont.setHeight(12.0f);
	juceVersionLabel->setFont(tempFont);
	/*juceVersionLabel->setColours(textColour,
								 Colours::transparentBlack,
								 Colours::transparentBlack,
								 Colours::transparentBlack,
								 Colours::transparentBlack,
								 Colours::transparentBlack);*/
	juceVersionLabel->setColour(Label::textColourId, textColour);

	tempstr = T("Author:");
	addAndMakeVisible(authorLabel = new Label(T("authorLabel"), tempstr));
	tempFont.setBold(true);
	tempFont.setHeight(14.0f);
	authorLabel->setFont(tempFont);
	/*authorLabel->setColours(textColour,
						    Colours::transparentBlack,
						    Colours::transparentBlack,
						    Colours::transparentBlack,
						    Colours::transparentBlack,
						    Colours::transparentBlack);*/
	authorLabel->setColour(Label::textColourId, textColour);

	tempstr = T("Website:");
	addAndMakeVisible(websiteLabel = new Label(T("websiteLabel"), tempstr));
	websiteLabel->setFont(tempFont);
	/*websiteLabel->setColours(textColour,
						     Colours::transparentBlack,
						     Colours::transparentBlack,
						     Colours::transparentBlack,
						     Colours::transparentBlack,
						     Colours::transparentBlack);*/
	websiteLabel->setColour(Label::textColourId, textColour);

	tempstr = T("Credits:");
	addAndMakeVisible(creditsLabel = new Label(T("creditsLabel"), tempstr));
	creditsLabel->setFont(tempFont);
	/*creditsLabel->setColours(textColour,
						     Colours::transparentBlack,
						     Colours::transparentBlack,
						     Colours::transparentBlack,
						     Colours::transparentBlack,
						     Colours::transparentBlack);*/
	creditsLabel->setColour(Label::textColourId, textColour);

	tempstr = T("About:");
	addAndMakeVisible(descriptionLabel = new Label(T("descriptionLabel"), tempstr));
	descriptionLabel->setFont(tempFont);
	/*descriptionLabel->setColours(textColour,
							     Colours::transparentBlack,
							     Colours::transparentBlack,
							     Colours::transparentBlack,
							     Colours::transparentBlack,
							     Colours::transparentBlack);*/
	descriptionLabel->setColour(Label::textColourId, textColour);

	tempstr = T("License:");
	addAndMakeVisible(licenseLabel = new Label(T("licenseLabel"), tempstr));
	licenseLabel->setFont(tempFont);
	/*licenseLabel->setColours(textColour,
						   Colours::transparentBlack,
						   Colours::transparentBlack,
						   Colours::transparentBlack,
						   Colours::transparentBlack,
						   Colours::transparentBlack);*/
	licenseLabel->setColour(Label::textColourId, textColour);

	tempstr = T("Niall Moody");
	addAndMakeVisible(author = new Label(T("author"), tempstr));
	tempFont.setBold(false);
	author->setFont(tempFont);
	/*author->setColours(textColour,
					   Colours::transparentBlack,
					   Colours::transparentBlack,
					   Colours::transparentBlack,
					   Colours::transparentBlack,
					   Colours::transparentBlack);*/
	author->setColour(Label::textColourId, textColour);

	tempstr = T("http://www.niallmoody.com/twindy/");
	addAndMakeVisible(website = new Label(T("website"), tempstr));
	website->setFont(tempFont);
	/*website->setColours(textColour,
					    Colours::transparentBlack,
					    Colours::transparentBlack,
					    Colours::transparentBlack,
					    Colours::transparentBlack,
					    Colours::transparentBlack);*/
	website->setColour(Label::textColourId, textColour);

	tempstr = T("Julian Storer, for making the");
	tempstr << T(" fantastic JUCE library upon which");
	tempstr << T(" Twindy is based (albeit slightly");
	tempstr << T(" modified).  See: \n");
	tempstr << T("http://www.rawmaterialsoftware.com/juce/");
	addAndMakeVisible(credits = new Label(T("credits"), tempstr));
	credits->setFont(tempFont);
	/*credits->setColours(textColour,
					    Colours::transparentBlack,
					    Colours::transparentBlack,
					    Colours::transparentBlack,
					    Colours::transparentBlack,
					    Colours::transparentBlack);*/
	credits->setColour(Label::textColourId, textColour);

	tempstr = T("Twindy is inspired by the Tracktion DAW (digital audio");
	tempstr << T(" workstation) software for Windows and OSX, and tries");
	tempstr << T(" to apply a similar workflow to a window manager.");
	tempstr << T("  The aim is to keep things as simple as possible while");
	tempstr << T(" still looking clean and respectable, with all options");
	tempstr << T(" accessable via the gui.");
	addAndMakeVisible(description = new Label(T("description"), tempstr));
	description->setFont(tempFont);
	/*description->setColours(textColour,
							Colours::transparentBlack,
							Colours::transparentBlack,
							Colours::transparentBlack,
							Colours::transparentBlack,
							Colours::transparentBlack);*/
	description->setColour(Label::textColourId, textColour);

	tempstr = T("GPL - see the COPYING file for details.");
	addAndMakeVisible(license = new Label(T("license"), tempstr));
	license->setFont(tempFont);
	/*license->setColours(textColour,
						Colours::transparentBlack,
						Colours::transparentBlack,
						Colours::transparentBlack,
						Colours::transparentBlack,
						Colours::transparentBlack);*/
	license->setColour(Label::textColourId, textColour);
}

//------------------------------------------------------------------------------
AboutPage::~AboutPage()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void AboutPage::resized()
{
	title->setBounds((getWidth()/2)-100, 5, 200, 30);
	juceVersionLabel->setBounds((getWidth()/2)+80, 20, 75, 25);

	authorLabel->setBounds((getWidth()/2)-200, 40, 75, 25);
	websiteLabel->setBounds((getWidth()/2)-200, 57, 75, 25);
	creditsLabel->setBounds((getWidth()/2)-200, 90, 75, 25);
	descriptionLabel->setBounds((getWidth()/2)-200, 160, 75, 25);
	licenseLabel->setBounds((getWidth()/2)-200, 270, 75, 25);

	author->setBounds((getWidth()/2)-125, 40, 275, 25);
	website->setBounds((getWidth()/2)-125, 57, 275, 25);
	credits->setBounds((getWidth()/2)-125, 90, 275, 65);
	description->setBounds((getWidth()/2)-125, 160, 275, 105);
	license->setBounds((getWidth()/2)-125, 270, 275, 25);
}

//------------------------------------------------------------------------------
void AboutPage::paint(Graphics &g)
{
	
}

//------------------------------------------------------------------------------
void AboutPage::setColours(const Colour& backCol,
						   const Colour& textCol,
						   const Colour& buttonCol)
{
	backgroundColour = backCol;
	textColour = textCol;

	title->setColour(Label::textColourId, textColour);
	juceVersionLabel->setColour(Label::textColourId, textColour);

	authorLabel->setColour(Label::textColourId, textColour);
	websiteLabel->setColour(Label::textColourId, textColour);
	creditsLabel->setColour(Label::textColourId, textColour);
	descriptionLabel->setColour(Label::textColourId, textColour);
	licenseLabel->setColour(Label::textColourId, textColour);

	author->setColour(Label::textColourId, textColour);
	website->setColour(Label::textColourId, textColour);
	credits->setColour(Label::textColourId, textColour);
	description->setColour(Label::textColourId, textColour);
	license->setColour(Label::textColourId, textColour);
}
