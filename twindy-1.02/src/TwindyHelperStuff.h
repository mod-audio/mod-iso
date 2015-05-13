//	TwindyHelperStuff.h - Some helper routines.
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

#ifndef TWINDYHELPERSTUFF_H_
#define TWINDYHELPERSTUFF_H_

#include <iostream>
#include <string>

#ifdef TWINDY_DEBUG
///	Outputs a debug message, along with the file and line number it came from.
#define TWINDY_DBG_MESSAGE(str) std::cout << __FILE__ << ":" << __LINE__ \
							 	<< " " << str << std::endl
#else
#define TWINDY_DBG_MESSAGE(string)
#endif

#endif
