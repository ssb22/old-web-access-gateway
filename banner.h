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
#ifndef BANNER_H
#define BANNER_H
#include "platform.h"
#include <ctype.h>
#include <string.h>
#ifdef MEMCPY_IS_IN_MEM_DOT_H
#include <mem.h>
#endif

enum HeuristicThresholds {
// See below for explanations of banner etc
//bannerDetectionThreshold=20,
bannerDetectionThreshold=30, // meaningful characters between link tags
bannerMinLength=200, // shorter than this is left alone (History: 100 200)
bannerMaxLength=4000, // longer than this is left alone (it's probably an index page or something)
// Reduce bannerMaxLength if too many index pages get split
// Increase bannerMaxLength if too many large banners fail to move
// History: 10000 4000 
// Really, make that user-definable (and in options)
bannerDocMinLength=500, // documents shorter than this are left alone (History: 300 500)
horizontalLineThreshold=30, // meaningful characters in a table row before a horizontal line is drawn after that row (as opposed to simply starting a new line)
charCountTableRowPenalty=10, // added to meaningfulCtr2 (if it's still less than bannerMinLength) at the end of a table row (if tables being processed); helps with banners that take up screenfuls because of the table processing (History: 5 10)
BigWordLen=20, // max average word length in applet parameter for it to get recognised as text
WrapThreshold1=30, // If a word is longer than this then a space will be added after the next non-alphanumeric character (if options.removeNobr)
WrapThreshold2=50 // If a word is longer than this then a space will be added anyway (if options.removeNobr)
};

struct BannerVariables {
// Banner detection variables.
// 2000-07-08: The idea of waiting for a threshold of
// 20 or 30 meaningful characters between links before
// concluding that the banner had ended worked on most
// web pages but tends to split an index in half!  The
// banner detect code isn't activated on a "front" page
// but that doesn't completely solve the problem.

// Idea: Make bannerDetectionThreshold proportional to the
// size of the banner (larger banners require larger
// numbers of meaningful characters after them).  Should
// fix the index problem.
// Problem: If there is a link early in the main text,
// the banner boundary might be set too late.

// Let's say: If it's an index page (the bannerEnd is
// greater than some large threshold), then forget banner
// detection.  This will probably fail with those pages
// that have hugely complicated banners in a very small
// space, and could still lead to some false positives
// on index pages, but I'm going to try it and see what
// happens.
// I've renamed bannerMaxLength to bannerMinLength (should
// have been that in the first place) and introduced
// bannerMaxLength as well.

// 2000-07-20: What happened was a small index page got
// split.  The "banner too large" threshold ought to
// shrink (or even disappear) for small pages.  For now,
// we'll have a threshold on the minimum total size of
// the page for banner movement to take place.
// This is bannerDocMinLength (in meaningful characters)
// Also increasing bannerMinLength

int meaningfulCharacterCounter,meaningfulCtr2,
hadMeaningfulThisRow,hadMeaningfulThisCell;
// meaningfulCtr2 accumulates; meaningfulCharacterCounter
// is reset at the end of each link.  meaningfulCtr2
// is the total number of meaningful chars IN the banner.
// meaningfulCharacterCounter becomes the total number of
// meaningful characters SINCE the banner.
// hadMeaningfulThisRow is for table processing - it
// counts the meaningful characters in each table row; if
// this exceeds a threshold then <HR> is used to separate
// rows (otherwise just <BR> or nothing)
char bannerEndLocked; // once bannerDetectionThreshold
// is exceeded at the start of a link, this is set to 1
// so bannerEnd cannot be moved any further
char hadHeadingYet;
long bannerEnd, lockedBannerEnd;
BannerVariables() { memset(this,0,sizeof(BannerVariables)); }
};

/// Returns non-0 if the parameter is "meaningful" (for example, if it indicates a proper ALT tag)
inline int isMeaningful(char ch) {
return(isalnum(ch) || (ch&0x80) || ch=='&');
}

/// Returns non-0 if any of the characters in the string are "meaningful"
int isMeaningful(const char* ch);

#endif
