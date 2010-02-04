/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _FWWX_CONFIG_HPP_
#define _FWWX_CONFIG_HPP_

#ifdef _WIN32

	#ifdef FWWX_EXPORTS
	#define FWWX_API __declspec(dllexport)
	#else
	#define FWWX_API __declspec(dllimport)
	#endif

	#define FWWX_CLASS_API

	#pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

	#ifdef FWWX_EXPORTS
	#define FWWX_API __attribute__ ((visibility("default")))
	#define FWWX_CLASS_API __attribute__ ((visibility("default")))
	#else
	//#define FWWX_API __attribute__ ((visibility("hidden")))
	//#define FWWX_CLASS_API __attribute__ ((visibility("hidden")))
	#define FWWX_API 
	#define FWWX_CLASS_API 
	#endif

#else

	#define FWWX_API
	#define FWWX_CLASS_API

#endif

#endif //_FWWX_CONFIG_HPP_
