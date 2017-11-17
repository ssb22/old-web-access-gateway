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
#ifndef STRTAB_H
#define STRTAB_H
#include <stdio.h>
/**
A class for storing a key/value table of strings.
Designed for small numbers of key/value pairs
and must guarantee to store things in the
given order.
*/
class StringTable {
public:
// No point in special hashing functions here
StringTable();
~StringTable();
void addLine(const char* lhs,const char* rhs);
const char* getValueOf(const char* lhs) const;
const char* getValueFromTop(const char* lhs) const;
const char* getCaseSensitiveValueOf(const char* lhs) const;
const char* getLhsByNumber(int n) const { return(theLHSs[n]); }
const char* getRhsByNumber(int n) const { return(theRHSs[n]); }
int getNumberOfValues() const { return(theNextLinePtr); }
void deleteAllByName(const char* lhs);
void deleteAllByCaseSensitiveName(const char* lhs);
void setByCaseSensitiveName(const char* lhs,const char* rhs); // Used by CGILib
void deleteByNumber(int i);
void setRhsDontNewIt(int n,char* rhs); // Used by tag / make URLs absolute
void setRhs(int n,const char* rhs); // again
void saveTo(FILE* fp) const;
void loadFrom(FILE* fp);
void saveTo(const char* filename) const;
void loadFrom(const char* filename);
protected:
char **theLHSs,**theRHSs;
int theNextLinePtr,theNumLines;
};
#endif
