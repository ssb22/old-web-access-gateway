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
#ifndef WEBGET_H
#define WEBGET_H

#include "cgilib.h"
#include "options.h"

#define CANONICAL_NAME "web_access_gateway"
#define CANONICAL_LEN 18

#define ENV_ENABLE_IGNORE ENV_PREFIX "ei"
#define BACKSLASH_ENABLE_TOKEN "access*gateway*ignore"
#define BACKSLASH_ENABLE_LEN 21

/// URL can be NULL, in which case got from environment.
/// lastPageReturned is used to return the actual URL found, for redirection purposes
/// - this should be used for future relative URLs.
/// Warning: getWebPage can output stuff (eg. about
/// refresh, encoding etc)
FILE* getWebPage(const char* url,CGIEnvironment* postDataOrNull,MayReturnNull mayReturnNull,InString &lastPageReturned,CGIEnvironment* encodingEnvironment,int dontTouchRefresh=0,int isTopLevel=0,int nestLevel=0,int returnAnyContentType=0);
void removeTempFiles();

bool needProtocolFor(const char* x); // used for "Leave gateway" links

#endif
