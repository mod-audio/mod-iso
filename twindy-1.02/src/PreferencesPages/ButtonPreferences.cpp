//	ButtonPreferences.cpp - Preferences page for the buttons.
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
#include "ButtonPreferences.h"
#include "TwindyToggleButton.h"
#include "TwindyRootWindow.h"
#include "TwindyErrorDisplay.h"
#include "TwindyApp.h"
#include "DrawableTextButton.h"
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
TwindyButtonPreferences::TwindyButtonPreferences():
Component(),
lastButton(0),
justConstructed(true)
{
	//Button text editor
	addAndMakeVisible(textLabel = new Label(T("btL"), TRANS("Text:")));
	textLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(textEditor = new TextEditor(T("tEd")));
	textEditor->addListener(this);

	//Menu toggle button.
	addAndMakeVisible(buttonIsMenu = new TwindyToggleButton(TRANS("Button is menu")));
	buttonIsMenu->addButtonListener(this);

	//Button executable editor
	addAndMakeVisible(execLabel = new Label(T("btL"), TRANS("Executable:")));
	execLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(execEditor = new TextEditor(T("eEd")));
	execEditor->addListener(this);

	//Menu items.
	addAndMakeVisible(menuStuff = new ButtonMenuComponent());
	menuStuff->setEnabled(false);

	//Buttons list.
	addAndMakeVisible(buttonsLabel = new Label(T("bL"), TRANS("Button:")));
	buttonsLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(buttons = new ListBox(T("Buttons"), this));
	buttons->updateContent();
	buttons->selectRow(0);
}

//------------------------------------------------------------------------------
TwindyButtonPreferences::~TwindyButtonPreferences()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void TwindyButtonPreferences::resized()
{
	buttonsLabel->setBounds(10, 0, 150, 20);
	buttons->setBounds(10, 25, 150, 308);
	textLabel->setBounds(170, 0, 150, 20);
	textEditor->setBounds(170, 25, 150, 20);
	buttonIsMenu->setBounds(170, 50, 150, 25);
	execLabel->setBounds(170, 77, 150, 20);
	execEditor->setBounds(170, 102, 150, 20);
	menuStuff->setBounds(330, 0, (getWidth()-340), getHeight());
}

//------------------------------------------------------------------------------
void TwindyButtonPreferences::buttonClicked(Button *button)
{
	if(button == buttonIsMenu)
	{
		if(buttonIsMenu->getToggleState())
		{
			execEditor->setText(T(""));
			execEditor->setEnabled(false);
			execLabel->setEnabled(false);
			menuStuff->setEnabled(true);
		}
		else
		{
			execEditor->setEnabled(true);
			execLabel->setEnabled(true);
			menuStuff->setEnabled(false);
			menuStuff->clearMenuItems();
		}
		saveChanges();
	}
}

//------------------------------------------------------------------------------
void TwindyButtonPreferences::textEditorReturnKeyPressed(TextEditor &editor)
{
	if(editor.getName() == textEditor->getName())
		saveChanges();
	else if(editor.getName() == execEditor->getName())
		saveChanges();
}

//------------------------------------------------------------------------------
void TwindyButtonPreferences::paintListBoxItem(int rowNumber,
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
	if(rowNumber < 7)
		tempstr << TRANS("Left Button ") << (rowNumber+1);
	else
		tempstr << TRANS("Right Button ") << (rowNumber-6);

	g.drawText(tempstr,
			   5,
			   0,
			   (width-10),
			   18,
			   Justification::left, false);
}

//------------------------------------------------------------------------------
void TwindyButtonPreferences::selectedRowsChanged(int lastRowselected)
{
	int i;
	String tempstr;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	/*if(!justConstructed)
		saveChanges();*/
	lastButton = lastRowselected;

	if(root)
	{
		TwindyProperties *prop = root->getProperties();
		//Set button text.
		if(lastRowselected < 7)
		{
			++lastRowselected;
			tempstr << T("LeftButton") << lastRowselected;
		}
		else
		{
			lastRowselected -= 6;
			tempstr << T("RightButton") << lastRowselected;
		}
		textEditor->setText(prop->getProperty(tempstr).name);

		//If it's not a menu button.
		if(prop->getNumSubProperties(tempstr) == 0)
		{
			buttonIsMenu->setToggleState(false, false);
			//execEditor->setEnabled(true);
			execEditor->setText(prop->getProperty(tempstr).value);
			menuStuff->clearMenuItems();
			execEditor->setEnabled(true);
			execLabel->setEnabled(true);
			menuStuff->setEnabled(false);
		}
		else //It is a menu button.
		{
			//buttonIsMenu->setToggleState(true, (!justConstructed));
			buttonIsMenu->setToggleState(true, false);
			menuStuff->clearMenuItems();
			for(i=0;i<prop->getNumSubProperties(tempstr);++i)
			{
				TwindyProperty *tempProp = new TwindyProperty;
				tempProp->name = prop->getSubProperty(tempstr, i).name;
				tempProp->value = prop->getSubProperty(tempstr, i).value;
				menuStuff->addMenuItem(tempProp);
			}
			execEditor->setText(T(""));
			execEditor->setEnabled(false);
			execLabel->setEnabled(false);
			menuStuff->setEnabled(true);
		}
	}
	else
	{
		TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("Button Selector Error"),
														   TRANS("Could not determine the root window!"));
	}
	if(justConstructed)
		justConstructed = false;
}

//------------------------------------------------------------------------------
void TwindyButtonPreferences::paintOverChildren(Graphics &g)
{
	//Button list.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(8, 23, 154, 312, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(9, 24, 152, 310, 1);

	//Button text.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(168, 23, 154, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(169, 24, 152, 22, 1);

	//Button executable.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(168, 100, 154, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(169, 101, 152, 22, 1);
}

//------------------------------------------------------------------------------
void TwindyButtonPreferences::setColours(const Colour& backCol, const Colour& textCol, const Colour& buttonCol)
{
	backgroundColour = backCol;
	textColour = textCol;

	buttonsLabel->setColour(Label::textColourId, textColour);
	textLabel->setColour(Label::textColourId, textColour);
	buttonIsMenu->setColours(backgroundColour, textColour);
	execLabel->setColour(Label::textColourId, textColour);
	menuStuff->setColours(backgroundColour, textColour, buttonCol);
}

//------------------------------------------------------------------------------
void TwindyButtonPreferences::saveChanges()
{
	int i;
	String tempstr;
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if(root)
	{
		TwindyProperties *prop = root->getProperties();

		if(lastButton < 7)
			tempstr << T("LeftButton") << (lastButton+1);
		else
			tempstr << T("RightButton") << (lastButton-6);

		if(!buttonIsMenu->getToggleState())
		{
			TwindyProperty tempProp;
			tempProp.name = textEditor->getText();
			tempProp.value = execEditor->getText();
			prop->clearSubProperties(tempstr);
			prop->setProperty(tempstr, tempProp);
		}
		else
		{
			//Set menu text.
			TwindyProperty tempProp;
			tempProp.name = textEditor->getText();
			tempProp.value = T("");
			prop->setProperty(tempstr, tempProp);

			//Set menu items.
			prop->clearSubProperties(tempstr);
			const OwnedArray<TwindyProperty> * const menu = menuStuff->getMenuItems();
			for(i=0;i<menu->size();++i)
				prop->setSubProperty(tempstr, *((*menu)[i]));
		}

		root->prefsChanged();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ButtonMenuComponent::ButtonMenuComponent():
Component(),
lastRow(-1)
{
	//Menu text editor
	addAndMakeVisible(textLabel = new Label(T("mitL"), TRANS("Text:")));
	textLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(textEditor = new TextEditor());
	textEditor->addListener(this);

	//Menu executable editor
	addAndMakeVisible(execLabel = new Label(T("mitL"), TRANS("Executable:")));
	execLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(execEditor = new TextEditor());
	execEditor->addListener(this);

	//Menu items list.
	addAndMakeVisible(menuItemsLabel = new Label(T("miL"), TRANS("Menu Items:")));
	menuItemsLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(menuItems = new ListBox(T("MenuItems"), this));

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
}

//------------------------------------------------------------------------------
ButtonMenuComponent::~ButtonMenuComponent()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::resized()
{
	menuItemsLabel->setBounds(10, 0, 150, 20);
	menuItems->setBounds(10, 25, 150, 280);
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
void ButtonMenuComponent::buttonClicked(Button *button)
{
	TwindyButtonPreferences *tempParent;

	tempParent = static_cast<TwindyButtonPreferences *>(getParentComponent());
	if(button == addItem)
	{
		AlertWindow alex(TRANS("Add a menu item"),
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
			menu.add(tempProp);
			menuItems->updateContent();
			tempParent->saveChanges();
		}
	}
	else if(button == removeItem)
	{
		int tempint = menuItems->getSelectedRow();
		if(getNumRows() > 0)
		{
			menu.remove(tempint);
			if(tempint >= getNumRows())
				tempint = getNumRows()-1;
			if(getNumRows() > 0)
			{
				menuItems->selectRow(tempint);
				textEditor->setText(menu[tempint]->name);
				execEditor->setText(menu[tempint]->value);
			}
			else
			{
				textEditor->setText(T(""));
				execEditor->setText(T(""));
			}
			menuItems->updateContent();
			tempParent->saveChanges();
		}
	}
	else if(button == clearItems)
	{
		menu.clear();
		menuItems->updateContent();
		textEditor->setText(T(""));
		execEditor->setText(T(""));
		tempParent->saveChanges();
	}
	else if(button == itemUp)
	{
		int tempint = menuItems->getSelectedRow();
		if((tempint > 0)&&(getNumRows() > 1))
		{
			TwindyProperty tempProp;
			tempProp.name = menu[tempint-1]->name;
			tempProp.value = menu[tempint-1]->value;
			menu[tempint-1]->name = menu[tempint]->name;
			menu[tempint-1]->value = menu[tempint]->value;
			menu[tempint]->name = tempProp.name;
			menu[tempint]->value = tempProp.value;
			menuItems->updateContent();
			//Make sure selectRow doesn't screw things up.
			textEditor->setText(menu[tempint]->name);
			execEditor->setText(menu[tempint]->value);
			menuItems->selectRow(tempint-1);
			tempParent->saveChanges();
		}
	}
	else if(button == itemDown)
	{
		int tempint = menuItems->getSelectedRow();
		if((tempint < (getNumRows()-1))&&(getNumRows() > 1))
		{
			TwindyProperty tempProp;
			tempProp.name = menu[tempint+1]->name;
			tempProp.value = menu[tempint+1]->value;
			menu[tempint+1]->name = menu[tempint]->name;
			menu[tempint+1]->value = menu[tempint]->value;
			menu[tempint]->name = tempProp.name;
			menu[tempint]->value = tempProp.value;
			menuItems->updateContent();
			//Make sure selectRow doesn't screw things up.
			textEditor->setText(menu[tempint]->name);
			execEditor->setText(menu[tempint]->value);
			menuItems->selectRow(tempint+1);
			tempParent->saveChanges();
		}
	}
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::textEditorReturnKeyPressed(TextEditor &editor)
{
	TwindyButtonPreferences *tempParent;

	if(lastRow > -1)
	{
		menu[lastRow]->name = textEditor->getText();
		menu[lastRow]->value = execEditor->getText();
		menuItems->updateContent();

		tempParent = static_cast<TwindyButtonPreferences *>(getParentComponent());
		tempParent->saveChanges();
	}
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::paintListBoxItem(int rowNumber,
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

	g.drawText(menu[rowNumber]->name,
			   5,
			   0,
			   (width-10),
			   18,
			   Justification::left, false);
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::selectedRowsChanged(int lastRowselected)
{
	if(menu.size() > 0)
	{
		if((lastRow > -1)&&(lastRow < menu.size()))
		{
			menu[lastRow]->name = textEditor->getText();
			menu[lastRow]->value = execEditor->getText();
		}
		textEditor->setText(menu[lastRowselected]->name);
		execEditor->setText(menu[lastRowselected]->value);
		lastRow = lastRowselected;
	}
	else
		lastRow = -1;
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::paintOverChildren(Graphics &g)
{
	//Menu item list.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(8, 23, 154, 284, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(9, 24, 152, 282, 1);

	//Menu text.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(168, 23, 154, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(169, 24, 152, 22, 1);

	//Menu executable.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(168, 75, 154, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(169, 76, 152, 22, 1);
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::clearMenuItems()
{
	if(menu.size() > 0)
		menu.clear();
	menuItems->updateContent();
	textEditor->setText(T(""));
	execEditor->setText(T(""));
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::addMenuItem(TwindyProperty *newItem)
{
	menu.add(newItem);
	menuItems->updateContent();
	if(menu.size() == 1)
		menuItems->selectRow(0);
	menuItems->updateContent();
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::removeMenuItem(int index)
{
	menu.remove(index);
	if((index == 0)&&(menu.size() > 0))
		menuItems->selectRow(0);
	menuItems->updateContent();
}

//------------------------------------------------------------------------------
void ButtonMenuComponent::setColours(const Colour& backCol,
									 const Colour& textCol,
									 const Colour& buttonCol)
{
	backgroundColour = backCol;
	textColour = textCol;

	menuItemsLabel->setColour(Label::textColourId, textColour);
	textLabel->setColour(Label::textColourId, textColour);
	execLabel->setColour(Label::textColourId, textColour);
	addItem->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	removeItem->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	clearItems->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	itemUp->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	itemDown->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
}
