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

#include "AboutPage.h"
#include "../Utils.h"

//------------------------------------------------------------------------------
AboutPage::AboutPage()
    : Component()
{
    const double m(getSystemScaling());

    String tempstr;
    Font tempFont(26.0f*m);
    tempFont.setHeight(28.0f*m);

    tempstr = T("MOD Live-ISO");
    addAndMakeVisible(title = new Label(T("title"), tempstr));
    title->setFont(tempFont);
    title->setColour(Label::textColourId, textColour);

    tempFont.setBold(true);
    tempFont.setHeight(14.0f*m);

    tempstr = T("Author:");
    addAndMakeVisible(authorLabel = new Label(T("authorLabel"), tempstr));
    authorLabel->setFont(tempFont);
    authorLabel->setColour(Label::textColourId, textColour);

    tempstr = T("Website:");
    addAndMakeVisible(websiteLabel = new Label(T("websiteLabel"), tempstr));
    websiteLabel->setFont(tempFont);
    websiteLabel->setColour(Label::textColourId, textColour);

    tempstr = T("Credits:");
    addAndMakeVisible(creditsLabel = new Label(T("creditsLabel"), tempstr));
    creditsLabel->setFont(tempFont);
    creditsLabel->setColour(Label::textColourId, textColour);

    tempstr = T("About:");
    addAndMakeVisible(descriptionLabel = new Label(T("descriptionLabel"), tempstr));
    descriptionLabel->setFont(tempFont);
    descriptionLabel->setColour(Label::textColourId, textColour);

    tempstr = T("License:");
    addAndMakeVisible(licenseLabel = new Label(T("licenseLabel"), tempstr));
    licenseLabel->setFont(tempFont);
    licenseLabel->setColour(Label::textColourId, textColour);

    tempstr = T("falkTX");
    addAndMakeVisible(author = new Label(T("author"), tempstr));
    tempFont.setBold(false);
    author->setFont(tempFont);
    author->setColour(Label::textColourId, textColour);

    tempstr = T("https://github.com/portalmod/mod-iso");
    addAndMakeVisible(website = new Label(T("website"), tempstr));
    website->setFont(tempFont);
    website->setColour(Label::textColourId, textColour);

    tempstr  = T("Niall Moody, for making Twindy. See: http://www.niallmoody.com/twindy/\n");
    tempstr << T("Julian Storer, for making JUCE. See: http://www.rawmaterialsoftware.com/juce/");
    addAndMakeVisible(credits = new Label(T("credits"), tempstr));
    credits->setFont(tempFont);
    credits->setColour(Label::textColourId, textColour);

    tempstr  = T("Publications, products, content or services referenced herein or on the website are the exclusive");
    tempstr << T(" trademarks or servicemarks of MOD.\nOther product and company names mentioned in the site may be the");
    tempstr << T(" trademarks of their respective owners.\nAll software is available under the GPL license, see");
    tempstr << T(" https://www.gnu.org/licenses/gpl-2.0.html for details.");
    addAndMakeVisible(description = new Label(T("description"), tempstr));
    description->setFont(tempFont);
    description->setColour(Label::textColourId, textColour);

    tempstr = T("GPL - see the COPYING file for details.");
    addAndMakeVisible(license = new Label(T("license"), tempstr));
    license->setFont(tempFont);
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
    const double m(getSystemScaling());

    title->setBounds((getWidth()/2)-100*m, 5*m, 200*m, 30*m);

    authorLabel->setBounds((getWidth()/2)-200*m, 40*m, 75*m, 25*m);
    websiteLabel->setBounds((getWidth()/2)-200*m, 57*m, 75*m, 25*m);
    creditsLabel->setBounds((getWidth()/2)-200*m, 90*m, 75*m, 25);
    descriptionLabel->setBounds((getWidth()/2)-200*m, 160*m, 75*m, 25*m);
    licenseLabel->setBounds((getWidth()/2)-200*m, 320*m, 75*m, 25*m);

    author->setBounds((getWidth()/2)-125*m, 40*m, 275*m, 25*m);
    website->setBounds((getWidth()/2)-125*m, 57*m, 275*m, 25*m);
    credits->setBounds((getWidth()/2)-125*m, 90*m, 275*m, 65*m);
    description->setBounds((getWidth()/2)-125*m, 160*m, 290*m, 155*m);
    license->setBounds((getWidth()/2)-125*m, 320*m, 275*m, 25*m);
}

//------------------------------------------------------------------------------
void AboutPage::paint(Graphics&)
{
}

//------------------------------------------------------------------------------
void AboutPage::setColours(const Colour& backCol, const Colour& textCol, const Colour& buttonCol)
{
    backgroundColour = backCol;
    textColour = textCol;

    title->setColour(Label::textColourId, textColour);

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
