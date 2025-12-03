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
