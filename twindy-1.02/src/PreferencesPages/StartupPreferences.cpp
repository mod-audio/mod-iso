//	StartupPreferences.cpp - Preferences page for the startup programs.
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
#include "StartupPreferences.h"
#include "DrawableTextButton.h"
#include "TwindyApp.h"
#include "TwindyRootWindow.h"
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
StartupPreferences::StartupPreferences():
Component(),
lastRow(-1)
{
	//Startup text editor
	addAndMakeVisible(textLabel = new Label(T("stL"), TRANS("Name:")));
	textLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(textEditor = new TextEditor());
	textEditor->addListener(this);

	//Startup executable editor
	addAndMakeVisible(execLabel = new Label(T("stL"), TRANS("Executable:")));
	execLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(execEditor = new TextEditor());
	execEditor->addListener(this);

	//Startup programs list.
	addAndMakeVisible(startupsLabel = new Label(T("sL"),
												 TRANS("Startup Programs:")));
	startupsLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(startups = new ListBox(T("StartupPrograms"), this));

	//Item buttons.
	addAndMakeVisible(addItem = new DrawableTextButton(T("..."), T("+")));
	addItem->addButtonListener(this);
	addAndMakeVisible(removeItem = new DrawableTextButton(T("..."), T("-")));
	removeItem->addButtonListener(this);
	addAndMakeVisible(clearItems = new DrawableTextButton(T("..."), TRANS("clear")));
	clearItems->addButtonListener(this);
	addAndMakeVisible(itemUp = new DrawableTextButton(T("..."), TRANS("up")));
	itemUp->addButtonListener(this);
	addAndMakeVisible(itemDown = new DrawableTextButton(T("..."), TRANS("down")));
	itemDown->addButtonListener(this);

	//Load startup programs from twindyrc.
	loadStartups();
}

//------------------------------------------------------------------------------
StartupPreferences::~StartupPreferences()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void StartupPreferences::resized()
{
	startupsLabel->setBounds(10, 0, 150, 20);
	startups->setBounds(10, 25, 150, 280);
	textLabel->setBounds(170, 0, 150, 20);
	textEditor->setBounds(170, 25, 150, 20);
	execLabel->setBounds(170, 52, 150, 20);
	execEditor->setBounds(170, 77, 150, 20);
	addItem->setBounds(10, 310, 25, 25);
	removeItem->setBounds(40, 310, 25, 25);
	clearItems->setBounds(70, 310, 90, 25);
	itemUp->setBounds(165, 256, 50, 25);
	itemDown->setBounds(165, 283, 50, 25);
}

//------------------------------------------------------------------------------
void StartupPreferences::buttonClicked(Button *button)
{
	if(button == addItem)
	{
		AlertWindow alex(TRANS("Add a startup program"),
						 T(""),
						 AlertWindow::QuestionIcon);
		alex.addTextEditor(T("name"), T(""), TRANS("Text:"));
		alex.addTextEditor(T("value"), T(""), TRANS("Executable:"));
		alex.addButton(TRANS("cancel"), 0);
		alex.addButton(TRANS("ok"), 1, KeyPress(KeyPress::returnKey,
											ModifierKeys(),
											'\r'));
		if(alex.runModalLoop())
		{
			TwindyProperty *tempProp = new TwindyProperty;
			tempProp->name = alex.getTextEditorContents(T("name"));
			tempProp->value = alex.getTextEditorContents(T("value"));
			startup.add(tempProp);
			startups->updateContent();
			saveChanges();
		}
	}
	else if(button == removeItem)
	{
		int tempint = startups->getSelectedRow();
		if(getNumRows() > 0)
		{
			startup.remove(tempint);
			if(tempint >= getNumRows())
				tempint = getNumRows()-1;
			if(getNumRows() > 0)
			{
				startups->selectRow(tempint);
				textEditor->setText(startup[tempint]->name);
				execEditor->setText(startup[tempint]->value);
			}
			else
			{
				textEditor->setText(T(""));
				execEditor->setText(T(""));
			}
			startups->updateContent();
			saveChanges();
		}
	}
	else if(button == clearItems)
	{
		startup.clear();
		startups->updateContent();
		textEditor->setText(T(""));
		execEditor->setText(T(""));
		saveChanges();
	}
	else if(button == itemUp)
	{
		int tempint = startups->getSelectedRow();
		if((tempint > 0)&&(getNumRows() > 0))
		{
			TwindyProperty tempProp;
			tempProp.name = startup[tempint-1]->name;
			tempProp.value = startup[tempint-1]->value;
			startup[tempint-1]->name = startup[tempint]->name;
			startup[tempint-1]->value = startup[tempint]->value;
			startup[tempint]->name = tempProp.name;
			startup[tempint]->value = tempProp.value;
			startups->updateContent();
			//Make sure selectRow doesn't screw things up.
			textEditor->setText(startup[tempint]->name);
			execEditor->setText(startup[tempint]->value);
			startups->selectRow(tempint-1);
			saveChanges();
		}
	}
	else if(button == itemDown)
	{
		int tempint = startups->getSelectedRow();
		if((tempint < (getNumRows()-1))&&(getNumRows() > 0))
		{
			TwindyProperty tempProp;
			tempProp.name = startup[tempint+1]->name;
			tempProp.value = startup[tempint+1]->value;
			startup[tempint+1]->name = startup[tempint]->name;
			startup[tempint+1]->value = startup[tempint]->value;
			startup[tempint]->name = tempProp.name;
			startup[tempint]->value = tempProp.value;
			startups->updateContent();
			//Make sure selectRow doesn't screw things up.
			textEditor->setText(startup[tempint]->name);
			execEditor->setText(startup[tempint]->value);
			startups->selectRow(tempint+1);
			saveChanges();
		}
	}
}

//------------------------------------------------------------------------------
void StartupPreferences::textEditorReturnKeyPressed(TextEditor &editor)
{
	if((lastRow > -1)&&(startup.size() > 0))
	{
		startup[lastRow]->name = textEditor->getText();
		startup[lastRow]->value = execEditor->getText();
	}
	startups->updateContent();
	saveChanges();
}

//------------------------------------------------------------------------------
void StartupPreferences::paintListBoxItem(int rowNumber,
										   Graphics &g,
										   int width,
										   int height,
										   bool rowIsSelected)
{
	String tempstr;

	if(rowIsSelected)
	{
		Colour tempCol = backgroundColour.contrasting();
		if(tempCol.brighter(0.5f) == Colours::white)
			tempCol = backgroundColour;
		g.setColour(tempCol.withAlpha(0.2f));
		g.fillRect(0, 0, width, 25);
	}

	g.setColour(Colours::black);

	g.drawText(startup[rowNumber]->name,
			   5,
			   0,
			   (width-10),
			   18,
			   Justification::left, true);
}

//------------------------------------------------------------------------------
void StartupPreferences::selectedRowsChanged(int lastRowselected)
{
	if(startup.size() > 0)
	{
		if((lastRow > -1) && (lastRow < startup.size()))
		{
			startup[lastRow]->name = textEditor->getText();
			startup[lastRow]->value = execEditor->getText();
		}
		textEditor->setText(startup[lastRowselected]->name);
		execEditor->setText(startup[lastRowselected]->value);
		lastRow = lastRowselected;
	}
}

//------------------------------------------------------------------------------
void StartupPreferences::paintOverChildren(Graphics &g)
{
	//Startup program list.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(8, 23, 154, 284, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(9, 24, 152, 282, 1);

	//Startup text.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(168, 23, 154, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(169, 24, 152, 22, 1);

	//Startup executable.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(168, 75, 154, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(169, 76, 152, 22, 1);
}

//------------------------------------------------------------------------------
void StartupPreferences::setColours(const Colour& backCol,
									const Colour& textCol,
									const Colour& buttonCol)
{
	backgroundColour = backCol;
	textColour = textCol;

	startupsLabel->setColour(Label::textColourId, textColour);
	textLabel->setColour(Label::textColourId, textColour);
	execLabel->setColour(Label::textColourId, textColour);
	addItem->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	removeItem->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	clearItems->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	itemUp->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	itemDown->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
}

//------------------------------------------------------------------------------
void StartupPreferences::saveChanges()
{
	int i;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if(root)
	{
		TwindyProperties *prop = root->getProperties();

		//Set startup programs.
		prop->clearSubProperties(T("StartupPrograms"));
		for(i=0;i<startup.size();++i)
			prop->setSubProperty(T("StartupPrograms"), *(startup[i]));
	}
}

//------------------------------------------------------------------------------
void StartupPreferences::loadStartups()
{
	long i;
	TwindyProperty tempprop;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if(root)
	{
		TwindyProperties *prop = root->getProperties();
		tempprop = prop->getProperty(T("StartupPrograms"));
	
		if(tempprop.name != T("nil"))
		{
			startup.clear();
			for(i=0;i<prop->getNumSubProperties(T("StartupPrograms"));++i)
			{
				TwindyProperty *tempProp2 = new TwindyProperty;
				TwindyProperty tempProp = prop->getSubProperty(T("StartupPrograms"), i);

				tempProp2->name = tempProp.name;
				tempProp2->value = tempProp.value;
				startup.add(tempProp2);

				if(i==0)
				{
					textEditor->setText(tempProp.name);
					execEditor->setText(tempProp.value);
				}
			}
			startups->updateContent();
			if(startup.size() > 0)
				startups->selectRow(0);
		}
	}
}
