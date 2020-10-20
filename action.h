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
#ifndef ACTION_H
#define ACTION_H
#include <string.h>

#include "images.h"
#include "cgilib.h"
#include "options.h"

// NEEDATTENTION This could be generating some code bloat

#define ACTION_URL      "http://" IP_ADDRESS CGI_LOCATION
#define SSL_ACTION_URL "https://" IP_ADDRESS CGI_LOCATION

#ifdef CAN_SWITCH_SSL
#define UseSSL WantSSL
#else
#define UseSSL UsingSSL
#endif

extern bool UsingSSL;
#define MakeActionURL(a,b) (UseSSL?(a SSL_ACTION_URL b):(a ACTION_URL b))
#ifndef RELATIVE_CGI_LOCATION
// support old platform.h's
#define RELATIVE_CGI_LOCATION CGI_LOCATION
#endif
#ifdef CAN_SWITCH_SSL
extern bool WantSSL;
#define SwitchedSSL (UsingSSL != WantSSL)
// (If switch to/from SSL, need an absolute action URL)
#else
#define SwitchedSSL (0)
#endif // CAN_SWITCH_SSL
#define MakeRelativeActionURL(a,b) ((UsingImages || SwitchedSSL)?MakeActionURL(a,b):(a RELATIVE_CGI_LOCATION b))
// Would just a "?" do?  (Might not on some browsers e.g. iMode or old ones)
// Also the UsingImages thing - really want something that sets the BASE HREF to whatever would save most bandwidth in each case

// NEEDATTENTION This is a bit hacky
#ifdef CAN_SWITCH_SSL
inline void SwitchIfNeeded(const CGIEnvironment* e,const char* url) {
if(!(e->getName(ENV_AVOID_SWITCH,MAY_RETURN_NULL))) {
if(UsingSSL) {
if(!strncmp(url,"http://",7)
|| !strncmp(url,"ftp://",5)) WantSSL=0;
} else {
if(!strncmp(url,"https://",8)) WantSSL=1;
}
}
}
#define UnSwitch WantSSL=UsingSSL
#else
#define SwitchIfNeeded(e,url) ((void)0)
#define UnSwitch ((void)0)
#endif

#endif
