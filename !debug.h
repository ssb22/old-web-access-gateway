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
#include "platform.h"
#ifdef DEVELOPMENT_COPY

// See also debug.h's Have_Checkpoints (for resolving sig11)

//#define DEBUG_MIME_HEADER "/tmp/mimehdr"
//#define PRESERVE_TEMP_FILES
//#define DEBUGGING_GETWEBPAGE
//#define DEBUGGING_HTTPHDR
//#define DEBUGGING_DEVNULL
//#define DEBUGGING_CGI_INPUT
//#define DEBUGGING_WEBGET_FN
//#define DEBUGGING_TABLES
//#define DONT_CLEAR_ON_EXCEPTIONS
//#define DEBUG_ADD_TEXT_PLAIN
//#define DEBUGGING_USER_AGENT
//#define DEBUGGING_SCORES

// export HTTP_USER_AGENT=Netscape; export REMOTE_HOST=ssb22.joh.cam.ac.uk; export QUERY_STRING=Au=www.cam.ac.uk; ./access
// lynx -mime_header http://access.ucam.org/cgi-bin/access?Au=www.cam.ac.uk |less
#endif
