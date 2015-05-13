//	TwindyProperties.h - Class to handle the various properties (buttons, menus,
//						 tracktionscheme file etc.) for Twindy.
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

#ifndef TWINDYPROPERTIES_H_
#define TWINDYPROPERTIES_H_

///	Simple struct representing a property.
typedef struct
{
	String name;
	String value;
} TwindyProperty;

///	Class to handle the various properties for Twindy.
/*!
	Properties include things like menu entries, buttons to start apps, and
	the tracktionscheme file to set the colours used.
 */
class TwindyProperties
{
  public:
	///	Constructor.
	/*!
		This looks for ~/.twindy/twindyrc.  If it doesn't exist, it creates the
		.twindy folder and adds a default twindyrc and Default.tracktionscheme
		in it.
	 */
	TwindyProperties();
	///	Destructor.
	/*!
		Writes the properties back to the twindyrc file (it doesn't do it yet,
		but eventually Twindy will have a bunch of dialogs to let the user set
		all the properties from the app, rather than having to edit the config
		file by hand).
	 */
	~TwindyProperties();

	///	Returns the named property.
	/*!
		The name and value members of the returned TwindyProperty will be set
		to "nil" if this fails.
	 */
	TwindyProperty getProperty(const String& name);
	///	Returns the number of sub-properties a (name'd) property contains.
	/*!
		This is used by the buttons, because each button could contain a menu,
		which is made up of a number sub-properties, with a name and an exec
		command.
	 */
	long getNumSubProperties(const String& name);
	///	Returns the indexed sub-property of the named property.
	/*!
		The name and value members of the returned TwindyProperty will be set
		to "nil" if this fails.
	 */
	TwindyProperty getSubProperty(const String& name, long index);

	///	Sets the named property (creates it if it doesn't exist).
	void setProperty(const String& name, const TwindyProperty& value);
	///	Sets a sub-property of the named property.
	/*!
		If a sub-property with the same subproperty.name exists, this just
		makes it's value member equal to subproperty.value.
	 */
	void setSubProperty(const String& property,
						const TwindyProperty& subproperty);
	///	Removes all the subproperties of the named property.
	void clearSubProperties(const String& property);
  private:
	///	Creates the ~/.twindy folder and twindyrc file, if they don't exist.
	void writeConfigFile();
	///	Writes a Default.tracktionscheme file to ~/.twindy, if it doesn't exist.
	void writeTracktionScheme();

	///	Searches for a child of element with a name attribute == name, and returns it.
	/*!
		Returns a newly-created XmlElement if it doesn't already exist.
	 */
	XmlElement *findElementWithName(XmlElement *element, const String& name);

	///	The twindyrc file.
	/*!
		This is kept open as long as the object exists.
	 */
	XmlDocument *doc;
};

#endif
