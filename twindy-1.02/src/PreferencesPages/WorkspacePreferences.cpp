//	WorkspacePreferences.cpp - Preferences page for the workspaces.
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
#include "WorkspacePreferences.h"
#include "DrawableTextButton.h"
#include "TwindyApp.h"
#include "TwindyProperties.h"
#include "TwindyRootWindow.h"
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
WorkspacePreferences::WorkspacePreferences():
Component()
{
	int i;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();
	TwindyProperties *prop = root->getProperties();

	TwindyProperty *tempProp = new TwindyProperty;
	tempProp->name = T("Preferences");
	tempProp->value = prop->getProperty(T("TracktionScheme")).name;
	workspaces.add(tempProp);
#if 0
	for(i=0;i<prop->getNumSubProperties(T("Workspace"));++i)
	{
		tempProp = new TwindyProperty;
		tempProp->name = prop->getSubProperty(T("Workspace"), i).name;
		tempProp->value = prop->getSubProperty(T("Workspace"), i).value;
		workspaces.add(tempProp);
	}
#endif

	//Workspace name editor
	addAndMakeVisible(textLabel = new Label(T("wtL"), TRANS("Name:")));
	textLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(textEditor = new TextEditor(T("wN")));
	textEditor->addListener(this);

	//Tracktionscheme editor
	addAndMakeVisible(schemeLabel = new Label(T("wtL"), TRANS("Colours:")));
	schemeLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(schemeEditor = new TextEditor(T("tsEd")));
	schemeEditor->addListener(this);
	addAndMakeVisible(schemeLoad = new DrawableTextButton(T("..."), T("...")));
	schemeLoad->addButtonListener(this);

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

	//Workspaces list.
	addAndMakeVisible(workspacesLabel = new Label(T("wL"),
												 TRANS("Workspaces:")));
	workspacesLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(workspacesList = new ListBox(T("Workspaces"), this));
	workspacesList->updateContent();
	workspacesList->selectRow(0);
}

//------------------------------------------------------------------------------
WorkspacePreferences::~WorkspacePreferences()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void WorkspacePreferences::resized()
{
	workspacesLabel->setBounds(10, 0, 150, 20);
	workspacesList->setBounds(10, 25, 150, 280);
	textLabel->setBounds(170, 0, 150, 20);
	textEditor->setBounds(170, 25, 150, 20);
	addItem->setBounds(10, 310, 25, 25);
	removeItem->setBounds(40, 310, 25, 25);
	clearItems->setBounds(70, 310, 90, 25);
	itemUp->setBounds(165, 256, 50, 25);
	itemDown->setBounds(165, 283, 50, 25);
	schemeLabel->setBounds(170, 50, 150, 20);
	schemeEditor->setBounds(170, 75, 250, 20);
	schemeLoad->setBounds(425, 73, 30, 25);
}

//------------------------------------------------------------------------------
void WorkspacePreferences::buttonClicked(Button *button)
{
	int i;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if(button == addItem)
	{
		AlertWindow alex(TRANS("Add a workspace"),
						 T(""),
						 AlertWindow::QuestionIcon);
		alex.addTextEditor(T("name"), T(""), TRANS("Name:"));
		alex.addButton(TRANS("cancel"), 0);
		alex.addButton(TRANS("ok"), 1, KeyPress(KeyPress::returnKey,
											ModifierKeys(),
											'\r'));
		if(alex.runModalLoop())
		{
			TwindyProperty *tempProp = new TwindyProperty;
			tempProp->name = alex.getTextEditorContents(T("name"));
			tempProp->value = T("~/.twindy/Default.tracktionscheme");
			workspaces.add(tempProp);
			workspacesList->updateContent();
			saveChanges();
			root->prefsChanged();
		}
	}
	else if(button == removeItem)
	{
		int tempint = workspacesList->getSelectedRow();
		if(getNumRows() > 0)
		{
			workspaces.remove(tempint);
			if(tempint >= getNumRows())
				tempint = getNumRows()-1;
			if(getNumRows() > 0)
			{
				workspacesList->selectRow(tempint);
				textEditor->setText(workspaces[tempint]->name);
			}
			else
			{
				textEditor->setText(T(""));
			}
			workspacesList->updateContent();
			saveChanges();
			root->prefsChanged();
		}
	}
	else if(button == clearItems)
	{
		for(i=(workspaces.size()-1);i>1;--i)
			workspaces.remove(i);
		workspacesList->updateContent();
		workspacesList->selectRow(1);
		saveChanges();
		root->prefsChanged();
	}
	else if(button == itemUp)
	{
		int tempint = workspacesList->getSelectedRow();
		if((tempint > 0)&&(getNumRows() > 0))
		{
			TwindyProperty tempProp;
			tempProp.name = workspaces[tempint-1]->name;
			tempProp.value = workspaces[tempint-1]->value;
			workspaces[tempint-1]->name = workspaces[tempint]->name;
			workspaces[tempint-1]->value = workspaces[tempint]->value;
			workspaces[tempint]->name = tempProp.name;
			workspaces[tempint]->value = tempProp.value;
			workspacesList->updateContent();
			workspacesList->selectRow(tempint-1);
			saveChanges();
			root->prefsChanged();
		}
	}
	else if(button == itemDown)
	{
		int tempint = workspacesList->getSelectedRow();
		if((tempint < (getNumRows()-1))&&(getNumRows() > 0))
		{
			TwindyProperty tempProp;
			tempProp.name = workspaces[tempint+1]->name;
			tempProp.value = workspaces[tempint+1]->value;
			workspaces[tempint+1]->name = workspaces[tempint]->name;
			workspaces[tempint+1]->value = workspaces[tempint]->value;
			workspaces[tempint]->name = tempProp.name;
			workspaces[tempint]->value = tempProp.value;
			workspacesList->updateContent();
			workspacesList->selectRow(tempint+1);
			saveChanges();
			root->prefsChanged();
		}
	}
	else if(button == schemeLoad)
	{
		FileChooser phil(TRANS("Choose Colours..."),
						 File(T("~/.twindy")),
						 T("*.tracktionscheme"),
						 false);

		if(phil.browseForFileToOpen())
		{
			schemeEditor->setText(phil.getResult().getFullPathName());
			workspaces[workspacesList->getSelectedRow()]->value = schemeEditor->getText();
			saveChanges();
			root->prefsChanged();
		}
	}
}

//------------------------------------------------------------------------------
void WorkspacePreferences::textEditorReturnKeyPressed(TextEditor &editor)
{
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if(editor.getName() == textEditor->getName())
	{
		workspaces[workspacesList->getSelectedRow()]->name = textEditor->getText();
		workspacesList->updateContent();
		saveChanges();
		root->prefsChanged();
	}
	else if(editor.getName() == schemeEditor->getName())
	{
		workspaces[workspacesList->getSelectedRow()]->value = schemeEditor->getText();
		saveChanges();
		root->prefsChanged();
	}
}

//------------------------------------------------------------------------------
void WorkspacePreferences::paintListBoxItem(int rowNumber,
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

	g.drawText(workspaces[rowNumber]->name,
			   5,
			   0,
			   (width-10),
			   18,
			   Justification::left, true);
}

//------------------------------------------------------------------------------
void WorkspacePreferences::selectedRowsChanged(int lastRowselected)
{
	textEditor->setText(workspaces[lastRowselected]->name);
	schemeEditor->setText(workspaces[lastRowselected]->value);
	if(lastRowselected == 0)
	{
		textLabel->setEnabled(false);
		textEditor->setEnabled(false);
		removeItem->setEnabled(false);
		itemUp->setEnabled(false);
		itemDown->setEnabled(false);
	}
	else if(lastRowselected == 1)
	{
		textLabel->setEnabled(true);
		textEditor->setEnabled(true);
		removeItem->setEnabled(false);
		itemUp->setEnabled(false);
		itemDown->setEnabled(true);
	}
	else
	{
		textLabel->setEnabled(true);
		textEditor->setEnabled(true);
		removeItem->setEnabled(true);
		itemUp->setEnabled(true);
		itemDown->setEnabled(true);
	}
	//saveChanges();
}

//------------------------------------------------------------------------------
void WorkspacePreferences::paintOverChildren(Graphics &g)
{
	//Workspaces list.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(8, 23, 154, 284, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(9, 24, 152, 282, 1);

	//Workspace text.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(168, 23, 154, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(169, 24, 152, 22, 1);

	//Workspace text.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(168, 73, 254, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(169, 74, 252, 22, 1);
}

//------------------------------------------------------------------------------
void WorkspacePreferences::setColours(const Colour& backCol,
									  const Colour& textCol,
									  const Colour& buttonCol)
{
	backgroundColour = backCol;
	textColour = textCol;

	workspacesLabel->setColour(Label::textColourId, textColour);
	textLabel->setColour(Label::textColourId, textColour);
	schemeLabel->setColour(Label::textColourId, textColour);
	addItem->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	removeItem->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	clearItems->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	itemUp->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	itemDown->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	schemeLoad->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
}

//------------------------------------------------------------------------------
void WorkspacePreferences::saveChanges()
{
	int i;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if(root)
	{
		TwindyProperties *prop = root->getProperties();

		//Set startup programs.
		TwindyProperty tempProp;
		tempProp.name = workspaces[0]->value;
		tempProp.value = workspaces[0]->value;
		prop->setProperty(T("TracktionScheme"), tempProp);
		prop->clearSubProperties(T("Workspace"));
		for(i=1;i<workspaces.size();++i)
			prop->setSubProperty(T("Workspace"), *(workspaces[i]));
	}
}

//------------------------------------------------------------------------------
void WorkspacePreferences::updateWorkspaces()
{
	/*int i;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getApplicationInstance());
	TwindyRootWindow *root = app->getRootWindow();
	TwindyProperties *prop = root->getProperties();

	workspaces.clear();
	TwindyProperty *tempProp = new TwindyProperty;
	tempProp->name = T("Preferences");
	tempProp->value = prop->getProperty(T("TracktionScheme"));
	for(i=0;i<prop->getNumSubProperties(T("Workspace"));++i)
	{
		tempProp = new TwindyProperty;
		tempProp->name = prop->getSubProperty(T("Workspace"), i).name;
		tempProp->value = prop->getSubProperty(T("Workspace"), i).value;
		workspaces.add(tempProp);
	}
	workspacesList->updateContent();*/
}
