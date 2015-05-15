//	TwindyUpperPanel.cpp - Component to hold windows in the upper panel.
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
#include "TwindyUpperPanel.h"
#include "TwindyRootWindow.h"
#include "TracktionScheme.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
TwindyUpperPanel::TwindyUpperPanel(bool isMOD):
TwindyTabbedComponent(T("Upper Panel"), isMOD ? TwindyTabbedComponent::ModeMOD : TwindyTabbedComponent::ModeDev),
workspaceVisible(false),
dontShowHideWindows(false),
visibleWindow(-1)
{
	colours = new TracktionScheme(T("~/.twindy/Default.tracktionscheme"));
}

//------------------------------------------------------------------------------
TwindyUpperPanel::~TwindyUpperPanel()
{
	int i;
	//The contents of upperWindows get deleted when it's deleted, so we don't
	//have to explicitly delete them here.
	for(i=0;i<windows.size();i++)
	{
		windows[i]->closeWindow();
		Thread::yield();
	}
	delete colours;
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::setWorkspaceIsVisible(bool val)
{
	workspaceVisible = val;

	if(workspaceVisible)
	{
		//Make first window visible.
		if(windows.size() > 0)
		{
			visibleWindow = 0;
			setCurrentlySelectedTab(0);
			windows[visibleWindow]->show();
		}
	}
	else
	{
		//Hide visible window.
		if(windows.size() > 0)
			windows[visibleWindow]->hide();
	}
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::addWindow(TwindyWindow *newWindow, TwindyUpperTab *newTab)
{
	addTab(getNumTabs(), newTab);
	windows.add(newWindow);
	visibleWindow = windows.size()-1;
	setCurrentlySelectedTab(getNumTabs()-1);
	if(!workspaceVisible)
		newWindow->hide();
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::removeWindow(Window window)
{
	int i;

	for(i=0;i<windows.size();i++)
	{
		if(windows[i]->getWindow() == window)
		{
			if((windows.size()-1) > 0)
			{
				visibleWindow--;
				if(visibleWindow == -1)
					visibleWindow = 0;
			}
			else
				visibleWindow = -1;
			removeTab(i);
			windows.remove(i);

			if(windows.size() > 0)
			{
				windows[visibleWindow]->show();
				setCurrentlySelectedTab(visibleWindow);
			}
			break;
		}
	}
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::removeIndexedWindow(int index)
{
	windows.remove(index, false);
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::removeWindowNoDelete(TwindyWindow *window)
{
	int i;

	for(i=0;i<windows.size();++i)
	{
		if(windows[i] == window)
		{
			if((windows.size()-1) > 0)
			{
				visibleWindow--;
				if(visibleWindow == -1)
					visibleWindow = 0;
			}
			else
				visibleWindow = -1;
			removeTab(i);
			windows.remove(i, false);

			if(windows.size() > 0)
			{
				windows[visibleWindow]->show();
				setCurrentlySelectedTab(visibleWindow);
			}
		}
	}
}

//------------------------------------------------------------------------------
TwindyWindow *TwindyUpperPanel::getCurrentWindow()
{
	TwindyWindow *retval;

	if(workspaceVisible && (visibleWindow > -1))
		retval = windows[visibleWindow];
	else
		retval = 0;

	return retval;
}

//------------------------------------------------------------------------------
TwindyWindow *TwindyUpperPanel::getWindow(int index)
{
	if(index > windows.size())
		return 0;
	else
		return windows[index];
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::setName(const String& newName)
{
	name = newName;
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::tabWasClicked(int newSelectedIndex,
                                     bool clickedTabWasAlreadySelected,
                                     const ModifierKeys &currentModifiers)
{
	if(!clickedTabWasAlreadySelected)
	{
		if((visibleWindow != newSelectedIndex) && !dontShowHideWindows)
		{
			windows[visibleWindow]->hide();
			visibleWindow = newSelectedIndex;
			windows[newSelectedIndex]->show();
		}
		else if(dontShowHideWindows)
			dontShowHideWindows = false;
	}
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::mouseMove(const MouseEvent& e)
{
	getParentComponent()->mouseMove(e);
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::closeTab(TwindyUpperTab *tab)
{
	int i;

	for(i=0;i<getNumTabs();i++)
	{
		if(getTabComponent(i) == tab)
		{
			closeWindow(tab, i);
			break;
		}
	}
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::setSchemePath(const String& path)
{
	coloursPath = path;

	delete colours;
	colours = new TracktionScheme(coloursPath);
}

//------------------------------------------------------------------------------
void TwindyUpperPanel::closeWindow(TwindyUpperTab *tab, int index)
{
	bool reselectTab = false;

	//I think there's probably some redundant code here...

	if((windows.size()-1) > 0)
	{
		if(visibleWindow != 0)
		{
			visibleWindow--;
			windows[visibleWindow]->show();
			setCurrentlySelectedTab(visibleWindow);
		}
		else
		{
			windows[visibleWindow+1]->show();
			setCurrentlySelectedTab(visibleWindow+1);
			reselectTab = true;
		}
	}
	else
		visibleWindow = -1;
	windows[index]->closeWindow();
	removeTab(static_cast<TwindyTabbedComponentBasicTab *>(tab));
	windows.remove(index);

	if((windows.size()-1) > 0)
		windows[getCurrentlySelectedTab()]->show();

	if(reselectTab)
	{
		dontShowHideWindows = true;
		setCurrentlySelectedTab(visibleWindow);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TwindyUpperTab::TwindyUpperTab(const String &text, Colour textCol, Colour fillCol):
TwindyTabbedComponentBasicTab(text)
{
	this->text = textCol;
	fill = fillCol;

	addAndMakeVisible(closeButton = new ShapeButton(T("closeButton"),
													textCol,
													textCol,
													fill.darker()));
	closeButton->addButtonListener(this);
	Path x;
	x.addLineSegment(0.0f, 0.0f, 10.0f, 10.0f, 3.0f);
	x.addLineSegment(10.0f, 0.0f, 0.0f, 10.0f, 3.0f);
	closeButton->setShape(x, false, true, true);
}

//------------------------------------------------------------------------------
TwindyUpperTab::~TwindyUpperTab()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void TwindyUpperTab::paintTab(Graphics &g,
							  bool isTheCurrentTab,
							  const TabbedComponent *ownerTabbedComp)
{
	g.setColour ((isMouseOver()) ? Colour::contrasting (text, fill)
                                 : text);

    g.setFont (jmin (20.0f, getHeight() * 0.8f));

    g.drawText (getName(), 4, 2, getWidth() - 8, getHeight() - 4,
                Justification::centred, true);
}

//------------------------------------------------------------------------------
void TwindyUpperTab::resized()
{
	closeButton->setBounds(6, 6, 15, 15);
}

//------------------------------------------------------------------------------
void TwindyUpperTab::buttonClicked(JUCE_NAMESPACE::Button* button)
{
	TwindyUpperPanel *parent = static_cast<TwindyUpperPanel *>(getParentComponent()->getParentComponent());

	if(button == closeButton)
		parent->closeTab(this);
}
