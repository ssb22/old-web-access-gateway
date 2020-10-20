/*
Access Gateway - a mediator for the World Wide Web

(C) 1998-2006 Silas S. Brown http://ssb22.user.srcf.net

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

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

