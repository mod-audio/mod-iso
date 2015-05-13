//	AboutPage.h - All about Twindy.
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

#ifndef ABOUTPAGE_H_
#define ABOUTPAGE_H_

///	All about Twindy.
class AboutPage : public Component
{
  public:
	///	Constructor.
	AboutPage();
	///	Destructor.
	~AboutPage();

	///	Places and sizes the various components.
	void resized();
	///	Draws the about page.
	void paint(Graphics &g);

	///	Called to set the background colour and text colour.
	void setColours(const Colour& backCol,
					const Colour& textCol,
					const Colour& buttonCol);

	///	Twindy's current version.
	static const String version;
  private:
	///	The colour of the background.
	Colour backgroundColour;
	///	The colour of the text.
	Colour textColour;

	///	Twindy title label.
	Label *title;
	///	JUCE version label.
	Label *juceVersionLabel;
	///	Author label.
	Label *authorLabel;
	///	Author text.
	Label *author;
	///	Website label.
	Label *websiteLabel;
	///	Website text.
	Label *website;
	///	Credits label.
	Label *creditsLabel;
	///	Credits text.
	Label *credits;
	///	Description label.
	Label *descriptionLabel;
	///	Description text.
	Label *description;
	///	License label.
	Label *licenseLabel;
	///	License text.
	Label *license;
};

#endif
