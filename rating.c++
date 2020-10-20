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
#include "allinc.h"
#include "rating.h"
#include "cgilib.h"
#include "makeform.h"
#include "action.h"
#include "lastupd.h"
#include "strfunc.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define PICS_START "(PICS-1.1"

const char* getAfterNextWhitespace(const char* s) {
while(*s && !isspace(*s)) s++;
while(*s && isspace(*s)) s++;
return s;
}

void checkRating(const char* val) {

if(!strnicmp(val,PICS_START,strlen(PICS_START)))
val+=strlen(PICS_START);
else return;

enum State {
FirstCharOfWord,
InWord,
InQuotedString,
StartOfRating,
InRating,
StartOfRatingValue,
InRatingValue,
InRatingMultiValue
} theState=FirstCharOfWord;

for(int i=0; val[i]; i++) {
switch(theState) {
case FirstCharOfWord:
if(!isspace(val[i])) {
if(val[i]=='\"') theState=InQuotedString;
else if(val[i]=='(') theState=StartOfRating;
else theState=InWord;
}
break;
case InWord:
if(isspace(val[i])) theState=FirstCharOfWord;
break;
case InQuotedString:
if(val[i]=='\"') theState=FirstCharOfWord;
break;
case StartOfRating:
if(isspace(val[i])) break;
// NEEDATTENTION Should really check the unique URLs
// Check "SafeSurf" rating
if(!strncmp(val+i,"SS~~00",6)) {
if(val[i+6]!='0') throw RatingWarning(); // rating on something other than age group
const char* level=getAfterNextWhitespace(val+i);
if(*level && atoi(level)!=1) throw RatingWarning(); // not suitable for all age groups
// Note: atoi() may not parse floating point or multi-valued ratings, but the failure should still throw the exception, which is what we want
}
// Check icra/rsac and "Safe for Kids" ratings
// (it would be nice if the former had more documentation in an accessible format - 2001-03-11)
else if(strchr("nsvlNSVL",val[i])
&& isspace(val[i+1])) {
const char* level=getAfterNextWhitespace(val+i);
if(*level && atoi(level)!=0) throw RatingWarning();
} else if(strchr("clnovCLNOV",val[i])
&& strchr("zZ",val[i+1])
&& isspace(val[i+2])) {
const char* level=getAfterNextWhitespace(val+i);
if(*level && atoi(level)!=1) throw RatingWarning();
}
// Done checking
theState=InRating;
break;
case InRating:
if(isspace(val[i])) theState=StartOfRatingValue;
else if(val[i]=='(') theState=InRatingMultiValue;
break;
case StartOfRatingValue:
if(!isspace(val[i])) theState=InRatingValue;
break;
case InRatingValue:
if(isspace(val[i])) theState=StartOfRating;
break;
case InRatingMultiValue:
if(val[i]==')') theState=StartOfRating;
}
}
}

void handleRatingWarningException(CGIEnvironment* theEnvironment) {
theEnvironment->initOutputData();
outputBodyTag(theEnvironment);
if(outputOptionsLink(theEnvironment)) outputStartPageLine(theEnvironment);
theEnvironment->outHTMLParagraph(L_RATING_WARNING);
theEnvironment->h_fputs(MakeRelativeActionURL("<A HREF=\"","?"));
theEnvironment->urlDumpCurrentOptions(ENV_PREFIX);
theEnvironment->h_fputs("\">");
theEnvironment->h_fputs(L_RATING_ABORT);
theEnvironment->h_fputs(MakeRelativeActionURL("</A> | <A HREF=\"","?"));
if(theEnvironment->urlDumpCurrentOptions("")) theEnvironment->h_putchar('&');
theEnvironment->h_fputs(CONTINUE_ON_RATING "=1\">");
theEnvironment->h_fputs(L_RATING_CONTINUE);
theEnvironment->h_puts("</A><HR>");
outputLastUpdate(theEnvironment);
}
