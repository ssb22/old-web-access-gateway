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
#ifndef OPTIONS_H
#define OPTIONS_H
/// Can comment out the definition of OLD_PREFIX if don't want to support it
/// (it's still here so that bookmarks etc can work)
#define OLD_PREFIX "ssb22A"
#define OLD_PREFIX_LEN 6
#define INCREMENT_OLD_TO_NEW 5 // *** Note the dependency! ***
/// SSB22_PREFIX is shorter, because we don't have to worry about uniqueness anymore
#define SSB22_PREFIX "A"
/// SSB22_ESCAPE: Used when someone else is using SSB22_PREFIX in forms
#define SSB22_ESCAPE SSB22_PREFIX "1"
/// Save some strlen calls
#define SSB22_ESCAPE_LEN 2

/// Matches no setting
#define NO_CONVERS_VAL "!"

#define CGI_COMPRESSION       SSB22_PREFIX "c"
#define ENV_PREFIX            SSB22_PREFIX "e"
#define HELP_ENV              SSB22_PREFIX "h"
#define ISINDEX_ENV           SSB22_PREFIX "i"
#define FORM_NEEDS_POSTING    SSB22_PREFIX "p"
#define CONTINUE_ON_RATING    SSB22_PREFIX "r"
#define ENV_SHOW_OPTIONS      SSB22_PREFIX "s"
#define ENV_OLD_SHOW_OPTIONS  SSB22_PREFIX "S"
// (ENV_OLD_SHOW_OPTIONS is used by help.c++)
#define ENV_SHOW_ENCODING_OPTIONS SSB22_PREFIX "t"
#define USER_TEXT_ENV         SSB22_PREFIX "T"
#define URL_ENV               SSB22_PREFIX "u" // can be used as a prefix by the WAP stuff

#define PREPROCESS_ENV SSB22_PREFIX "P" // NB this value is now documented!
#define preprocessOnly(e) (e->getName(PREPROCESS_ENV,MAY_RETURN_NULL))

// *** Important note ***    if adding text boxes!

// If adding more text boxes, don't forget to update isCheckBox()
// in makeform.c++ !  (Sorry about this; I had to put the patch in
// because of KDE's browser)


// URL_ENV different (not dumped on transfer to new one)
// But we do want user and password to be carried across

// All the ENV_PREFIX ones must be at least two letters
// to avoid conflicts with the access options
#define USER_ENV ENV_PREFIX "Ur"
#define PWD_ENV  ENV_PREFIX "Pd"
#define ENV_LANG_SHOW_CHARS_AS ENV_PREFIX "la"
#define ENV_ENCODING_DESCRIPTION ENV_PREFIX "ld"
#define ENV_OMIT_SUB_ENCODING ENV_PREFIX "lh"
#define ENV_LANG_DONT_SHOW_IMAGES ENV_PREFIX "li" // legacy
#define ENV_LANG_DOUBLE_IMAGE_SIZE ENV_PREFIX "l2" // legacy
#define ENV_NONLOCAL_PASSWORD ENV_PREFIX "lp"
#define ENV_FIRST_TIME ENV_PREFIX "lq" // not preserved
#define ENV_OMIT_SERVER_ENCODING ENV_PREFIX "ls"
#define ENV_LANGUAGE_TABLE ENV_PREFIX "lt"
#define ENV_FUNCTION_PREFIX ENV_PREFIX "fn" // For extensions
#define ENV_COOKIES ENV_PREFIX "ck"
#define ENV_AVOID_SWITCH ENV_PREFIX "as"

#define ENV_UNICODE_URL ENV_PREFIX "uu"
#define ENV_PREFERRED_STYLE ENV_PREFIX "us"

#define ENV_ALINK_COLOR ENV_PREFIX "ca"
#define ENV_BGCOLOR     ENV_PREFIX "cb"
#define ENV_BOLD_COLOUR ENV_PREFIX "cB"
#define ENV_HEAD_COLOUR ENV_PREFIX "ch"
#define ENV_LINK_COLOR  ENV_PREFIX "cl"
#define ENV_HOVER_BG    ENV_PREFIX "cL"
#define ENV_TEXT_COLOR  ENV_PREFIX "ct"
#define ENV_VLINK_COLOR ENV_PREFIX "cv"

#define ENV_FONTSIZE    ENV_PREFIX "fs"
#define ENV_NOT_VI      ENV_PREFIX "si"
#define DONT_SHOW_OPTIONS_LINK ENV_PREFIX "so"
#define DONT_SHOW_CHARSET ENV_PREFIX "sc"
#define SHOW_URL_BOX ENV_PREFIX "su"
#define LONG_URL_BOX ENV_PREFIX "sU"
#define TABLE_TRIM SSB22_PREFIX "tt"
#define LINK_FOLLOW SSB22_PREFIX "lf"
#define UNSPEC_COLOUR "x"

#endif
