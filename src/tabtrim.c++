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
#include "tabtrim.h"
#include "strfunc.h"
#include <string.h>

enum Delimeters { and_delim=' ', or_delim=',' };
int doesQueryMatch(const char* data,const char* in_keywords) {
int found=0;
char* keywords=strnew(in_keywords),*k=keywords;
while(!found) {
/* Find next AND and OR, and break off the first chunk */
// Remeber: OR is *lower* precidence!
char *nextAnd=strchr(k,and_delim),
*nextOr=strchr(k,or_delim);
char *breakPoint=((nextOr && nextOr<nextAnd)?nextOr:nextAnd);
char oldValue=0, *nextTry=breakPoint;
if(breakPoint) { oldValue=*breakPoint; *breakPoint=0; }

/* Decide what to do next based on whether this is found
(ignore empty terms, to cope with multiple spaces etc) */
if(*k) {
if(stristr(data,k)) {
if(!nextAnd || (nextOr && nextOr<nextAnd)) found=1;
} else nextTry=nextOr;
}

if(breakPoint) *breakPoint=oldValue;
if(nextTry) k=nextTry+1; else break;
} delete[] keywords; return found;
}

void TableTrim::setCloseTR(CGIEnvironment* c) {
if(!doesQueryMatch(c->dataFromPos(theOpenPos),theKeyword))
c->truncateAtPos(theOpenPos);
}
