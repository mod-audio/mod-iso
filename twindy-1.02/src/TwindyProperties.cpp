//	TwindyProperties.cpp - Class to handle the various properties (buttons,
//						   menus, tracktionscheme file etc.) for Twindy.
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
#include "TwindyProperties.h"
#include "DefaultTracktionScheme.h"
#include "TwindyErrorDisplay.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
TwindyProperties::TwindyProperties():
doc(0)
{
	File test(T("~/.twindy/twindyrc"));
	if(!test.exists())
	{
		std::cout << "~/.twindy/twindyrc does not exist" << std::endl;
		writeConfigFile();
		writeTracktionScheme();
	}

	doc = new XmlDocument(File(T("~/.twindy/twindyrc")));
}

//------------------------------------------------------------------------------
TwindyProperties::~TwindyProperties()
{
	delete doc;
}

//------------------------------------------------------------------------------
TwindyProperty TwindyProperties::getProperty(const String& name)
{
	TwindyProperty retval;
	XmlElement *tempElement;
	XmlElement *mainElement = doc->getDocumentElement();

	if(mainElement)
	{
		if(mainElement->hasTagName(T("TwindyProperties")))
		{
			tempElement = mainElement->getChildByName(name);
			if(tempElement)
			{
				retval.name = tempElement->getStringAttribute(T("name"));
				retval.value = tempElement->getStringAttribute(T("value"));
			}
			else
			{
				retval.name = T("nil");
				retval.value = T("nil");
			}
		}
		delete mainElement;
	}

	return retval;
}

//------------------------------------------------------------------------------
long TwindyProperties::getNumSubProperties(const String& name)
{
	long retval = 0;
	XmlElement *tempElement;
	XmlElement *mainElement = doc->getDocumentElement();

	if(mainElement)
	{
		if(mainElement->hasTagName(T("TwindyProperties")))
		{
			tempElement = mainElement->getChildByName(name);
			if(tempElement)
				retval = tempElement->getNumChildElements();
		}
		delete mainElement;
	}

	return retval;
}

//------------------------------------------------------------------------------
TwindyProperty TwindyProperties::getSubProperty(const String& name, long index)
{
	TwindyProperty retval;
	XmlElement *tempElement;
	XmlElement *tempElement2;
	XmlElement *mainElement = doc->getDocumentElement();

	if(mainElement)
	{
		if(mainElement->hasTagName(T("TwindyProperties")))
		{
			tempElement = mainElement->getChildByName(name);
			if(tempElement)
			{
				tempElement2 = tempElement->getChildElement(index);
				if(tempElement2)
				{
					retval.name = tempElement2->getStringAttribute(T("name"));
					retval.value = tempElement2->getStringAttribute(T("value"));
				}
				else
				{
					retval.name = T("nil");
					retval.value = T("nil");
				}
			}
		}
		delete mainElement;
	}

	return retval;
}

//------------------------------------------------------------------------------
void TwindyProperties::setProperty(const String& name,
								   const TwindyProperty& value)
{
	XmlElement *tempElement;
	XmlElement *mainElement = doc->getDocumentElement();

	if(mainElement)
	{
		if(mainElement->hasTagName(T("TwindyProperties")))
		{
			tempElement = mainElement->getChildByName(name);
			if(!tempElement) //If it doesn't exist already.
			{
				tempElement = new XmlElement(name);
				mainElement->addChildElement(tempElement);
			}
			tempElement->setAttribute(T("name"), value.name);
			tempElement->setAttribute(T("value"), value.value);
		}
		if(!mainElement->writeToFile(File(T("~/.twindy/twindyrc")), T("")))
		{
			TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("twindyrc Error"),
															   TRANS("Could not write changes to twindyrc."));
			delete mainElement;
		}
		else
		{
			delete mainElement;
			delete doc;
			doc = new XmlDocument(File(T("~/.twindy/twindyrc")));
		}
	}
}

//------------------------------------------------------------------------------
void TwindyProperties::setSubProperty(const String& property,
									  const TwindyProperty& subproperty)
{
	XmlElement *tempElement;
	XmlElement *tempElement2;
	XmlElement *mainElement = doc->getDocumentElement();

	if(mainElement)
	{
		if(mainElement->hasTagName(T("TwindyProperties")))
		{
			tempElement = mainElement->getChildByName(property);
			if(tempElement)
			{
				tempElement2 = findElementWithName(tempElement, subproperty.name);
				tempElement2->setAttribute(T("value"), subproperty.value);
			}
		}
		if(!mainElement->writeToFile(File(T("~/.twindy/twindyrc")), T("")))
		{
			TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("twindyrc Error"),
															   TRANS("Could not write changes to twindyrc."));
			delete mainElement;
		}
		else
		{
			delete mainElement;
			delete doc;
			doc = new XmlDocument(File(T("~/.twindy/twindyrc")));
		}
	}
}

//------------------------------------------------------------------------------
void TwindyProperties::clearSubProperties(const String& property)
{
	XmlElement *tempElement;
	XmlElement *mainElement = doc->getDocumentElement();

	if(mainElement)
	{
		if(mainElement->hasTagName(T("TwindyProperties")))
		{
			tempElement = mainElement->getChildByName(property);
			if(tempElement)
			{
				if(tempElement->getNumChildElements() > 0)
					tempElement->deleteAllChildElements();
			}
		}
		if(!mainElement->writeToFile(File(T("~/.twindy/twindyrc")), T("")))
		{
			TwindyErrorDisplay::getInstance()->addErrorMessage(TRANS("twindyrc Error"),
															   TRANS("Could not write changes to twindyrc."));
			delete mainElement;
		}
		else
		{
			delete mainElement;
			delete doc;
			doc = new XmlDocument(File(T("~/.twindy/twindyrc")));
		}
	}
}

//------------------------------------------------------------------------------
void TwindyProperties::writeConfigFile()
{
	XmlElement mainElement(T("TwindyProperties"));
	XmlElement *tempElement;
	XmlElement *tempElement2;
	File phil(T("~/.twindy/twindyrc"));
	phil.create();

	tempElement = new XmlElement(T("TracktionScheme"));
	tempElement->setAttribute(T("name"), T("~/.twindy/Default.tracktionscheme"));
	tempElement->setAttribute(T("value"), T("~/.twindy/Default.tracktionscheme"));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("StartupPrograms"));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("LeftButton1"));
	tempElement->setAttribute(T("name"), T("Firefox"));
	tempElement->setAttribute(T("value"), T("firefox"));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("LeftButton2"));
	tempElement->setAttribute(T("name"), T("Gedit"));
	tempElement->setAttribute(T("value"), T("gedit"));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("LeftButton3"));
	tempElement->setAttribute(T("name"), T("Run App..."));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("LeftButton4"));
	tempElement->setAttribute(T("name"), T("Left 4"));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("LeftButton5"));
	tempElement->setAttribute(T("name"), T("Left 5"));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("LeftButton6"));
	tempElement->setAttribute(T("name"), T("Left 6"));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("LeftButton7"));
	tempElement->setAttribute(T("name"), T("Left 7"));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("RightButton1"));
	tempElement->setAttribute(T("name"), T("XTerm"));
	tempElement->setAttribute(T("value"), T("xterm -bg \"#A0A0BB\""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("RightButton2"));
	tempElement->setAttribute(T("name"), T("Gnome Terminal"));
	tempElement->setAttribute(T("value"), T("gnome-terminal"));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("RightButton3"));
	tempElement->setAttribute(T("name"), T("Nautilus"));
	tempElement->setAttribute(T("value"), T("nautilus --browser --no-desktop"));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("RightButton4"));
	tempElement->setAttribute(T("name"), T("Run App..."));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("RightButton5"));
	tempElement->setAttribute(T("name"), T("Right 5"));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("RightButton6"));
	tempElement->setAttribute(T("name"), T("Right 6"));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("RightButton7"));
	tempElement->setAttribute(T("name"), T("Right 7"));
	tempElement->setAttribute(T("value"), T(""));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("GlobalFont"));
	tempElement->setAttribute(T("name"), T("Bitstream Vera Sans"));
	tempElement->setAttribute(T("value"), T("Bitstream Vera Sans"));
	mainElement.addChildElement(tempElement);

	tempElement = new XmlElement(T("Workspace"));
	tempElement->setAttribute(T("name"), T("."));
	tempElement->setAttribute(T("value"), T("."));
	tempElement2 = new XmlElement(T("SubProperty"));
	tempElement2->setAttribute(T("name"), T("Workspace 1"));
	tempElement2->setAttribute(T("value"), T("~/.twindy/Default.tracktionscheme"));
	tempElement->addChildElement(tempElement2);
	mainElement.addChildElement(tempElement);

	mainElement.writeToFile(phil, T(""));
}

//------------------------------------------------------------------------------
void TwindyProperties::writeTracktionScheme()
{
	String tempstr;
	File phil(T("~/.twindy/Default.tracktionscheme"));
	phil.create();

	FileOutputStream philly(phil);
	philly << DefaultTracktionScheme;
}

//------------------------------------------------------------------------------
XmlElement *TwindyProperties::findElementWithName(XmlElement *element,
												  const String& name)
{
	bool foundElement = false;
	XmlElement *returnElement;

	for(returnElement = element->getFirstChildElement();
		returnElement;
		returnElement = returnElement->getNextElement())
	{
		if(returnElement->getStringAttribute(T("name")) == name)
		{
			foundElement = true;
			break;
		}
	}

	if(!foundElement)
	{
		returnElement = new XmlElement(T("SubProperty"));
		returnElement->setAttribute(T("name"), name);
		element->addChildElement(returnElement);
	}

	return returnElement;
}
