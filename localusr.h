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
#ifndef LOCALUSR_H
#define LOCALUSR_H
#include "platform.h"
#ifdef LOCAL_DNS_SUFFIX
#ifndef LOCAL_IP_PREFIX
#define LOCAL_IP_PREFIX
#endif
#endif
#ifdef LOCAL_IP_PREFIX
#include "cgilib.h"
/// @return non-0 if the user qualifies as "local" and can use the gateway (if there is a restriction)
int userIsLocal(const CGIEnvironment* theEnvironment);
/// @return non-0 if the user can use the gateway anyway (e.g. we've got an external proxy for them to go through)
int dealWithUnlocalUser(CGIEnvironment* theEnvironment);
#endif
#endif
