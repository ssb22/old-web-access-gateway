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
#include "linkfol.h"
#include "tabtrim.h"
#include "strfunc.h"
#include "tagstuff.h"
#include "access.h"
#include "debug.h"
#include <string.h>

StringTable LinkFollow::urlsAlreadyHad;

void LinkFollow::setStartOfLink(const CGIEnvironment* c,const char* href) {
checkPoint();
theOpenPos=c->getPos();
theHref=strnew(href);
if(strrchr(theHref,'#')) *(strrchr(theHref,'#'))=0;
}

void LinkFollow::setEndOfLink(CGIEnvironment* c,struct AccessOptions* ao) {
checkPoint();
if(!notAnHtmlDocument(theHref,c) && notHadAlready() && doesQueryMatch(c->dataFromPos(theOpenPos),theKeyword)) {
c->h_puts("{{{<BR>");
InString i; // Dummy
HTMLTag tag("FRAME");
tag.addAttribute("SRC",theHref);
ao->processTag(tag,i);
c->h_puts("<BR>}}}");
}
delete[] theHref;
}

int LinkFollow::notHadAlready() {
checkPoint();
if(urlsAlreadyHad.getValueOf(theHref)) return(0);
urlsAlreadyHad.addLine(theHref,"");
return(1);
}
