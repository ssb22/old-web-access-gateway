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
#ifndef DEBUG_H
#define DEBUG_H

//#define Have_Checkpoints

#ifdef Have_Checkpoints
#include <stdio.h>
#include <string.h>
extern char checkpointBuf[];
class DebugCheckPoint {
public:
DebugCheckPoint(const char* file,int line) {
oldOne=new char[strlen(checkpointBuf)+1];
strcpy(oldOne,checkpointBuf);
sprintf(checkpointBuf,"%s line %d",file,line); // %s OK
}
~DebugCheckPoint() {
strcpy(checkpointBuf,oldOne);
delete[] oldOne;
}
private:
char* oldOne;
};
// Can't say dcp##__LINE__ (or equivalent) because it processes the ##'s etc before substituting __LINE__ in
// $__LINE__ ($ being a GNU extension) also doesn't work
// Therefore one per scope! or call namedCheckPoint
#define checkPoint() DebugCheckPoint myDebugCheckPoint(__FILE__,__LINE__)
#define namedCheckPoint(n) DebugCheckPoint n(__FILE__,__LINE__)
#else
#define checkPoint() ((void)(0))
#define namedCheckPoint(n) ((void)(0))
#endif // Have_Checkpoints

#ifdef MAKE_FREQUENCY_TABLE
extern int theNewFrequencyTable[65536];
#endif

#endif
