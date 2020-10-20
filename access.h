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
// -*- C++ -*-
#ifndef ACCESS_H
#define ACCESS_H
#include "platform.h"
#include "cgilib.h"
#include "tabtrim.h"
#include "linkfol.h"
#include "tagstuff.h"
#include "banner.h"
#include <stdio.h>

/**
A structure to hold the user's preferences and the current state.  If the
filter needs to re-enter itself then it will use another.
*/
struct AccessOptions {
// NB This struct gets memset to 0 to start with
// so don't put anything here that has a civilised constructor!

// Much of this is a bit of optimisation
// (NB processTag is going to be called a *lot*)

CGIEnvironment* theEnvironment;
FILE* in; const char* currentPage; char needToDeleteCurpage;
const char* boldColour, *normalColour, *headColour;
int frameNesting;
int lineInputPtr;

char ignoreBody,lastLinkWasHref,inHref,inPre,hadAreaInMapYet,
needLinkImgNoAlt,*urlForLinkImgNoAlt;

char alreadyHadLinkBrackets, inSelect;

BannerVariables* banner; // in a separate struct because
// it needs to be the same thing across all frames
int charsSinceLastSpace;

char disableScript; char scriptGuess;
char disableStyle;
char ignoreBodyTag;
char ignoreFontTags;
char processFrames; char stripFrames;
char processTables; char stripTables;
char noApplets;
char changeBlinkToBold;
char changeULToBold;
char changeItalToBold;
char separateAdjacentLinks;
char removeMarquee;
char smileySearch;
char processHrefImg;
char replaceEmbedWithBGSound;
char processEmbedTags;
char processImageMaps;
char removeDocumentHead;
char resetSaysReset;
char resetFontAfterLink;
char headingPromotionLevel;
char repeatAltTagsAfterImages;
char dontStripHrefForAlt;
char replaceParasWithIndent;
char processParams;
char spaceDetect;
char addHardWrap;
char removeAltTags;
char removeAllImages;
char removeNobr;
char zapImageSubmits;
char zapHeightsWithPercent;
char removeSpacer;
char expandAcronyms;
char showHiddenFormFields;
char adobePdfHack;
char doFlash;
TableTrim* tableTrimmer;
LinkFollow* linkFollower;
char doLinkStatusLine;

char canWriteInHead,inHead;
char inHeading; // i.e. in <H1> etc, different from <HEAD>

char win1252;

const char* wapLastTitle;

int processTag(HTMLTag& theTag,InString&htmlLineInput);
int processTag(const char* theTag,InString&htmlLineInput) { HTMLTag x(theTag); return(processTag(x,htmlLineInput)); }
int doScriptyStuff(const HTMLTag &givenTag,const char* matchString,int disable,FILE* in,InString &htmlLineInput,int guessURLs);
int doScriptSrcStuff(const HTMLTag &theTag,int disableScript,FILE* in,InString &htmlLineInput,int guessURLs,const char* currentPage,CGIEnvironment* theEnvironment);
int doSwfStuff(const char* src,InString &htmlLineInput,const char* currentPage,CGIEnvironment* theEnvironment);
void doScriptyStuff(const char* string,InString &htmlLineInput);
void outputGuessedLink(const char* link,InString &htmlLineInput,bool justHadOne);
void separateTableRow(CGIEnvironment* theEnvironment,const char* comment);
void initFromEnvironment();
char doBannerMove,addEndOfPage;
};

#endif
