//	TwindyRootWindow.cpp - The Component which makes up the root window of the
//						   Window Manager.
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

#include <iostream>
#include <string>
#include "juce_amalgamated.h"
#include <signal.h>
#include "TwindyRootWindow.h"
#include "TwindyLAF.h"
#include "TwindyUpperPanel.h"
#include "TwindyLowerPanel.h"
#include "TwindyWindow.h"
#include "Clock.h"
#include "DrawableTextButton.h"
#include "TwindyProperties.h"
#include "TwindyErrorDisplay.h"
#include "TwindyTabs.h"
#include "TwindyPreferences.h"
#include "TwindyApp.h"
#include "TracktionScheme.h"
#include "TwindyHelperStuff.h"

//----------------------------------------------------------------------------------------------
namespace JUCE_NAMESPACE
{
///	Not entirely sure if this is how I want to do this...
/*!
	(the definition is in juce_linux_Files.cpp)
 */
bool juce_launchFile(const String& fileName, const String& parameters);

extern Display* display;
}

//----------------------------------------------------------------------------------------------
TwindyRootWindow::TwindyRootWindow():
Component(T("Twindy Window Manager")),
currentUpperPanelComp(0),
exitButton(0),
properties(0),
lowerWindow(0),
destinedForLowerPanel(false),
currentlyInFocus(false)
{
	int i;
	TwindyProperty tempProp;

	//Construct this first to make sure we get any error messages that might be
	//generated from the following code.
	//addAndMakeVisible(TwindyErrorDisplay::getInstance());

	//Load default font.
	properties = new TwindyProperties();
	tempProp = properties->getProperty(T("GlobalFont"));
	if(tempProp.name == T("nil"))
	{
		TWINDY_DBG_MESSAGE("Pre-v0.40 twindyrc, updating global font.");
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("Bitstream Vera Sans");
		tempProp.value = T("Bitstream Vera Sans");
		properties->setProperty(T("GlobalFont"), tempProp);
	}
	//Font::setDefaultSansSerifFontName(tempProp.name);
	static_cast<TwindyApp *>(JUCEApplication::getInstance())->setRootWindow(this);

	{
		TwindyLAF *laf = new TwindyLAF;

		laf->setDefaultSansSerifTypefaceName(tempProp.name);
		LookAndFeel::setDefaultLookAndFeel(laf);
	}

	//Setup text localisation.
	tempProp = properties->getProperty(T("LocalisationFile"));
	if((tempProp.name != T("nil")) && (tempProp.value != T("")))
	{
		LocalisedStrings *tempStrings = new LocalisedStrings(File(tempProp.value));
		LocalisedStrings::setCurrentMappings(tempStrings);
	}

	//Load colours.
	loadColours(properties->getProperty(T("TracktionScheme")).name);

	TwindyErrorDisplay::getInstance()->setBackgroundColour(colours.propertyPanelBackground);
	TwindyErrorDisplay::getInstance()->setTextColour(colours.propertyPanelText);

	//This is so our 'root' window never obscures it's 'children'.
	setBroughtToFrontOnMouseClick(false);

	MessageManager::getInstance()->registerMapRequestCallback(this);

	//Set up workspace tabs.
	addAndMakeVisible(workspaces = new TwindyTabs());
	workspaces->setTabBarDepth(25);
	//workspaces->setOutline(colours.mainBackground, 0);
	workspaces->setOutline(0);
	workspaces->setIndent(10);
	//Setup up preferences tab.
	preferences = new TwindyPreferences();
	preferences->setTracktionScheme(properties->getProperty(T("TracktionScheme")).name);
	preferences->setPrefColours(colours.propertyPanelBackground,
								colours.propertyPanelText,
								colours.selectedFilterOutline,
								colours.propertyPanelOutline,
								colours.mainBackground,
								colours.blueButton);
	preferences->setTabOrientation(TwindyTabbedComponent::TabsAtLeft,
								   150,
								   150,
								   1);
	workspaces->addTab(TRANS("Preferences"),
					   colours.mainBackground, 
					   preferences, 
					   false);
	workspaces->setCurrentTabIndex(0);
	//Load workspaces.
	tempProp = properties->getProperty(T("Workspace"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, with no Workspace entries, and we need to
		//update it.
		TWINDY_DBG_MESSAGE("Pre-v0.40 twindyrc, updating workspaces.");
		tempProp.name = T(".");
		tempProp.value = T(".");
		properties->setProperty(T("Workspace"), tempProp);
		tempProp.name = T("Workspace 1");
		tempProp.value = T("~/.twindy/Default.tracktionscheme");
		properties->setSubProperty(T("Workspace"), tempProp);
	}
	for(i=0;i<properties->getNumSubProperties(T("Workspace"));++i)
	{
		TracktionScheme *tempScheme;
		upperPanelComps.add(new TwindyUpperPanel());
		upperPanelComps[i]->setName(properties->getSubProperty(T("Workspace"),
															   i).name);
		upperPanelComps[i]->setSchemePath(properties->getSubProperty(T("Workspace"),
															  i).value);
		tempScheme = upperPanelComps[i]->getTracktionScheme();
		upperPanelComps[i]->setColours(tempScheme->getColour(T("propertyPanelBackground")),
									   tempScheme->getColour(T("propertyPanelText")),
									   tempScheme->getColour(T("selectedFilterOutline")),
									   tempScheme->getColour(T("propertyPanelOutline")),
									   tempScheme->getColour(T("mainBackground")));
		upperPanelComps[i]->setTabOrientation(TwindyTabbedComponent::TabsAtLeft,
											  150,
											  150,
											  1);
		workspaces->addTab(upperPanelComps[i]->getName(),
						   tempScheme->getColour(T("mainBackground")),
						   upperPanelComps[i],
						   false);
	}
	workspaces->setCurrentTabIndex(1);
	preferences->updateWorkspaces();

	//Set up lower panel.
	addAndMakeVisible(lowerPanel = new TwindyLowerPanel());
	lowerPanel->setOutlineColour(colours.propertyPanelOutline);
	lowerPanel->setTextColour(colours.propertyPanelText);
	lowerPanel->setBackgroundColour(colours.propertyPanelBackground);

	//Set up buttons labels.
	addAndMakeVisible(upperLabel = new Label(T("Upper"), TRANS("Upper")));
	upperLabel->setColour(Label::textColourId, colours.menuText);
	upperLabel->setJustificationType(Justification(Justification::centred));
	upperLabel->setFont(Font(12.0f, Font::bold));
	addAndMakeVisible(lowerLabel = new Label(T("Lower"), TRANS("Lower")));
	lowerLabel->setColour(Label::textColourId, colours.menuText);
	lowerLabel->setJustificationType(Justification(Justification::centred));
	lowerLabel->setFont(Font(12.0f, Font::bold));

	//Set up buttons to move apps between panels.
	addAndMakeVisible(moveUp = new DrawableButton(T("MoveUp"),
					  							  DrawableButton::ImageOnButtonBackground));
	{
		Path p;
		DrawablePath d;

		p.addArrow(50, 90, 50, 10, 20, 60, 40);
		d.setPath(p);
		d.setSolidFill(Colours::white);

		moveUp->setImages(&d);
	}
	moveUp->setBackgroundColours(colours.yellowButton,
								 colours.yellowButton.darker(2.5f));
	moveUp->setConnectedEdges(Button::ConnectedOnRight);
	moveUp->addButtonListener(this);
	addAndMakeVisible(moveDown = new DrawableButton(T("MoveDown"),
					  							    DrawableButton::ImageOnButtonBackground));
	{
		Path p;
		DrawablePath d;

		p.addArrow(50, 10, 50, 90, 20, 60, 40);
		d.setPath(p);
		d.setSolidFill(Colours::white);

		moveDown->setImages(&d);
	}
	moveDown->setBackgroundColours(colours.yellowButton,
								   colours.yellowButton.darker(2.5f));
	moveDown->setConnectedEdges(Button::ConnectedOnLeft);
	moveDown->addButtonListener(this);

	//Set up buttons.
	addAndMakeVisible(exitButton = new DrawableTextButton(T("Log Off..."),
												  		  TRANS("Log Off...")));
	exitButton->setBackgroundColours(colours.yellowButton,
									 colours.yellowButton.darker(2.5f));
	exitButton->setTextColour(colours.menuText);
	exitButton->addButtonListener(this);
	addAndMakeVisible(leftButton1 = new DrawableTextButton(T("LeftButton1"),
												  		   TRANS("LeftButton1")));
	leftButton1->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton1->setTextColour(colours.menuText);
	leftButton1->setButtonText(properties->getProperty(T("LeftButton1")).name);
	if(properties->getNumSubProperties(T("LeftButton1")) > 0)
		leftButton1->setBold();
	leftButton1->addButtonListener(this);
	addAndMakeVisible(leftButton2 = new DrawableTextButton(T("LeftButton2"),
												  		   TRANS("LeftButton2")));
	leftButton2->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton2->setTextColour(colours.menuText);
	leftButton2->setButtonText(properties->getProperty(T("LeftButton2")).name);
	if(properties->getNumSubProperties(T("LeftButton2")) > 0)
		leftButton2->setBold();
	leftButton2->addButtonListener(this);
	addAndMakeVisible(leftButton3 = new DrawableTextButton(T("LeftButton3"),
												  		   TRANS("LeftButton3")));
	leftButton3->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton3->setTextColour(colours.menuText);
	leftButton3->setButtonText(properties->getProperty(T("LeftButton3")).name);
	if(properties->getNumSubProperties(T("LeftButton3")) > 0)
		leftButton3->setBold();
	leftButton3->addButtonListener(this);
	addAndMakeVisible(leftButton4 = new DrawableTextButton(T("LeftButton4"),
												  		   TRANS("LeftButton4")));
	leftButton4->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton4->setTextColour(colours.menuText);
	leftButton4->setButtonText(properties->getProperty(T("LeftButton4")).name);
	if(properties->getNumSubProperties(T("LeftButton4")) > 0)
		leftButton4->setBold();
	leftButton4->addButtonListener(this);
	addAndMakeVisible(leftButton5 = new DrawableTextButton(T("LeftButton5"),
												  		   TRANS("LeftButton5")));
	leftButton5->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton5->setTextColour(colours.menuText);
	leftButton5->setButtonText(properties->getProperty(T("LeftButton5")).name);
	if(properties->getNumSubProperties(T("LeftButton5")) > 0)
		leftButton5->setBold();
	leftButton5->addButtonListener(this);
	addAndMakeVisible(leftButton6 = new DrawableTextButton(T("LeftButton6"),
												  		   TRANS("LeftButton6")));
	leftButton6->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton6->setTextColour(colours.menuText);
	leftButton6->setButtonText(properties->getProperty(T("LeftButton6")).name);
	if(properties->getNumSubProperties(T("LeftButton6")) > 0)
		leftButton6->setBold();
	leftButton6->addButtonListener(this);
	addAndMakeVisible(leftButton7 = new DrawableTextButton(T("LeftButton7"),
												  		   TRANS("LeftButton7")));
	leftButton7->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton7->setTextColour(colours.menuText);
	leftButton7->setButtonText(properties->getProperty(T("LeftButton7")).name);
	if(properties->getNumSubProperties(T("LeftButton7")) > 0)
		leftButton7->setBold();
	leftButton7->addButtonListener(this);

	addAndMakeVisible(rightButton1 = new DrawableTextButton(T("RightButton1"),
												  		    TRANS("RightButton1")));
	rightButton1->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton1->setTextColour(colours.menuText);
	rightButton1->setButtonText(properties->getProperty(T("RightButton1")).name);
	if(properties->getNumSubProperties(T("RightButton1")) > 0)
		rightButton1->setBold();
	rightButton1->addButtonListener(this);
	addAndMakeVisible(rightButton2 = new DrawableTextButton(T("RightButton2"),
												  		    TRANS("RightButton2")));
	rightButton2->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton2->setTextColour(colours.menuText);
	rightButton2->setButtonText(properties->getProperty(T("RightButton2")).name);
	if(properties->getNumSubProperties(T("RightButton2")) > 0)
		rightButton2->setBold();
	rightButton2->addButtonListener(this);
	addAndMakeVisible(rightButton3 = new DrawableTextButton(T("RightButton3"),
												  		    TRANS("RightButton3")));
	rightButton3->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton3->setTextColour(colours.menuText);
	rightButton3->setButtonText(properties->getProperty(T("RightButton3")).name);
	if(properties->getNumSubProperties(T("RightButton3")) > 0)
		rightButton3->setBold();
	rightButton3->addButtonListener(this);
	addAndMakeVisible(rightButton4 = new DrawableTextButton(T("RightButton4"),
												  		    TRANS("RightButton4")));
	rightButton4->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton4->setTextColour(colours.menuText);
	rightButton4->setButtonText(properties->getProperty(T("RightButton4")).name);
	if(properties->getNumSubProperties(T("RightButton4")) > 0)
		rightButton4->setBold();
	rightButton4->addButtonListener(this);
	addAndMakeVisible(rightButton5 = new DrawableTextButton(T("RightButton5"),
												  		    TRANS("RightButton5")));
	rightButton5->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton5->setTextColour(colours.menuText);
	rightButton5->setButtonText(properties->getProperty(T("RightButton5")).name);
	if(properties->getNumSubProperties(T("RightButton5")) > 0)
		rightButton5->setBold();
	rightButton5->addButtonListener(this);
	addAndMakeVisible(rightButton6 = new DrawableTextButton(T("RightButton6"),
												  		    TRANS("RightButton6")));
	rightButton6->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton6->setTextColour(colours.menuText);
	rightButton6->setButtonText(properties->getProperty(T("RightButton6")).name);
	if(properties->getNumSubProperties(T("RightButton6")) > 0)
		rightButton6->setBold();
	rightButton6->addButtonListener(this);
	addAndMakeVisible(rightButton7 = new DrawableTextButton(T("RightButton7"),
												  		    TRANS("RightButton7")));
	rightButton7->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton7->setTextColour(colours.menuText);
	rightButton7->setButtonText(properties->getProperty(T("RightButton7")).name);
	if(properties->getNumSubProperties(T("RightButton7")) > 0)
		rightButton7->setBold();
	rightButton7->addButtonListener(this);

	//Clock
	addAndMakeVisible(clock = new Clock());
	clock->setColour(colours.propertyPanelText);
	tempProp = properties->getProperty(T("ClockIncludeSeconds"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("ClockIncludeSeconds");
		tempProp.value = T("TRUE");
		properties->setProperty(T("ClockIncludeSeconds"), tempProp);
	}
	if(tempProp.value == T("TRUE"))
		clock->setIncludeSeconds(true);
	else
		clock->setIncludeSeconds(false);
	//use24HourClock property.
	tempProp = properties->getProperty(T("ClockUse24HourClock"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("ClockUse24HourClock");
		tempProp.value = T("FALSE");
		properties->setProperty(T("ClockUse24HourClock"), tempProp);
	}
	if(tempProp.value == T("TRUE"))
		clock->setUse24HourClock(true);
	else
		clock->setUse24HourClock(false);
	//monthDisplayedFirst property.
	tempProp = properties->getProperty(T("ClockMonthDisplayedFirst"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("ClockMonthDisplayedFirst");
		tempProp.value = T("FALSE");
		properties->setProperty(T("ClockMonthDisplayedFirst"), tempProp);
	}
	if(tempProp.value == T("TRUE"))
		clock->setMonthDisplayedFirst(true);
	else
		clock->setMonthDisplayedFirst(false);
	//dateSeparator property.
	tempProp = properties->getProperty(T("ClockMonthDateSeparator"));
	if(tempProp.name == T("nil"))
	{
		//It's an older twindyrc, and we need to update it.
		tempProp.name = T("ClockMonthDateSeparator");
		tempProp.value = T("/");
		properties->setProperty(T("ClockMonthDateSeparator"), tempProp);
	}
	clock->setDateSeparator(tempProp.value);

	updateColours();

	TWINDY_DBG_MESSAGE("About to makeTwindyRootWindow visible and add it to the desktop");
	setVisible(true);
	addToDesktop(ComponentPeer::windowAppearsOnTaskbar);
	TWINDY_DBG_MESSAGE("TwindyRootWindow now visible and added to desktop");

	setupMappingRedirect();

	//Now launch any programs specified in twindyrc.
	launchStartupPrograms();
}

//----------------------------------------------------------------------------------------------
TwindyRootWindow::~TwindyRootWindow()
{
	long i;

	if(lowerWindow)
	{
		lowerWindow->closeWindow();
		delete lowerWindow;
	}

	//Wait, in case some of the upperWindows were started from a terminal in the
	//lowerWindow, in which case upperWindows will need to be updated, or we
	//may not kill all the programs in the upper panel.
	Thread::yield();

	//Kill any programs we ran at startup.
	for(i=0;i<pidArray.size();++i)
		kill(pidArray[i], SIGTERM);

	//Remove error display first, so we don't try and delete it twice.
	removeChildComponent(TwindyErrorDisplay::getInstance());

	deleteAllChildren();

	delete preferences;

	delete properties;
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::paint(Graphics &g)
{
	const int lowerBorder = (int)(getHeight()-((float)getHeight()/3.5f));
	int textHeight = 425;
	float textStretch = 8.0f;
	int textStart;
	Font tempFont(16.0f, Font::bold);

	g.setFont(tempFont);

	g.setColour(colours.mainBackground.withAlpha((unsigned char)255));
	g.fillRect(0, 0, getWidth(), getHeight());
	g.setColour(colours.tabAreaBackground);
	g.fillRect(0, 0, getWidth(), 25);

	textHeight = (lowerBorder - 75);
	textStart = lowerBorder - 25;
	textStretch = (float)textHeight * (8.0f/425.0f);

	g.setColour(colours.propertyPanelBackground);
	g.drawTextAsPath(TRANS("Twindy"),
					 AffineTransform::identity.scaled(textStretch, 6.0f).
					 followedBy(AffineTransform::identity.rotated(-1.570796327f).
					 followedBy(AffineTransform::identity.translated(110, textStart))));
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::resized()
{
	const int lowerBorder = (int)(getHeight()-((float)getHeight()/3.5f));
	const int buttonHeight = (int)(((float)getHeight()/3.5f)/9.0f);
	int tempint;

	/*upperPanel->setBounds(10,
						  30,
						  (getWidth()-20),
						  (getHeight()-(getHeight()/3)));*/
	workspaces->setBounds(0,
						  0,
						  getWidth(),
						  (getHeight()-(getHeight()/3))+25);
	lowerPanel->setBounds(220,
						 lowerBorder,
						 (getWidth()-230),
						 (getHeight()-lowerBorder-5));

	upperLabel->setBounds(10, lowerBorder-buttonHeight+5, 100, buttonHeight-4);
	lowerLabel->setBounds(110, lowerBorder-buttonHeight+5, 100, buttonHeight-4);

	moveUp->setBounds((getWidth()-90),
					  lowerBorder-(buttonHeight/2)-2, 
					  40, 
					  buttonHeight);
	moveDown->setBounds((getWidth()-50),
					    lowerBorder-(buttonHeight/2)-2, 
					    40, 
					    buttonHeight);

	tempint = lowerBorder;
	leftButton1->setBounds(10, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	leftButton2->setBounds(10, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	leftButton3->setBounds(10, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	leftButton4->setBounds(10, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	leftButton5->setBounds(10, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	leftButton6->setBounds(10, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	leftButton7->setBounds(10, tempint, 100, buttonHeight);

	tempint = lowerBorder;
	rightButton1->setBounds(110, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	rightButton2->setBounds(110, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	rightButton3->setBounds(110, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	rightButton4->setBounds(110, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	rightButton5->setBounds(110, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	rightButton6->setBounds(110, tempint, 100, buttonHeight);
	tempint += buttonHeight;
	rightButton7->setBounds(110, tempint, 100, buttonHeight);
	tempint += buttonHeight;

	tempint += buttonHeight;
	exitButton->setBounds(10, tempint, 100, buttonHeight);

	clock->setBounds((getWidth()-100), 0, 100, 25);

	TwindyErrorDisplay::getInstance()->setBounds((getWidth()-170),
												 (lowerBorder+13),
												 160,
												 (getHeight()-lowerBorder-18));
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::actionListenerCallback(const String& message)
{
	if(message == exitButton->getName())
	{
		//JUCEApplication::quit();
		delete this;
	}
	else if(message == leftButton1->getName())
		launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton1")).value));
	else if(message == leftButton2->getName())
		launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton2")).value));
	else if(message == leftButton3->getName())
		launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton3")).value));
	else if(message == leftButton4->getName())
		launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton4")).value));
	else if(message == leftButton5->getName())
		launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton5")).value));
	else if(message == leftButton6->getName())
		launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton6")).value));
	else if(message == leftButton7->getName())
		launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton7")).value));
	else if(message == rightButton1->getName())
	{
		if(lowerWindow)
		{
			closeLowerWindow();
			delete lowerWindow;
		}

		destinedForLowerPanel = true;
		launchExecutable(static_cast<const char *>(properties->getProperty(T("RightButton1")).value));
	}
	else if(message == rightButton2->getName())
	{
		if(lowerWindow)
		{
			closeLowerWindow();
			delete lowerWindow;
		}

		destinedForLowerPanel = true;
		launchExecutable(static_cast<const char *>(properties->getProperty(T("RightButton2")).value));
	}
	else if(message == rightButton3->getName())
	{
		if(lowerWindow)
		{
			closeLowerWindow();
			delete lowerWindow;
		}

		destinedForLowerPanel = true;
		launchExecutable(static_cast<const char *>(properties->getProperty(T("RightButton3")).value));
	}
	else if(message == rightButton4->getName())
	{
		if(lowerWindow)
		{
			closeLowerWindow();
			delete lowerWindow;
		}

		destinedForLowerPanel = true;
		launchExecutable(static_cast<const char *>(properties->getProperty(T("RightButton4")).value));
	}
	else if(message == rightButton5->getName())
	{
		if(lowerWindow)
		{
			closeLowerWindow();
			delete lowerWindow;
		}

		destinedForLowerPanel = true;
		launchExecutable(static_cast<const char *>(properties->getProperty(T("RightButton5")).value));
	}
	else if(message == rightButton6->getName())
	{
		if(lowerWindow)
		{
			closeLowerWindow();
			delete lowerWindow;
		}

		destinedForLowerPanel = true;
		launchExecutable(static_cast<const char *>(properties->getProperty(T("RightButton6")).value));
	}
	else if(message == rightButton7->getName())
	{
		if(lowerWindow)
		{
			closeLowerWindow();
			delete lowerWindow;
		}

		destinedForLowerPanel = true;
		launchExecutable(static_cast<const char *>(properties->getProperty(T("RightButton7")).value));
	}
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::buttonClicked(Button* button)
{
	long i;
	int menuResult;
	PopupMenu menu;

	//There's got to be a nicer way of doing this...

	if(button == exitButton)
	{
		bool test = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
												 TRANS("Log Off..."),
												 TRANS("Are you sure you want to quit Twindy?"),
												 TRANS("Yes"),
												 TRANS("No"));

		if(test)
			JUCEApplication::quit();
	}
	else if(button == leftButton1)
	{
		if(properties->getNumSubProperties(T("LeftButton1")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("LeftButton1"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("LeftButton1"), i).name);
			menuResult = menu.showAt(leftButton1);
			//raiseAllWindows();
			if(menuResult > 0)
				launchExecutable(static_cast<const char *>(properties->getSubProperty(T("LeftButton1"), (menuResult-1)).value));
		}
		else
			launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton1")).value));
	}
	else if(button == leftButton2)
	{
		if(properties->getNumSubProperties(T("LeftButton2")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("LeftButton2"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("LeftButton2"), i).name);
			menuResult = menu.showAt(leftButton2);
			//raiseAllWindows();
			if(menuResult > 0)
				launchExecutable(static_cast<const char *>(properties->getSubProperty(T("LeftButton2"), (menuResult-1)).value));
		}
		else
		{
			launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton2")).value));
		}
	}
	else if(button == leftButton3)
	{
		if(properties->getNumSubProperties(T("LeftButton3")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("LeftButton3"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("LeftButton3"), i).name);
			menuResult = menu.showAt(leftButton3);
			//raiseAllWindows();
			if(menuResult > 0)
				launchExecutable(static_cast<const char *>(properties->getSubProperty(T("LeftButton3"), (menuResult-1)).value));
		}
		else
		{
			launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton3")).value));
		}
	}
	else if(button == leftButton4)
	{
		if(properties->getNumSubProperties(T("LeftButton4")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("LeftButton4"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("LeftButton4"), i).name);
			menuResult = menu.showAt(leftButton4);
			//raiseAllWindows();
			if(menuResult > 0)
				launchExecutable(static_cast<const char *>(properties->getSubProperty(T("LeftButton4"), (menuResult-1)).value));
		}
		else
		{
			launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton4")).value));
		}
	}
	else if(button == leftButton5)
	{
		if(properties->getNumSubProperties(T("LeftButton5")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("LeftButton5"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("LeftButton5"), i).name);
			menuResult = menu.showAt(leftButton5);
			//raiseAllWindows();
			if(menuResult > 0)
				launchExecutable(static_cast<const char *>(properties->getSubProperty(T("LeftButton5"), (menuResult-1)).value));
		}
		else
		{
			launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton5")).value));
		}
	}
	else if(button == leftButton6)
	{
		if(properties->getNumSubProperties(T("LeftButton6")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("LeftButton6"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("LeftButton6"), i).name);
			menuResult = menu.showAt(leftButton6);
			//raiseAllWindows();
			if(menuResult > 0)
				launchExecutable(static_cast<const char *>(properties->getSubProperty(T("LeftButton6"), (menuResult-1)).value));
		}
		else
		{
			launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton6")).value));
		}
	}
	else if(button == leftButton7)
	{
		if(properties->getNumSubProperties(T("LeftButton7")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("LeftButton7"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("LeftButton7"), i).name);
			menuResult = menu.showAt(leftButton7);
			//raiseAllWindows();
			if(menuResult > 0)
				launchExecutable(static_cast<const char *>(properties->getSubProperty(T("LeftButton7"), (menuResult-1)).value));
		}
		else
		{
			launchExecutable(static_cast<const char *>(properties->getProperty(T("LeftButton7")).value));
		}
	}
	else if(button == rightButton1)
	{
		if(properties->getNumSubProperties(T("RightButton1")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("RightButton1"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("RightButton1"), i).name);
			menuResult = menu.showAt(rightButton1);
			//raiseAllWindows();
			if(menuResult > 0)
				launchLowerPanel(static_cast<const char *>(properties->getSubProperty(T("RightButton1"), (menuResult-1)).value));
		}
		else
		{
			launchLowerPanel(static_cast<const char *>(properties->getProperty(T("RightButton1")).value));
		}
	}
	else if(button == rightButton2)
	{
		if(properties->getNumSubProperties(T("RightButton2")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("RightButton2"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("RightButton2"), i).name);
			menuResult = menu.showAt(rightButton2);
			//raiseAllWindows();
			if(menuResult > 0)
				launchLowerPanel(static_cast<const char *>(properties->getSubProperty(T("RightButton2"), (menuResult-1)).value));
		}
		else
		{
			launchLowerPanel(static_cast<const char *>(properties->getProperty(T("RightButton2")).value));
		}
	}
	else if(button == rightButton3)
	{
		if(properties->getNumSubProperties(T("RightButton3")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("RightButton3"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("RightButton3"), i).name);
			menuResult = menu.showAt(rightButton3);
			//raiseAllWindows();
			if(menuResult > 0)
				launchLowerPanel(static_cast<const char *>(properties->getSubProperty(T("RightButton3"), (menuResult-1)).value));
		}
		else
		{
			launchLowerPanel(static_cast<const char *>(properties->getProperty(T("RightButton3")).value));
		}
	}
	else if(button == rightButton4)
	{
		if(properties->getNumSubProperties(T("RightButton4")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("RightButton4"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("RightButton4"), i).name);
			menuResult = menu.showAt(rightButton4);
			//raiseAllWindows();
			if(menuResult > 0)
				launchLowerPanel(static_cast<const char *>(properties->getSubProperty(T("RightButton4"), (menuResult-1)).value));
		}
		else
		{
			launchLowerPanel(static_cast<const char *>(properties->getProperty(T("RightButton4")).value));
		}
	}
	else if(button == rightButton5)
	{
		if(properties->getNumSubProperties(T("RightButton5")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("RightButton5"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("RightButton5"), i).name);
			menuResult = menu.showAt(rightButton5);
			//raiseAllWindows();
			if(menuResult > 0)
				launchLowerPanel(static_cast<const char *>(properties->getSubProperty(T("RightButton5"), (menuResult-1)).value));
		}
		else
		{
			launchLowerPanel(static_cast<const char *>(properties->getProperty(T("RightButton5")).value));
		}
	}
	else if(button == rightButton6)
	{
		if(properties->getNumSubProperties(T("RightButton6")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("RightButton6"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("RightButton6"), i).name);
			menuResult = menu.showAt(rightButton6);
			//raiseAllWindows();
			if(menuResult > 0)
				launchLowerPanel(static_cast<const char *>(properties->getSubProperty(T("RightButton6"), (menuResult-1)).value));
		}
		else
		{
			launchLowerPanel(static_cast<const char *>(properties->getProperty(T("RightButton6")).value));
		}
	}
	else if(button == rightButton7)
	{
		if(properties->getNumSubProperties(T("RightButton7")) > 0)
		{
			//raiseAllWindows();
			for(i=0;i<properties->getNumSubProperties(T("RightButton7"));++i)
				menu.addItem((i+1), properties->getSubProperty(T("RightButton7"), i).name);
			menuResult = menu.showAt(rightButton7);
			//raiseAllWindows();
			if(menuResult > 0)
				launchLowerPanel(static_cast<const char *>(properties->getSubProperty(T("RightButton7"), (menuResult-1)).value));
		}
		else
		{
			launchLowerPanel(static_cast<const char *>(properties->getProperty(T("RightButton7")).value));
		}
	}
	else if(button == moveUp)
		moveLowerToUpper();
	else if(button == moveDown)
		moveUpperToLower();
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::setVisibleUpperPanel(int index)
{
	//Hide currently-visible window, if this isn't the preferences panel.
	if((currentUpperPanelComp < upperPanelComps.size())&&
	   (currentUpperPanelComp > -1))
		upperPanelComps[currentUpperPanelComp]->setWorkspaceIsVisible(false);

	//Set currentUpperPanelComp, making sure index doesn't point to a non-
	//existant tab.
	if(index >= upperPanelComps.size())
		index = (upperPanelComps.size()-1);
	currentUpperPanelComp = index;

	//Show newly-visible window, if it isn't the preferences panel.
	if((currentUpperPanelComp < upperPanelComps.size())&&
	   (currentUpperPanelComp > -1))
		upperPanelComps[currentUpperPanelComp]->setWorkspaceIsVisible(true);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::prefsChanged()
{
	int i, j;
	TracktionScheme *prefScheme;

	if(!exitButton)
		return;

	//Handle workspace changes.
	int newWorkspaces;
	int oldWorkspaces;
	newWorkspaces = properties->getNumSubProperties(T("Workspace"));
	oldWorkspaces = upperPanelComps.size();
	if(newWorkspaces < oldWorkspaces)
	{
		for(i=(oldWorkspaces-1);i>=newWorkspaces;--i)
		{
			std::string tempstr;

			tempstr = "extraWS no.";
			tempstr += i;
			TWINDY_DBG_MESSAGE(tempstr);
			//Move any newly-orphaned windows to the first valid workspace.
			for(j=(upperPanelComps[i]->getNumWindows()-1);j>=0;--j)
			{
				tempstr = "extraWindow no.";
				tempstr += j;
				TWINDY_DBG_MESSAGE(tempstr);
				TwindyUpperTab *newTab = new TwindyUpperTab(T("Browser"),
															colours.propertyPanelText,
															colours.propertyPanelBackground);
				upperPanelComps[newWorkspaces-1]->addWindow(upperPanelComps[i]->getWindow(j),
															newTab);
				tempstr = "Added to WS no.";
				tempstr += newWorkspaces-1;
				TWINDY_DBG_MESSAGE(tempstr);
				upperPanelComps[i]->removeIndexedWindow(j);
			}
			//Now it's okay, remove the workspace.
			upperPanelComps.remove(i);
		}
	}
	else if(newWorkspaces > oldWorkspaces)
	{
		//Add any new workspaces.
		for(i=oldWorkspaces;i<newWorkspaces;++i)
		{
			TwindyUpperPanel *tempPanel = new TwindyUpperPanel();
			tempPanel ->setTabOrientation(TwindyTabbedComponent::TabsAtLeft,
										  150,
										  150,
										  1);
			upperPanelComps.add(tempPanel);
		}
	}
	//Remove all the current tabs, so we can reset the colours.
	workspaces->clearTabs();

	//Set workspace colours.
	preferences->setTracktionScheme(properties->getProperty(T("TracktionScheme")).name);
	prefScheme = preferences->getTracktionScheme();
	preferences->setPrefColours(prefScheme->getColour(T("propertyPanelBackground")),
								prefScheme->getColour(T("propertyPanelText")),
								prefScheme->getColour(T("selectedFilterOutline")),
								prefScheme->getColour(T("propertyPanelOutline")),
								prefScheme->getColour(T("mainBackground")),
								prefScheme->getColour(T("blueButton")));
	workspaces->addTab(TRANS("Preferences"),
					   prefScheme->getColour(T("mainBackground")),
					   preferences,
					   false);
	for(i=0;i<properties->getNumSubProperties(T("Workspace"));++i)
	{
		TracktionScheme *tempScheme;
		upperPanelComps[i]->setName(properties->getSubProperty(T("Workspace"),
															   i).name);
		upperPanelComps[i]->setSchemePath(properties->getSubProperty(T("Workspace"),
															  i).value);
		tempScheme = upperPanelComps[i]->getTracktionScheme();
		upperPanelComps[i]->setColours(tempScheme->getColour(T("propertyPanelBackground")),
									   tempScheme->getColour(T("propertyPanelText")),
									   tempScheme->getColour(T("selectedFilterOutline")),
									   tempScheme->getColour(T("propertyPanelOutline")),
									   tempScheme->getColour(T("mainBackground")));
		workspaces->addTab(upperPanelComps[i]->getName(),
						   tempScheme->getColour(T("mainBackground")),
						   upperPanelComps[i],
						   false);
	}

	//Set button colours.
	leftButton1->setButtonText(properties->getProperty(T("LeftButton1")).name);
	leftButton1->setBold(properties->getNumSubProperties(T("LeftButton1")) > 0);
	leftButton2->setButtonText(properties->getProperty(T("LeftButton2")).name);
	leftButton2->setBold(properties->getNumSubProperties(T("LeftButton2")) > 0);
	leftButton3->setButtonText(properties->getProperty(T("LeftButton3")).name);
	leftButton3->setBold(properties->getNumSubProperties(T("LeftButton3")) > 0);
	leftButton4->setButtonText(properties->getProperty(T("LeftButton4")).name);
	leftButton4->setBold(properties->getNumSubProperties(T("LeftButton4")) > 0);
	leftButton5->setButtonText(properties->getProperty(T("LeftButton5")).name);
	leftButton5->setBold(properties->getNumSubProperties(T("LeftButton5")) > 0);
	leftButton6->setButtonText(properties->getProperty(T("LeftButton6")).name);
	leftButton6->setBold(properties->getNumSubProperties(T("LeftButton6")) > 0);
	leftButton7->setButtonText(properties->getProperty(T("LeftButton7")).name);
	leftButton7->setBold(properties->getNumSubProperties(T("LeftButton7")) > 0);
	rightButton1->setButtonText(properties->getProperty(T("RightButton1")).name);
	rightButton1->setBold(properties->getNumSubProperties(T("RightButton1")) > 0);
	rightButton2->setButtonText(properties->getProperty(T("RightButton2")).name);
	rightButton2->setBold(properties->getNumSubProperties(T("RightButton2")) > 0);
	rightButton3->setButtonText(properties->getProperty(T("RightButton3")).name);
	rightButton3->setBold(properties->getNumSubProperties(T("RightButton3")) > 0);
	rightButton4->setButtonText(properties->getProperty(T("RightButton4")).name);
	rightButton4->setBold(properties->getNumSubProperties(T("RightButton4")) > 0);
	rightButton5->setButtonText(properties->getProperty(T("RightButton5")).name);
	rightButton5->setBold(properties->getNumSubProperties(T("RightButton5")) > 0);
	rightButton6->setButtonText(properties->getProperty(T("RightButton6")).name);
	rightButton6->setBold(properties->getNumSubProperties(T("RightButton6")) > 0);
	rightButton7->setButtonText(properties->getProperty(T("RightButton7")).name);
	rightButton7->setBold(properties->getNumSubProperties(T("RightButton7")) > 0);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::updateColours()
{
	TracktionScheme *tempScheme;
	TracktionScheme *prefScheme;

	if(!exitButton)
		return;

	if(currentUpperPanelComp == -1)
	{
		tempScheme = preferences->getTracktionScheme();
		prefScheme = tempScheme;
	}	
	else
	{
		tempScheme = upperPanelComps[currentUpperPanelComp]->getTracktionScheme();
		prefScheme = preferences->getTracktionScheme();
	}

	colours.mainBackground = tempScheme->getColour(T("mainBackground"));
	colours.tabAreaBackground = prefScheme->getColour(T("tabAreaBackground"));
	colours.menuText = tempScheme->getColour(T("menuText"));
	colours.propertyPanelBackground = tempScheme->getColour(T("propertyPanelBackground"));
	colours.propertyPanelText = tempScheme->getColour(T("propertyPanelText"));
	colours.propertyPanelOutline = tempScheme->getColour(T("propertyPanelOutline"));
	colours.selectedFilterOutline = tempScheme->getColour(T("selectedFilterOutline"));
	colours.yellowButton = tempScheme->getColour(T("yellowButton"));
	colours.blueButton = tempScheme->getColour(T("blueButton"));

	upperLabel->setColour(Label::textColourId, colours.propertyPanelText);
	lowerLabel->setColour(Label::textColourId, colours.propertyPanelText);

	{
		Path p;
		DrawablePath d;

		p.addArrow(50, 90, 50, 10, 20, 60, 40);
		d.setPath(p);
		d.setSolidFill(colours.menuText);

		moveUp->setImages(&d);
	}
	moveUp->setBackgroundColours(colours.blueButton,
								 colours.blueButton.darker(2.5f));
	{
		Path p;
		DrawablePath d;

		p.addArrow(50, 10, 50, 90, 20, 60, 40);
		d.setPath(p);
		d.setSolidFill(colours.menuText);

		moveDown->setImages(&d);
	}
	moveDown->setBackgroundColours(colours.blueButton,
								   colours.blueButton.darker(2.5f));

	exitButton->setBackgroundColours(colours.yellowButton,
									 colours.yellowButton.darker(2.5f));
	exitButton->setTextColour(colours.menuText);
	leftButton1->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton1->setTextColour(colours.menuText);
	leftButton2->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton2->setTextColour(colours.menuText);
	leftButton3->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton3->setTextColour(colours.menuText);
	leftButton4->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton4->setTextColour(colours.menuText);
	leftButton5->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton5->setTextColour(colours.menuText);
	leftButton6->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton6->setTextColour(colours.menuText);
	leftButton7->setBackgroundColours(colours.blueButton,
									  colours.blueButton.darker(2.5f));
	leftButton7->setTextColour(colours.menuText);
	rightButton1->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton1->setTextColour(colours.menuText);
	rightButton2->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton2->setTextColour(colours.menuText);
	rightButton3->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton3->setTextColour(colours.menuText);
	rightButton4->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton4->setTextColour(colours.menuText);
	rightButton5->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton5->setTextColour(colours.menuText);
	rightButton6->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton6->setTextColour(colours.menuText);
	rightButton7->setBackgroundColours(colours.blueButton,
									   colours.blueButton.darker(2.5f));
	rightButton7->setTextColour(colours.menuText);
	clock->setColour(prefScheme->getColour(T("propertyPanelText")));
	lowerPanel->setOutlineColour(colours.propertyPanelOutline);
	lowerPanel->setTextColour(colours.propertyPanelText);
	lowerPanel->setBackgroundColour(colours.propertyPanelBackground);

	repaint();
}

//----------------------------------------------------------------------------------------------
/*void TwindyRootWindow::mouseMove(const MouseEvent& e)
{
	Rectangle temprect;

	//std::cout << "...MouseMove()..." << std::endl;
	//std::cout << "...currentlyInFocus = " << currentlyInFocus << "..." << std::endl;

	//upperPanel first.
	temprect.setBounds(10, 30, (getWidth()-20), (getHeight()-270));
	if((temprect.contains(e.getScreenX(), e.getScreenY())) &&
	   (!currentlyInFocus))
	{
		currentlyInFocus = true;

		if(upperWindow > -1)
			giveWindowFocus(upperWindows[upperWindow]);
		return;
	}

	//Now lowerPanel first.
	temprect.setBounds(220, (getHeight()-225), (getWidth()-400), 220);
	if((temprect.contains(e.getScreenX(), e.getScreenY())) &&
	   (currentlyInFocus))
	{
		currentlyInFocus = false;

		if(lowerWindow)
			giveWindowFocus(lowerWindow);
	}
}*/

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::launchLowerPanel(JUCE_NAMESPACE::String cmd)
{
	if(lowerWindow)
	{
		bool test;
		String tempstr(TRANS("Do you want to close "));

		tempstr << lowerPanel->getTitle() << T("?");
		test = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
											TRANS("LowerPanel"),
											tempstr,
											TRANS("Yes"),
											TRANS("No"));

		if(!test)
			return;

		closeLowerWindow();
		delete lowerWindow;
	}

	destinedForLowerPanel = true;
	launchExecutable(static_cast<const char *>(cmd));
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::loadColours(const String& file)
{
	TracktionScheme tempScheme(file);

	colours.mainBackground = tempScheme.getColour(T("mainBackground"));
	colours.tabAreaBackground = tempScheme.getColour(T("tabAreaBackground"));
	colours.menuText = tempScheme.getColour(T("menuText"));
	colours.propertyPanelBackground = tempScheme.getColour(T("propertyPanelBackground"));
	colours.propertyPanelText = tempScheme.getColour(T("propertyPanelText"));
	colours.propertyPanelOutline = tempScheme.getColour(T("propertyPanelOutline"));
	colours.selectedFilterOutline = tempScheme.getColour(T("selectedFilterOutline"));
	colours.yellowButton = tempScheme.getColour(T("yellowButton"));
	colours.blueButton = tempScheme.getColour(T("blueButton"));
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::launchStartupPrograms()
{
	long i;
	TwindyProperty tempprop;

	tempprop = properties->getProperty(T("StartupPrograms"));

	if(tempprop.name != T("nil"))
	{
		for(i=0;i<properties->getNumSubProperties(T("StartupPrograms"));++i)
		{
			tempprop = properties->getSubProperty(T("StartupPrograms"), i);
			if(tempprop.value != T("nil"))
				launchExecutable(static_cast<const char *>(tempprop.value),
								 true);
		}
	}
}
