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
#ifndef STRFUNC_H
#define STRFUNC_H
#include "platform.h"
#include <string.h>
#ifdef STR_FUNCTIONS_NEED_IMPLEMENTING
char* strchr (const char* szSearch, int cFor);
char* strrchr (const char* szSearch, int cFor);
char* strstr(const char* szSearch, const char *szFor);
#endif
#ifdef STRICMP_NEEDS_IMPLEMENTING
int strnicmp(const char* s1,const char* s2,int n);
int stricmp(const char* s1,const char* s2);
#endif
char* stristr(const char* szSearch, const char *szFor);
char* strnew(const char* s);
char* strnew(const char* s,int n);

#endif
