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
#include "allinc.h"
#include "images.h"
#include "instring.h"
#include "webget.h"
#include "ampersnd.h"
#include "debug.h"
#include <stdio.h>
#include <ctype.h>
#ifdef CAN_SHOW_IMAGES

#define HORRIBLE_1618_HACK // NEEDATTENTION
// This is if MULTIPLE_STYLES_SUPPORTED and it subtracts 2
// from the height for Korean style - to compensate
// for a quirk in the image server's data that needs fixing
// (only the Japanese is 16x18, everything else is 16x16).
// It's inadequate because the image server might change the
// style of a character if it's not available in the
// preferred style.  2001-07-13

/// Writes an IMG tag for the Unicode character c into InString dest
void addUnicodeCharacter(InString &dest,unsigned c,int doubleIt,const CGIEnvironment* theEnvironment) {
checkPoint();
char buf[9];
// UNICODE_URL is commented out, because we're now putting
// it as a BASE HREF
dest.addString("<IMG SRC=" /*UNICODE_URL*/);
#ifdef DEVELOPMENT_COPY
extern int dontProcessLinks;
if(dontProcessLinks) addUnicodeURL(dest,theEnvironment);
#endif
#ifdef UNICODE_SEPARATOR
sprintf(buf,"%02X",c>>8); dest.addString(buf);
dest.addString(UNICODE_SEPARATOR);
sprintf(buf,"%02X",c&0xFF); dest.addString(buf);
#else
sprintf(buf,"%04X",c); dest.addString(buf);
#endif
dest.addString(UNICODE_EXTENSION
#ifdef UNICODE_EXTRA_HTML
" " UNICODE_EXTRA_HTML
#endif
);
#ifdef UNICODE_HEIGHT
#if (defined(HORRIBLE_1618_HACK) && defined(MULTIPLE_STYLES_SUPPORTED))
int unicodeHeight=UNICODE_HEIGHT;
const char* style=theEnvironment->getStyle();
if(style && *style=='k') unicodeHeight-=2;
#else
#define unicodeHeight (UNICODE_HEIGHT)
#endif // HORRIBLE_1618_HACK etc
sprintf(buf,"%d",doubleIt?(unicodeHeight<<1):unicodeHeight);
dest.addString(" HEIGHT="); dest.addString(buf);
sprintf(buf,"%d",doubleIt?(UNICODE_WIDTH<<1):UNICODE_WIDTH);
dest.addString(" WIDTH="); dest.addString(buf);
#endif // UNICODE_HEIGHT
dest.addString(">");
}

void imageFilter(const char* sourceLine,InString &dest,int doubleIt,const CGIEnvironment* theEnvironment) {
checkPoint();
// sourceLine is just text, not tags etc
for(int i=0; sourceLine[i]; i++) {
if(sourceLine[i]=='&' && sourceLine[i+1]=='#') {
int oldI=i;
unsigned charVal=0; i+=2;
while(isdigit(sourceLine[i])) charVal=charVal*10+(sourceLine[i++]-'0');
if(sourceLine[i]==';') {
addUnicodeCharacter(dest,charVal,doubleIt,theEnvironment);
continue;
} else i=oldI; // and carry on
} dest.addCharacter(sourceLine[i]);
}
}

// Let's have a nasty global...
ShowCharsAs subHtmlShowCharsAs=CharsIMG;
#endif
