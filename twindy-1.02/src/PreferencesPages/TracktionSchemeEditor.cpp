//	TracktionSchemeEditor.cpp - Preferences page to edit .tracktionscheme files.
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
#include "TracktionSchemeEditor.h"
#include "TwindyApp.h"
#include "DrawableTextButton.h"
#include "TwindyProperties.h"
#include "DefaultTracktionScheme.h"
#include "TwindyErrorDisplay.h"
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
TracktionSchemeEditor::TracktionSchemeEditor():
Component()
{
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	//Colours list.
	addAndMakeVisible(coloursLabel = new Label(T("cL"), TRANS("Colours:")));
	coloursLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(colours = new ListBox(T("Colours"), this));

	//Colour selector.
	addAndMakeVisible(colourSelector = new ColourSelector());
	colourSelector->addChangeListener(this);

	//Buttons.
	addAndMakeVisible(newButton = new DrawableTextButton(T("..."), TRANS("new")));
	newButton->addButtonListener(this);
	addAndMakeVisible(openButton = new DrawableTextButton(T("..."), TRANS("open")));
	openButton->addButtonListener(this);
	addAndMakeVisible(saveButton = new DrawableTextButton(T("..."), TRANS("save")));
	saveButton->addButtonListener(this);
	addAndMakeVisible(saveAsButton = new DrawableTextButton(T("..."), TRANS("save as")));
	saveAsButton->addButtonListener(this);

	//Path.
	addAndMakeVisible(pathLabel = new Label(T("cL"), TRANS("Path:")));
	pathLabel->setFont(Font(16.0f, Font::bold));
	addAndMakeVisible(pathEditor = new TextEditor());

	currentColours.mainBackground = root->getColours(0).mainBackground;
	currentColours.tabAreaBackground = root->getColours(0).tabAreaBackground;
	currentColours.menuText = root->getColours(0).menuText;
	currentColours.propertyPanelBackground = root->getColours(0).propertyPanelBackground;
	currentColours.propertyPanelText = root->getColours(0).propertyPanelText;
	currentColours.propertyPanelOutline = root->getColours(0).propertyPanelOutline;
	currentColours.selectedFilterOutline = root->getColours(0).selectedFilterOutline;
	currentColours.yellowButton = root->getColours(0).yellowButton;
	currentColours.blueButton = root->getColours(0).blueButton;
	TwindyProperties *prop = root->getProperties();
	pathEditor->setText(prop->getProperty(T("TracktionScheme")).name);
	colours->updateContent();
	colours->selectRow(0);
}

//------------------------------------------------------------------------------
TracktionSchemeEditor::~TracktionSchemeEditor()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::resized()
{
	newButton->setBounds(10, 3, 50, 25);
	openButton->setBounds(65, 3, 50, 25);
	saveButton->setBounds(120, 3, 50, 25);
	saveAsButton->setBounds(175, 3, 70, 25);
	pathLabel->setBounds(250, 5, 45, 20);
	pathEditor->setBounds(300, 5, 220, 20);

	coloursLabel->setBounds(10, 35, 200, 20);
	colours->setBounds(10, 60, 200, 280);
	colourSelector->setBounds(220, 40, 300, 300);
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::changeListenerCallback(void *objectThatHasChanged)
{
	if(objectThatHasChanged == colourSelector)
	{
		switch(colours->getSelectedRow())
		{
			case 0:
				currentColours.mainBackground = colourSelector->getCurrentColour();
				break;
			case 1:
				currentColours.tabAreaBackground = colourSelector->getCurrentColour();
				break;
			case 2:
				currentColours.menuText = colourSelector->getCurrentColour();
				break;
			case 3:
				currentColours.propertyPanelBackground = colourSelector->getCurrentColour();
				break;
			case 4:
				currentColours.propertyPanelText = colourSelector->getCurrentColour();
				break;
			case 5:
				currentColours.propertyPanelOutline = colourSelector->getCurrentColour();
				break;
			case 6:
				currentColours.selectedFilterOutline = colourSelector->getCurrentColour();
				break;
			case 7:
				currentColours.yellowButton = colourSelector->getCurrentColour();
				break;
			case 8:
				currentColours.blueButton = colourSelector->getCurrentColour();
				break;
		}
		colours->updateContent();
	}
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::buttonClicked(Button *button)
{
	TwindyApp *app = static_cast<TwindyApp *>(JUCEApplication::getInstance());
	TwindyRootWindow *root = app->getRootWindow();

	if(button == newButton)
	{
		currentColours.mainBackground = root->getColours(0).mainBackground;
		currentColours.tabAreaBackground = root->getColours(0).tabAreaBackground;
		currentColours.menuText = root->getColours(0).menuText;
		currentColours.propertyPanelBackground = root->getColours(0).propertyPanelBackground;
		currentColours.propertyPanelText = root->getColours(0).propertyPanelText;
		currentColours.propertyPanelOutline = root->getColours(0).propertyPanelOutline;
		currentColours.selectedFilterOutline = root->getColours(0).selectedFilterOutline;
		currentColours.yellowButton = root->getColours(0).yellowButton;
		currentColours.blueButton = root->getColours(0).blueButton;
		colours->updateContent();
		pathEditor->setText(T(""));
	}
	else if(button == openButton)
	{
		FileChooser phil(TRANS("Open Tracktionscheme..."),
						 File(T("~/.twindy")),
						 T("*.tracktionscheme"),
						 false);

		if(phil.browseForFileToOpen())
		{
			loadColours(phil.getResult());
			pathEditor->setText(phil.getResult().getFullPathName());
		}
	}
	else if(button == saveButton)
	{
		if(pathEditor->getText().isEmpty())
			buttonClicked(saveAsButton);
		else
			saveColours();
	}
	else if(button == saveAsButton)
	{
		FileChooser phil(TRANS("Save Colours As..."),
						 File(T("~/.twindy")),
						 T("*.tracktionscheme"),
						 false);

		if(phil.browseForFileToSave(true))
		{
			String tempstr = phil.getResult().getFullPathName();

			if(!tempstr.endsWith(T(".tracktionscheme")))
				tempstr << T(".tracktionscheme");
			pathEditor->setText(tempstr);
			saveColours();
		}
	}
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::paintListBoxItem(int rowNumber,
										     Graphics &g,
										     int width,
										     int height,
										     bool rowIsSelected)
{
	String tempstr;
	Colour tempCol;

	if(rowIsSelected)
	{
		Colour tempCol = backgroundColour.contrasting();
		if(tempCol.brighter(0.5f) == Colours::white)
			tempCol = backgroundColour;
		g.setColour(tempCol.withAlpha(0.2f));
		g.fillRect(0, 0, width, 25);
	}

	switch(rowNumber)
	{
		case 0:
			tempstr = TRANS("mainBackground");
			tempCol = currentColours.mainBackground;
			break;
		case 1:
			tempstr = TRANS("tabAreaBackground");
			tempCol = currentColours.tabAreaBackground;
			break;
		case 2:
			tempstr = TRANS("buttonText");
			tempCol = currentColours.menuText;
			break;
		case 3:
			tempstr = TRANS("lowerPanelBackground");
			tempCol = currentColours.propertyPanelBackground;
			break;
		case 4:
			tempstr = TRANS("windowTitleText");
			tempCol = currentColours.propertyPanelText;
			break;
		case 5:
			tempstr = TRANS("lowerPanelOutline");
			tempCol = currentColours.propertyPanelOutline;
			break;
		case 6:
			tempstr = TRANS("upperPanelOutline");
			tempCol = currentColours.selectedFilterOutline;
			break;
		case 7:
			tempstr = TRANS("exitButton");
			tempCol = currentColours.yellowButton;
			break;
		case 8:
			tempstr = TRANS("otherButtons");
			tempCol = currentColours.blueButton;
			break;
	}

	g.setColour(tempCol.contrasting());
	g.drawRect(1, 1, 25, (height-2), 1);
	g.setColour(tempCol);
	g.fillRect(2, 2, 23, (height-4));

	g.setColour(Colours::black);
	g.drawText(tempstr,
			   30,
			   2,
			   (width-35),
			   18,
			   Justification::left, true);
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::selectedRowsChanged(int lastRowselected)
{
	switch(lastRowselected)
	{
		case 0:
			colourSelector->setCurrentColour(currentColours.mainBackground);
			break;
		case 1:
			colourSelector->setCurrentColour(currentColours.tabAreaBackground);
			break;
		case 2:
			colourSelector->setCurrentColour(currentColours.menuText);
			break;
		case 3:
			colourSelector->setCurrentColour(currentColours.propertyPanelBackground);
			break;
		case 4:
			colourSelector->setCurrentColour(currentColours.propertyPanelText);
			break;
		case 5:
			colourSelector->setCurrentColour(currentColours.propertyPanelOutline);
			break;
		case 6:
			colourSelector->setCurrentColour(currentColours.selectedFilterOutline);
			break;
		case 7:
			colourSelector->setCurrentColour(currentColours.yellowButton);
			break;
		case 8:
			colourSelector->setCurrentColour(currentColours.blueButton);
			break;
	}
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::paintOverChildren(Graphics &g)
{
	//Colours list.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(8, 58, 204, 284, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(9, 59, 202, 282, 1);

	//Colour selector.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(218, 38, 304, 304, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(219, 39, 302, 302, 1);

	//Path editor.
	g.setColour(backgroundColour.contrasting().withAlpha(0.1f));
	g.drawRect(298, 3, 224, 24, 1);
	g.setColour(backgroundColour.contrasting().withAlpha(0.3f));
	g.drawRect(299, 4, 222, 22, 1);
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::setColours(const Colour& backCol,
									const Colour& textCol,
									const Colour& buttonCol)
{
	backgroundColour = backCol;
	textColour = textCol;

	coloursLabel->setColour(Label::textColourId, textColour);
	newButton->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	openButton->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	saveButton->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	saveAsButton->setBackgroundColours(buttonCol, buttonCol.darker(2.5f));
	pathLabel->setColour(Label::textColourId, textColour);
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::loadColours(const File& file)
{
	XmlDocument doc(file);
	XmlElement *rootNode = doc.getDocumentElement();
	XmlElement *tempNode;
	String tempstr;
	String tempstr2;

	if(rootNode)
	{
		if(rootNode->hasTagName(T("COLOURS")))
		{
			tempNode = rootNode->getFirstChildElement();

			while(tempNode)
			{
				if(tempNode->hasTagName(T("COLOUR")))
				{
					tempstr = tempNode->getStringAttribute(T("name"));
					if(tempstr == T("tabAreaBackground"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.tabAreaBackground = Colour(tempstr2.getHexValue32());
					}
					else if(tempstr == T("mainBackground"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.mainBackground = Colour(tempstr2.getHexValue32());
					}
					else if(tempstr == T("propertyPanelBackground"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.propertyPanelBackground = Colour(tempstr2.getHexValue32());
					}
					else if(tempstr == T("yellowButton"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.yellowButton = Colour(tempstr2.getHexValue32());
					}
					else if(tempstr == T("menuText"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.menuText = Colour(tempstr2.getHexValue32());
					}
					else if(tempstr == T("propertyPanelText"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.propertyPanelText = Colour(tempstr2.getHexValue32());
					}
					else if(tempstr == T("propertyPanelOutline"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.propertyPanelOutline = Colour(tempstr2.getHexValue32());
					}
					else if(tempstr == T("blueButton"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.blueButton = Colour(tempstr2.getHexValue32());
					}
					else if(tempstr == T("selectedFilterOutline"))
					{
						tempstr2 = tempNode->getStringAttribute(T("colour"));
						currentColours.selectedFilterOutline = Colour(tempstr2.getHexValue32());
					}
				}

				tempNode = tempNode->getNextElement();
			}
		}
	}
	else
	{
		currentColours.tabAreaBackground = Colours::black;
		currentColours.mainBackground = Colour(0xff88889f);
		currentColours.propertyPanelBackground = Colour(0xffa0a0bb);
	}
	colours->updateContent();
	selectedRowsChanged(colours->getSelectedRow());
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::saveColours()
{
	File phil(pathEditor->getText());

	//Make sure the file exists before we start trying to re-write parts of it.
	if(!phil.exists())
		writeNewTracktionScheme(pathEditor->getText());

	XmlDocument doc(phil);
	XmlElement *rootNode = doc.getDocumentElement();
	XmlElement *tempNode;
	String tempstr;

	if(rootNode)
	{
		if(rootNode->hasTagName(T("COLOURS")))
		{
			tempNode = rootNode->getFirstChildElement();

			while(tempNode)
			{
				if(tempNode->hasTagName(T("COLOUR")))
				{
					tempstr = tempNode->getStringAttribute(T("name"));
					if(tempstr == T("mainBackground"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.mainBackground.getARGB()));
					}
					else if(tempstr == T("tabAreaBackground"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.tabAreaBackground.getARGB()));
					}
					else if(tempstr == T("menuText"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.menuText.getARGB()));
					}
					else if(tempstr == T("propertyPanelBackground"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.propertyPanelBackground.getARGB()));
					}
					else if(tempstr == T("propertyPanelText"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.propertyPanelText.getARGB()));
					}
					else if(tempstr == T("propertyPanelOutline"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.propertyPanelOutline.getARGB()));
					}
					else if(tempstr == T("selectedFilterOutline"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.selectedFilterOutline.getARGB()));
					}
					else if(tempstr == T("yellowButton"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.yellowButton.getARGB()));
					}
					else if(tempstr == T("blueButton"))
					{
						tempNode->setAttribute(T("colour"),
											   String::toHexString((int)currentColours.blueButton.getARGB()));
					}
				}

				tempNode = tempNode->getNextElement();
			}
			if(!rootNode->writeToFile(File(pathEditor->getText()), T("")))
			{
				TwindyErrorDisplay::getInstance()->addErrorMessage(T("twindrc Error"),
																   T("Could not write changes to twindyrc."));
			}
		}
	}
}

//------------------------------------------------------------------------------
void TracktionSchemeEditor::writeNewTracktionScheme(const String& filepath)
{
	String tempstr;
	File phil(filepath);
	phil.create();

	FileOutputStream philly(phil);
	philly << DefaultTracktionScheme;
}
