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
#ifndef IMAGES_H
#define IMAGES_H
#include "platform.h"
// NB This file is included by platform.h, so we can't
// include webget.h.  Instead the enum is put here
// #include "webget.h"
enum ShowCharsAs { CharsIMG, CharsDbl, CharsAmp, CharsUtf,
CharsSJIS, CharsNoConv };
#ifdef ENCODINGS_AVAILABLE
#define CAN_SHOW_IMAGES
extern ShowCharsAs subHtmlShowCharsAs;
// Hacks for converting the legacy options:
#define LCharsAmp "2"
#define LCharsDbl "1"

#define RealUsingImages (subHtmlShowCharsAs==CharsIMG || subHtmlShowCharsAs==CharsDbl || subHtmlShowCharsAs==CharsSJIS)

#define LinksNeedBrackettingForImages (subHtmlShowCharsAs==CharsIMG || subHtmlShowCharsAs==CharsDbl)
// (2000-09-19: Not if CharsSJIS, since iMode handsets identify links OK)
#define NeedCharset ((subHtmlShowCharsAs==CharsAmp || subHtmlShowCharsAs==CharsUtf)?"UTF-8":(subHtmlShowCharsAs==CharsSJIS)?"Shift_JIS":NULL)
#else
#define RealUsingImages (0)
#define LinksNeedBrackettingForImages (0)
#define NeedCharset (NULL)
#endif // ENCODINGS_AVAILABLE

#ifdef DEVELOPMENT_COPY
extern int dontProcessLinks;
#define UsingImages (!dontProcessLinks && RealUsingImages)
#else
#define UsingImages RealUsingImages
#endif

#endif
