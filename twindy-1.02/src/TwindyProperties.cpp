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
TwindyProperties::TwindyProperties()
{
}

//------------------------------------------------------------------------------
TwindyProperties::~TwindyProperties()
{
}

//------------------------------------------------------------------------------
TwindyProperty TwindyProperties::getProperty(const String&)
{
    TwindyProperty retval;
    retval.name  = T("nil");
    retval.value = T("nil");
    return retval;
}

//------------------------------------------------------------------------------
long TwindyProperties::getNumSubProperties(const String&)
{
	return 0;
}

//------------------------------------------------------------------------------
TwindyProperty TwindyProperties::getSubProperty(const String&, long)
{
    TwindyProperty retval;
    retval.name  = T("nil");
    retval.value = T("nil");
    return retval;
}

//------------------------------------------------------------------------------
void TwindyProperties::setProperty(const String&, const TwindyProperty&)
{
}

//------------------------------------------------------------------------------
void TwindyProperties::setSubProperty(const String&, const TwindyProperty&)
{
}

//------------------------------------------------------------------------------
void TwindyProperties::clearSubProperties(const String&)
{
}
