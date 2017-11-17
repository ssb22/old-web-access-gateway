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
