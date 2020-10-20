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
#ifndef PROXY_H
#define PROXY_H

#include "platform.h"

#ifdef PROXY_FOR_NONLOCALS
extern const char* nonLocalProxyOverride;
#endif

inline const char* proxy() {
// (Note: The length of this string is trusted.  If
// implementing user proxy override, careful (also
// careful that a non-local user can't override to a
// local proxy; etc))
#ifdef PROXY_FOR_NONLOCALS
if(nonLocalProxyOverride) return nonLocalProxyOverride;
#endif
#ifdef PROXY
return PROXY;
#else
return NULL;
#endif
}

inline int proxyPort() {
#ifdef PROXY_PORT_FOR_NONLOCALS
if(nonLocalProxyOverride) return PROXY_PORT_FOR_NONLOCALS;
#endif
#ifdef PROXY_PORT
return PROXY_PORT;
#else
return 8080; // as good a guess as any
#endif
}

inline int proxyFtp() {
#ifdef PROXY_FTP
return 1;
#else
#ifdef PROXY_FOR_NONLOCALS
if(nonLocalProxyOverride) return 1;
#endif
return 0;
#endif
}

inline const char* proxyauth() {
// (The length of this string is trusted.  See above.)
#ifdef PROXY_AUTH_ID
return "-pauth=" PROXY_AUTH_ID ":" PROXY_AUTH_PASSWORD;
#else
return NULL;
#endif
}
inline const char* proxyauth_wget1() {
#ifdef PROXY_AUTH_ID
return "--proxy-user=" PROXY_AUTH_ID;
#else
return NULL;
#endif
}
inline const char* proxyauth_wget2() {
#ifdef PROXY_AUTH_PASSWORD
return "--proxy-passwd=" PROXY_AUTH_PASSWORD;
#else
return NULL;
#endif
}

#ifdef PROXY_AUTH_BASE64
#define PROXY_AUTH_HEADER "Proxy-Authorization: Basic " PROXY_AUTH_BASE64
#endif
#endif
