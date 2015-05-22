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
#include "TwindyTabs.h"
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
#include "Utils.h"

//----------------------------------------------------------------------------------------------
TwindyRootWindow::TwindyRootWindow()
    : Component(T("Twindy Window Manager")),
      currentUpperPanelComp(0),
      exitButton(0),
      properties(0)
{
    static_cast<TwindyApp*>(JUCEApplication::getInstance())->setRootWindow(this);

    // Construct this first to make sure we get any error messages that might be generated from the following code.
    TwindyErrorDisplay* const errorDisplay(new TwindyErrorDisplay());
    errorDisplay->setBackgroundColour(colours.propertyPanelBackground);
    errorDisplay->setTextColour(colours.propertyPanelText);

    // Get coordinates (to know where to put things)
    const Coordinates& coords(getCoordinates());

    // TODO: remove this
    properties = new TwindyProperties();

    TwindyProperty tempProp;

    // set LookAndFeel and Font
    {
        tempProp.name = T("DejaVu Sans");
        tempProp.value = T("DejaVu Sans");

        TwindyLAF* const laf(new TwindyLAF());
        laf->setDefaultSansSerifTypefaceName(tempProp.name);
        LookAndFeel::setDefaultLookAndFeel(laf);
    }

#if 0
    //Setup text localisation.
    tempProp = properties->getProperty(T("LocalisationFile"));
    if ((tempProp.name != T("nil")) && (tempProp.value != T("")))
    {
        LocalisedStrings* const tempStrings(LocalisedStrings(File(tempProp.value)));
        LocalisedStrings::setCurrentMappings(tempStrings);
    }
#endif

    // Load colours.
    loadColours(properties->getProperty(T("TracktionScheme")).name);

    // This is so our 'root' window never obscures it's 'children'.
    setBroughtToFrontOnMouseClick(false);

    MessageManager::getInstance()->registerMapRequestCallback(this);

    // Set up workspace tabs.
    workspaces = new TwindyTabs();
    workspaces->setOutline(0);
    workspaces->setTabBarDepth(coords.topTabBarHeight);
    workspaces->setIndent(coords.topTabBarIndent);
    addAndMakeVisible(workspaces);

    // Setup workspaces.
    {
        TwindyUpperPanel* const panelMOD(new TwindyUpperPanel(true));
        panelMOD->setName(T("MOD Workspace"));
        panelMOD->setTabOrientation(TwindyTabbedComponent::TabsAtLeft, 0, 0, 1);

        panelMOD->setColours(colours.propertyPanelBackground,
                             colours.propertyPanelText,
                             colours.selectedFilterOutline,
                             colours.propertyPanelOutline,
                             colours.mainBackground);

        upperPanelComps[0] = panelMOD;
        workspaces->addTab(panelMOD->getName(), Colour(37, 37, 37), panelMOD, false);

        if (true)
        {
            TwindyUpperPanel* const panelDEV(new TwindyUpperPanel(false));
            panelDEV->setName(T("Dev Tools"));
            panelDEV->setTabOrientation(TwindyTabbedComponent::TabsAtLeft, coords.leftTabsWidth, coords.leftTabsWidth, 1);

            panelDEV->setColours(colours.propertyPanelBackground,
                                 colours.propertyPanelText,
                                 colours.selectedFilterOutline,
                                 colours.propertyPanelOutline,
                                 colours.mainBackground);

            upperPanelComps[1] = panelDEV;
            workspaces->addTab(panelDEV->getName(), Colour(37, 37, 37), panelDEV, false);
        }
    }

    // Setup up preferences tab.
    preferences = new TwindyPreferences();
    preferences->setTracktionScheme(properties->getProperty(T("TracktionScheme")).name);
    preferences->setPrefColours(colours.propertyPanelBackground,
                                colours.propertyPanelText,
                                colours.selectedFilterOutline,
                                colours.propertyPanelOutline,
                                colours.mainBackground,
                                colours.blueButton);
    preferences->setTabOrientation(TwindyTabbedComponent::TabsAtLeft, coords.leftTabsWidth, coords.leftTabsWidth, 1);
    workspaces->addTab(TRANS("Settings"), colours.mainBackground, preferences, false);

    //Set up buttons.
    addAndMakeVisible(exitButton = new DrawableTextButton(T("ExitButton"), TRANS("Power Off...")));
    exitButton->setBackgroundColours(colours.yellowButton, colours.yellowButton.darker(2.5f));
    exitButton->setTextColour(colours.menuText);
    exitButton->addButtonListener(this);

    addAndMakeVisible(leftButton1 = new DrawableTextButton(T("LeftButton1"), TRANS("Ingen (UI)")));
    leftButton1->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton1->setTextColour(colours.menuText);
    leftButton1->addButtonListener(this);

    addAndMakeVisible(leftButton2 = new DrawableTextButton(T("LeftButton2"), TRANS("Patchage")));
    leftButton2->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton2->setTextColour(colours.menuText);
    leftButton2->addButtonListener(this);

    addAndMakeVisible(leftButton3 = new DrawableTextButton(T("LeftButton3"), TRANS("Jaaa")));
    leftButton3->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton3->setTextColour(colours.menuText);
    leftButton3->addButtonListener(this);

    addAndMakeVisible(leftButton4 = new DrawableTextButton(T("LeftButton4"), TRANS("Japa")));
    leftButton4->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton4->setTextColour(colours.menuText);
    leftButton4->addButtonListener(this);

    addAndMakeVisible(leftButton5 = new DrawableTextButton(T("LeftButton5"), TRANS("TODO")));
    leftButton5->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton5->setTextColour(colours.menuText);
    leftButton5->addButtonListener(this);

    addAndMakeVisible(leftButton6 = new DrawableTextButton(T("LeftButton6"), TRANS("TODO")));
    leftButton6->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton6->setTextColour(colours.menuText);
    leftButton6->addButtonListener(this);

    addAndMakeVisible(leftButton7 = new DrawableTextButton(T("LeftButton7"), TRANS("Terminal")));
    leftButton7->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton7->setTextColour(colours.menuText);
    leftButton7->addButtonListener(this);

    leftButton4->setVisible(false);
    leftButton5->setVisible(false);
    leftButton6->setVisible(false);

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

    // Final setup
    addAndMakeVisible(errorDisplay);
    errorDisplay->toFront(false);

    setButtonsVisible(false);
    workspaces->setCurrentTabIndex(0);
}

//----------------------------------------------------------------------------------------------
TwindyRootWindow::~TwindyRootWindow()
{
    // Kill any programs we ran via buttons.
    for (int i=pidArray.size(); --i>=0;)
        terminateAndWaitForProcess(pidArray[i]);

    // Just in case..
    Thread::yield();

    deleteAllChildren();

    delete preferences;
    delete properties;
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::paint(Graphics &g)
{
    const Coordinates& coords(getCoordinates());

    g.setColour(colours.tabAreaBackground);
    g.fillRect(0, 0, getWidth(), coords.topTabBarHeight);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::resized()
{
    const Coordinates& coords(getCoordinates());

    workspaces->setBounds(0, 0, getWidth(), getHeight());

    const int lowerBorder = getHeight()-coords.buttonHeight*3/2;
    /* */ int tempint     = lowerBorder;

    exitButton->setBounds(coords.buttonMargin, tempint, coords.buttonWidth, coords.buttonHeight);
    tempint -= coords.buttonHeight*3/2;

    leftButton7->setBounds(coords.buttonMargin, tempint, coords.buttonWidth, coords.buttonHeight);
    tempint -= coords.buttonHeight;
    //leftButton6->setBounds(coords.buttonMargin, tempint, coords.buttonWidth, coords.buttonHeight);
    //tempint -= coords.buttonHeight;
    //leftButton5->setBounds(coords.buttonMargin, tempint, coords.buttonWidth, coords.buttonHeight);
    //tempint -= coords.buttonHeight;
    //leftButton4->setBounds(coords.buttonMargin, tempint, coords.buttonWidth, coords.buttonHeight);
    //tempint -= coords.buttonHeight;
    leftButton3->setBounds(coords.buttonMargin, tempint, coords.buttonWidth, coords.buttonHeight);
    tempint -= coords.buttonHeight;
    leftButton2->setBounds(coords.buttonMargin, tempint, coords.buttonWidth, coords.buttonHeight);
    tempint -= coords.buttonHeight;
    leftButton1->setBounds(coords.buttonMargin, tempint, coords.buttonWidth, coords.buttonHeight);
    tempint -= coords.buttonHeight;

    clock->setBounds(getWidth()-coords.clockWidth, 0, coords.clockWidth, coords.topTabBarHeight);

    TwindyErrorDisplay::getInstance()->setBounds(getWidth()-coords.buttonMargin-coords.errorDisplayWidth,
                                                 lowerBorder-coords.errorDisplayHeight,
                                                 coords.errorDisplayWidth,
                                                 coords.errorDisplayHeight);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::buttonClicked(Button* button)
{
    StringArray args;

    if (button == exitButton)
    {
        if (AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                         TRANS("Log Off..."),
                                         TRANS("Are you sure you want to quit Live-MOD?"),
                                         TRANS("Yes"),
                                         TRANS("No")))
        {
            StringArray args;

            args.add(T("dbus-send"));
            args.add(T("--system"));
            args.add(T("--print-reply"));
            args.add(T("--dest=\"org.freedesktop.ConsoleKit\""));
            args.add(T("/org/freedesktop/ConsoleKit/Manager"));
            args.add(T("org.freedesktop.ConsoleKit.Manager.Stop"));

            startProcess(args);
            JUCEApplication::quit();
        }
        return;
    }

    /**/ if (button == leftButton1)
    {
        args.add(T("ingen"));
        args.add(T("-c"));
        args.add(T("unix:///tmp/mod-app-8888.sock"));
        args.add(T("-g"));
    }
    else if (button == leftButton2)
    {
        args.add(T("patchage"));
    }
    else if (button == leftButton3)
    {
        args.add(T("jaaa"));
        args.add(T("-J"));
    }
    else if (button == leftButton4)
    {
        args.add(T("japa"));
        args.add(T("-J"));
    }
    else if (button == leftButton5)
    {
        args.add(T("terminator")); // TODO
    }
    else if (button == leftButton6)
    {
        args.add(T("terminator")); // TODO
    }
    else if (button == leftButton7)
    {
        args.add(T("terminator"));
        args.add(T("-u"));
        args.add(T("-T"));
        args.add(T("Terminal"));
    }
    else
    {
        // who's there!?
        return;
    }

    pidArray.add(startProcess(args));
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::setVisibleUpperPanel(uint index)
{
    // Hide currently-visible window, if this isn't the preferences panel.
    if (currentUpperPanelComp < upperPanelCompSize)
    {
        if (TwindyUpperPanel* const panel = upperPanelComps[currentUpperPanelComp])
            panel->setWorkspaceIsVisible(false);
    }
//     else if (currentUpperPanelComp == upperPanelCompSize)
//     {
//         if (TwindyWindow* const window = preferences->getMixerWindow())
//             window->hide();
//     }

    // Set currentUpperPanelComp, making sure index doesn't point to a non-existant tab.
    /**/ if (index == upperPanelCompSize)
        currentUpperPanelComp = index;
    else if (index < upperPanelCompSize && upperPanelComps[index] != nullptr)
        currentUpperPanelComp = index;
    else
        currentUpperPanelComp = 0;

    // Show newly-visible window, if it isn't the preferences panel.
    if (currentUpperPanelComp < upperPanelCompSize)
        upperPanelComps[currentUpperPanelComp]->setWorkspaceIsVisible(true);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::updateColours()
{
    const TracktionScheme* tempScheme;
    const TracktionScheme* prefScheme;

    if (! exitButton)
        return;

    if (currentUpperPanelComp == upperPanelCompSize)
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

    exitButton->setBackgroundColours(colours.yellowButton, colours.yellowButton.darker(2.5f));
    exitButton->setTextColour(colours.menuText);
    leftButton1->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton1->setTextColour(colours.menuText);
    leftButton2->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton2->setTextColour(colours.menuText);
    leftButton3->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton3->setTextColour(colours.menuText);
    leftButton4->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton4->setTextColour(colours.menuText);
    leftButton5->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton5->setTextColour(colours.menuText);
    leftButton6->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton6->setTextColour(colours.menuText);
    leftButton7->setBackgroundColours(colours.blueButton, colours.blueButton.darker(2.5f));
    leftButton7->setTextColour(colours.menuText);

    clock->setColour(prefScheme->getColour(T("propertyPanelText")));

    repaint();
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
void TwindyRootWindow::setButtonsVisible(const bool visible)
{
    printf("setButtonsVisible(%i)\n", visible);

    if (! exitButton)
        return;

    //exitButton->setVisible(visible);

    leftButton1->setVisible(visible);
    leftButton2->setVisible(visible);
    leftButton3->setVisible(visible);
    //leftButton4->setVisible(visible);
    //leftButton5->setVisible(visible);
    //leftButton6->setVisible(visible);
    leftButton7->setVisible(visible);
}

//----------------------------------------------------------------------------------------------
void TwindyRootWindow::changeToAppTab()
{
    workspaces->setCurrentTabIndex(0);
}
