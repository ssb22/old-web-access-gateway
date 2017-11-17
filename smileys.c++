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
/// All smileys must start with a character in this string.  This means the program needn't bother to start a substitution search on other characters.
const char* smileyFirstChars="&*:;.,+0BEX8>@{}=-`(%[|";
// const char smileyFirstChars[] doesn't seem to link properly

// NB THe smileys in this file were taken from some random
// web page for which I have lost the URL.  I don't know
// what all the American cultural references are; I hope
// it's all OK.  - Silas

/// The decoding of all smileys
const char* smileys[]={
// Must be sorted in descending order of size
// Also make sure that the first character of each
// is present in smileyFirstChars
"*&#60;:-)","(grin in Santa Claus hat)",
"&#60;|-)", "(Chinese grin)",
"&#60;|-(", "(Chinese sad face)",
"&#60;:-I", "(dunce's face)",
"*&lt;:-)", "(grin in Santa Claus hat)",
":&#60;)",  "(grin from an Ivy League School)",
"&lt;|-)",  "(Chinese grin)",
"&lt;|-(",  "(Chinese sad face)",
"&lt;:-I",  "(dunce's face)",
":&lt;)",   "(grin from an Ivy League School)",
":-)-8",    "(big girl's grin)",
"+-:-)",    "(clergy's grin)",
"O :-)",    "(angel's grin)",
"C=:-)",    "(chef's grin)",
"E-:-)",    "(amateur radio operator's grin)",
"8 :-)",    "(wizard's grin)",
">:->",     "(devilish face)",
"@:-)",     "(grin from a turban wearer)",
">;->",     "(lewd face)",
"B:-)",     "(grin with specs on head)",
"::-)",     "(grin with normal glasses)",
"8:-)",     "(little girl's grin)",
":-{)",     "(grin with mustache)",
":-{}",     "(grin with lipstick)",
"{:-)",     "(grin with toupee)",
"}:-(",     "(sad face with toupee in an updraft)",
":-)~",     "(drooling grin)",
":-~)",     "(grin with a cold)",
":'-(",     "(crying face)",
":'-)",     "(face crying with happiness)",
"=:-)",     "(grin with hosepipe on head)",
"-:-)",     "(punk rocker's grin)",
"-:-(",     "(punk rocker's scowl)",
"`:-)",     "(grin with an eyebrow shaved off)",
",:-)",     "(grin with an eyebrow shaved off)",
"[:-)",     "(grin from a walkman wearer)",
":-)",      "(grin)",
":-(",      "(sad face)",
";-)",      "(wink)",
":->",      "(big wink)",
":-I",      "(indifferent face)",
"(-:",      "(left-handed grin)",
"%-)",      "(tired-eyed grin)",
":*)",      "(drunk grin)",
"[:]",      "(robot face)",
"8-)",      "(grin with specs)",
"B-)",      "(grin with horn-rimmed glasses)",
":-[",      "(vampire's face)",
":-E",      "(bucktoothed vampire's face)",
":-F",      "(bucktoothed vampire's face with one tooth missing)",
":-7",      "(wry face)",
":-*",      "(face having eaten something sour)",
":-@",      "(screaming face)",
":-#",      "(wearing braces)",
":^)",      "(grin with broken nose)",
":v)",      "(grin with broken nose)",
":_)",      "(grin with nose sliding off face)",
":-&",      "(tongue tied face)",
":=)",      "(grin with two noses)",
"|-I",      "(sleeping face)",
"|-O",      "(yawning or snoring face)",
":-Q",      "(smoking face)",
":-0",      "(surprised or shouting face)",
".-)",      "(one-eyed grin)",
",-)",      "(one-eyed wink)",
"X-(",      "(dead face)",
":-?",      "(pipe-smoking face)",
"O-)",      "(SCUBA diver's grin)",
":-9",      "(licking lips)",
"(:I",      "(egghead face)",
"%-6",      "(brain dead face)",
":-/",      "(skeptical face)",
":-`",      "(spitting face)",
":-S",      "(incoherent face)",
":-D",      "(laugh)",
":-X",      "(sealed lips)",
":-C",      "(very sad face)",
":-o",      "(worried face)",
":)",       "(smug grin)",
":}",       "(smug grin)",
":]",       "(friendly little grin)",
":(",       "(sad face)",
":O",       "(surprised face)",
"@=",       "(pro- nuclear war symbol)",
"END"};
