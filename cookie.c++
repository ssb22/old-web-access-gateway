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

#include "allinc.h"
#include "cookie.h"
#include "strfunc.h"
#include "debug.h"
#include "mylynx.h" // For CRLF
#include <ctype.h>

#define COOKIE_SEPARATOR ','
// 2000-11-11: Not ';' because it confuses refresh tags (try ',')
#define LEGACY_COOKIE_SEPARATOR ';' // so still recognised

void CookieTable::processSetCookieHeader(const char* content) {
checkPoint();
InString value,domain,path; int i=0;
while(content[i] && content[i]!=';') value.addCharacter(content[i++]);
while(content[i]==';' || (isascii(content[i]) && isspace(content[i]))) i++;
while(content[i]) {
if(!strnicmp(content+i,"domain=",7)) {
i+=7;
while(content[i] && content[i]!=';') domain.addCharacter(content[i++]);
} else if(!strnicmp(content+i,"path=",5)) {
i+=5;
while(content[i] && content[i]!=';') path.addCharacter(content[i++]);
} else while(content[i] && content[i]!=';') i++;
while(content[i]==';' || (isascii(content[i]) && isspace(content[i]))) i++;
}
// If path is "/", ignore it, otherwise use it
if(strcmp(path.getString(),"/")) domain.addString(path.getString());
set(value.getString(),domain.getString());
}

void CookieTable::writeToString(InString &s) const {
checkPoint();
for(int i=0; i<theData.getNumberOfValues();i++) {
if(i) s.addCharacter(COOKIE_SEPARATOR);
// 2001-03-16: We need to handle " characters specially
// because otherwise it confuses hidden form fields.
// 2001-03-17: Actually need to drop them, apparently
// (even though it's not in the spec, it's what browsers
// do, and keeping them has confused some servers)
// addStringWithQuoteEscape modified
// Actually dropping them had no effect (only rdhw's
// suggestion), so reverting
// See also rfsServiceFunc
s.addStringWithQuoteEscape(theData.getLhsByNumber(i));
s.addCharacter(COOKIE_SEPARATOR);
s.addStringWithQuoteEscape(theData.getRhsByNumber(i));
}
}

enum { OverheadPerCookie = 2 };

int CookieTable::length() const {
checkPoint();
int len=0;
for(int i=0; i<theData.getNumberOfValues();i++) {
len+=(strlen(theData.getLhsByNumber(i))+strlen(theData.getRhsByNumber(i))+OverheadPerCookie);
}
return len;
}

void CookieTable::reduceLengthBy(int val) {
checkPoint();
int done=0;
while(done<val && theData.getNumberOfValues()) {
done+=(strlen(theData.getLhsByNumber(0))+strlen(theData.getRhsByNumber(0))+OverheadPerCookie);
theData.deleteByNumber(0);
}
}

inline void rfsServiceFunc(InString &i,const char* &s) {
while(*s && *s!=COOKIE_SEPARATOR && *s!=LEGACY_COOKIE_SEPARATOR) {
if(*s=='\\') {
s++; if(*s=='q') { i.addCharacter('\"'); s++; }
else if(*s) i.addCharacter(*s++);
} else i.addCharacter(*s++);
}
if(*s) s++; // skip over the separator
}

void CookieTable::readFromString(const char* s) {
checkPoint();
InString lhs,rhs;
while(*s) {
rfsServiceFunc(lhs,s);
rfsServiceFunc(rhs,s);
set(lhs.getString(),rhs.getString());
lhs.clear(); rhs.clear();
}
}

void CookieTable::appendToUserAgent(InString &s,const char* url) {
checkPoint();
// NEEDATTENTION Do we delete any cookies that shouldn't be sent to that URL (or that domain) to cut down?  (should be OK for now because we have a size limit)
if(!strnicmp(url,"http://",7)) url+=7;
#ifdef LYNX_SSL
else if(!strnicmp(url,"https://",8)) url+=8;
// NEEDATTENTION Shouldn't the paths for SSL cookies be regarded as different from those on the insecure server?  (Well, we did say that using the gateway on secure sites is a compromise anyway...)
#endif
int hadOneYet=0;
for(int i=0; i<theData.getNumberOfValues();i++) {
// See if the cookie's domain/path matches this URL
const char* domainAndPath=theData.getRhsByNumber(i);
int numToCompare=strlen(domainAndPath);
const char* url2=url;
// Abort the comparison at first / (domain didn't match)
while(*url2 && *url2!='/') {
if(!strnicmp(url2,domainAndPath,numToCompare)) {
// Match
s.addString(hadOneYet?"; ":CRLF "Cookie: ");
hadOneYet=1;
s.addString(theData.getLhsByNumber(i));
break;
}
url2++;
}
}
}

void CookieTable::set(const char* value,const char* path) {
checkPoint();
// If value is of the form A=B, allow overwrite of any
// previous cookie of same A
// *** we probably need to check paths (or at least
// domains) are the same NEEDATTENTION
const char* eq=strchr(value,'=');
if(eq) {
int numToCompare=eq-value;
for(int i=0; i<theData.getNumberOfValues();i++) {
if(!strnicmp(theData.getLhsByNumber(i),value,numToCompare)) {
theData.deleteByNumber(i);
break;
}
}
}

int lenToAdd=strlen(value)+strlen(path)+OverheadPerCookie;
if(lenToAdd>theMaxLength) return; // Forget that!
int diff=length()+lenToAdd-theMaxLength;
if(diff>0) reduceLengthBy(diff);

//theData.addLine(value,path);
theData.setByCaseSensitiveName(value,path); // again causes a problem if having multiple paths/domains NEEDATTENTION
}

const char* getUserCookie(const char* content) {
checkPoint();
while(*content && content[0]!=COOKIE_LETTER && content[1]!='=') {
// Not a Q=, so skip past next semicolon
while(*content && *content!=';' && *content!=',') content++;
if(*content) {
do content++; while(*content && isspace(*content));
}
}
if(*content) {
// Found a Q=
content+=2; if(*content) {
InString retVal;
int ampToEqual=1;
while(*content && *content!=';' && *content!=',') {
if(*content=='&') {
// Convert every odd & to an =
// (see encodeUserCookie)
retVal.addCharacter(ampToEqual?'=':'&');
ampToEqual=!ampToEqual;
} else retVal.addCharacter(*content);
content++;
}
return(strnew(retVal.getString()));
} else return NULL; // empty cookie
} else return NULL; // couldn't find our cookie
}

void encodeUserCookie(const char* query,InString &s) {
checkPoint();
s.addString(COOKIE_QUOTES_LETTER "=");
for(int i=0; query[i]; i++) {
if(query[i]=='=') s.addCharacter('&');
// (We can assume that it's of the form x=y&x=y&...,
// hence we know that every odd ampersand has to be
// converted to = on decode)
else if(strchr(";, ",query[i])) {
char buf[5]; sprintf(buf,"%%%02X",(int)((unsigned char)query[i]));
s.addString(buf);
} else s.addCharacter(query[i]);
}
}
