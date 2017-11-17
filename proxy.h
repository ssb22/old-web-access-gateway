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
