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
#ifndef HTMLFILT_H
#define HTMLFILT_H
#include "platform.h"
#include "cgilib.h"
#include "banner.h"
#include <stdio.h>
/**
Decodes the configuration options, and filters an HTML page.  May call itself
if it needs to retrieve a frame.
@param theEnvironment The CGI environment to output to and read configuration from
@param in The input file of the original (or pre-filtered) HTML page
@param currentPage The absolute URL of the original HTML page, used for relative URLs
@return The value of hadMeaningfulThisRow (if it's a frameset then the caller can decide if it's a good idea to draw a line)
*/
void htmlFilter(CGIEnvironment* theEnvironment,FILE* in,const char* currentPage,int isPlainText=0,int frameNesting=0,BannerVariables* banner=NULL);

/**
Sets the upper limit on the frame nest depth.  If there were no limit,
someone could cause a stack overflow by pointing a FRAME SRC to the main page. */
#define MAX_FRAME_NESTING 10

#endif
