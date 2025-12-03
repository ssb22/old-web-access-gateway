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
#ifndef MAKEFORM_H
#define MAKEFORM_H
#include "cgilib.h"
/// outputAllOptions outputs the complete form for changing options
void outputAllOptions(CGIEnvironment* theEnvironment);
/// @return 1 if need a line (HR)
int outputOptionsLink(CGIEnvironment* theEnvironment,const char* defaultURL=NULL);
inline void outputStartPageLine(CGIEnvironment* theEnvironment) {
theEnvironment->h_fputs("<HR TITLE=\"");
theEnvironment->h_fputs(L_START_PAGE);
theEnvironment->h_puts("\">"); // Don't change to h_fputs (see comment in access.c++ about Mozilla bug)
}
void outputBodyTag(CGIEnvironment* theEnvironment);
void outputIsIndex(CGIEnvironment* theEnvironment);
/// initTheFont is called by outputBodyTag, but is also used elsewhere (when need to reset it)
void initTheFont(CGIEnvironment* theEnvironment,int unconditional=0,const char* colour=NULL);
void closeTheFontTag(CGIEnvironment* theEnvironment,int unconditional=0);
#endif
