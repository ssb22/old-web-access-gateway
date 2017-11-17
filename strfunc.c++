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
#include "platform.h"
#include <string.h>
#include <ctype.h>
#include "debug.h"

char* strnew(const char* s) {
checkPoint();
char* x=new char[strlen(s)+1]; strcpy(x,s); return(x);
}

#ifdef STR_FUNCTIONS_NEED_IMPLEMENTING
char* strchr (const char* szSearch, int cFor) {
checkPoint();
for(int lp=0; szSearch[lp]; lp++) if(szSearch[lp]==cFor) return((char*)(szSearch+lp));
return(NULL);
}

char* strrchr (const char* szSearch, int cFor) {
checkPoint();
for(int lp=strlen(szSearch)-1; lp>=0; lp--) if(szSearch[lp]==cFor) return((char*)(szSearch+lp));
return(NULL);
}

char* strstr(const char* szSearch, const char *szFor) {
checkPoint();
int j=0; char* retVal=NULL;
for(int i=0; szSearch[i]; i++) {
if(szSearch[i]==szFor[j]) {
if(!j) retVal=(char*)(szSearch+i);
j++;
if(!szFor[j]) return(retVal);
} else j=0;
} return(NULL);
}
#endif

char* stristr(const char* szSearch, const char *szFor) {
checkPoint();
int j=0; char* retVal=NULL;
for(int i=0; szSearch[i]; i++) {
if(toupper(szSearch[i])==toupper(szFor[j])) {
if(!j) retVal=(char*)(szSearch+i);
j++;
if(!szFor[j]) return(retVal);
} else j=0;
} return(NULL);
}

// DO NOT use this function without reading the comments below!
int strnicmp(const char* s1,const char* s2,int n) {
checkPoint();
// Previously I had a comment here saying "Let's do an inefficent implementation, might as well"
// but gprof said that my program often spends most of its time in this small function.
int r,char1,char2;
for(;n;--n) { // PLEASE optimse that to the loop instruction, gcc!
char1=*s1++; char2=*s2++;
if((r=toupper(char2)-toupper(char1))!=0) return(r);
if(!char1) return (char2>0);
if(!char2) return -(char1>0);
}
return(0);
}

#ifdef STRICMP_NEEDS_IMPLEMENTING
int stricmp(const char* s1,const char* s2) {
checkPoint();
// This function is called a LOT.  It needs to be
// optimised.
int r,c1,c2;
while(1) {
c1=*s1; c2=*s2;
r=toupper(c2)-toupper(c1);
if(r || !c2) return r;
s1++; s2++;
}
}
#endif

char* strnew(const char* s,int n) {
checkPoint();
char* x=new char[n+1];
strncpy(x,s,n); x[n]=0;
return x;
}
