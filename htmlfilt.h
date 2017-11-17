/*
Access Gateway - a mediator for the World Wide Web

(C) 1998-2006 Silas S. Brown (St John's College,
Cambridge, UK, http://www.cus.cam.ac.uk/~ssb22 )

This is not intended to be model or example code,
but it is intended to be secure enough to run on a
Web server without compromising it.

If you would like to email me a patch, please first
ensure that you have the latest release, and use
"diff -Naur old-directory new-directory", where
"old-directory" contains the unmodified release
version and "new-directory" contains the
modifications.  Please also include a brief
description of what the patch achieves, your name
(for an acknowledgement), and any copyright
restrictions (ideally the patch should be public
domain).  I may not apply every patch but I do try to
answer my email, unless I am inundated, busy, or
offline for a long time.

This program is free software; you can redistribute it
and/or modify it under the terms of the GNU General
Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General
Public License along with this program; see the file
COPYING.  If not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

*/

#ifndef ALLINC_H
#define ALLINC_H
#include "allinc.h"
#endif
// This "allinc.h" fragment is also in "notice.txt" (copied
// to top of every file on release), just in case forget to
// put it in some file
// Also, need to put this HERE (not in include file) for
// Monash:
#ifdef __GNUC__
#if (__GNUC__ > 2)
using namespace std;
#endif
#else
// Unknown compiler - assume it's standard
using namespace std;
#endif

// end of notice.txt
#ifndef HTMLFILT_H
#define HTMLFILT_H
#include "platform.h"
#include "cgilib.h"
#include "banner.h"
#include <stdio.h>
/**
Decodes the configuration options, and filters an HTML page.  May call itself
if it needs to retrieve a frame.
@param theEnvironment The CGI environment to output to and read configuration from
@param in The input file of the original (or pre-filtered) HTML page
@param currentPage The absolute URL of the original HTML page, used for relative URLs
@return The value of hadMeaningfulThisRow (if it's a frameset then the caller can decide if it's a good idea to draw a line)
*/
void htmlFilter(CGIEnvironment* theEnvironment,FILE* in,const char* currentPage,int isPlainText=0,int frameNesting=0,BannerVariables* banner=NULL);

/**
Sets the upper limit on the frame nest depth.  If there were no limit,
someone could cause a stack overflow by pointing a FRAME SRC to the main page. */
#define MAX_FRAME_NESTING 10

#endif
