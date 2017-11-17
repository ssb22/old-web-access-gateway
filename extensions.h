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


// For documentation about the extensions mechanism, please
// see the file extenlib.h


#ifndef EXTENSIONS_H
#define EXTENSIONS_H

// Be careful about includes - this is included in
// platform.h!
class CGIEnvironment;
class HTMLTag;

// Test with undefined also

#define makeLiteral(identifier) ENV_FUNCTION_PREFIX #identifier
#define mkLit2(identifier) ENV_FUNCTION_PREFIX identifier
// mkLit2 for external, makeLiteral for internal

#define IEE_XHTML(filename) {\
if(!makingCheckBoxes) {\
static char canDo=((theEnvironment->getName(mkLit2(filename),MAY_RETURN_NULL))!=NULL);\
if(canDo) {\
html2xhtml(filenameOfHtml,otherFilename);\
const char* t=filenameOfHtml; filenameOfHtml=otherFilename; otherFilename=t;\
}\
}\
}

#define IEE(filename,description,spaceChar) {\
if(makingCheckBoxes) {\
outputCheckOption(mkLit2(filename),description,theEnvironment); \
} else {\
static char canDo=((theEnvironment->getName(mkLit2(filename),MAY_RETURN_NULL))!=NULL); \
if(canDo) {\
temp.clear(); temp.addString(filename spaceChar);\
temp.addStringWithCommandPrecautions(filenameOfHtml);\
temp.addCharacter('>'); temp.addStringWithCommandPrecautions(otherFilename);\
system(temp.getString());\
const char* t=filenameOfHtml; filenameOfHtml=otherFilename; otherFilename=t; \
}\
}\
}

#define Include_External_Extension(filename,description) IEE(filename,description," ")

#define Include_Filter_Extension(filename,description) IEE(filename,description," < ")

#define Include_XHTML_External_Extension(filename,description) IEE_XHTML(filename) IEE(filename,description," ")

#define Include_XHTML_Filter_Extension(filename,description) IEE_XHTML(filename) IEE(filename,description," < ")

#define Include_Script_Extension(uid,html,description) {\
if(makingCheckBoxes) outputCheckOption(mkLit2(uid),description,c);\
else if(c->getName(mkLit2(uid),MAY_RETURN_NULL)) c->h_puts(html);\
}

#define Include_Internal_Extension1(fclass) \
ext_##fclass theExt_##fclass;

#define Include_Internal_Extension2(fclass,description) { \
if(makingCheckBoxes) {\
outputCheckOption(makeLiteral(fclass),description,theEnvironment); \
} else {\
static char canDo=((theEnvironment->getName(makeLiteral(fclass),MAY_RETURN_NULL))!=NULL); \
if(canDo) { \
if(t) theExtensions->theExt_##fclass.tag(t); else theExtensions->theExt_##fclass.startingSecondPass(); \
}\
}\
}

// NB External_Extensions might not be defined yet
// but this should work anyway (it's never called if they're
// not defined)
// Feature: If filenameOfHtml is NULL, check boxes are printed
const char* callFilters(const char* filenameOfHtml,const char* otherFilename,CGIEnvironment* theEnvironment);

// Internal stuff (non-comment tag t; NULL between passes)
void callExtensions(HTMLTag* t,CGIEnvironment* theEnvironment,int makingCheckBoxes=0);

class Extension {
public:
Extension() {};
virtual ~Extension() {};
virtual void tag(HTMLTag* t)=0;
virtual void startingSecondPass()=0;
};

class ExtensionContainer; // Don't define it here!
// (because need to put user extensions in it)

ExtensionContainer* makeNewReturnOldExtensions();
void resetOldExtensions(ExtensionContainer* e);

void appendScriptExtensionsTo(CGIEnvironment* c,int makingCheckBoxes=0);

#endif

