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
#include "help.h"
#include "platform.h"
#include "literals.h"
#include "action.h"
#include <string.h>
#include <stdlib.h>

#define HELP_BUFLEN 100
#ifdef TBL_SUBDIR
#define HELP_FILENAME TBL_SUBDIR "help.htm"
#else
#define HELP_FILENAME "help.htm"
#endif

Helper::~Helper() { if(theHelpFile) fclose(theHelpFile); }

#define HELP_OPTION_COMMENT "<!-- @@Option: "
// (don't use any printf formatting characters in that)
#define HELP_ENDOPTION "<!-- @@EndOption"

void Helper::help(const char* option,CGIEnvironment* theEnvironment) {
// First, find out if the user actually wants help
// (the "static" is a hack to avoid repeated queries)
// NEEDATTENTION need to sort that out if doing FastCGI
static int userWantsHelp=-1;
if(userWantsHelp==-1) userWantsHelp=((theEnvironment->getName(ENV_INLINE_HELP,MAY_RETURN_NULL)!=NULL)?1:0);
// (hack for compilers that don't like initialising a
// static with a value that can't be precomputed)
if(userWantsHelp) {
// Lazily open the help file (only when actually needed)
if(!theHelpFile) {
theHelpFile=fopen(HELP_FILENAME,"rt");
if(!theHelpFile) throw "Failed to open help file " HELP_FILENAME;
}
char line[HELP_BUFLEN+1],inOption=0;
char lookingFor[HELP_BUFLEN+1];
sprintf(lookingFor,HELP_OPTION_COMMENT "%s",option); // %s OK (we're providing "option", not the user)
int found=0;
// This is a bit hacky (could be better if we move to
// FastCGI or something and the object becomes
// persistent)
// The following code should be fast enough as long as
// the help file is kept roughly in the same order as
// the forms themselves
for(int rewoundYet=0; rewoundYet<=1; rewoundYet++) {
while(fgets(line,HELP_BUFLEN,theHelpFile)) {
while(strlen(line) && line[strlen(line)-1]<' ') line[strlen(line)-1]=0;
if(!inOption && strstr(line,lookingFor)) {
inOption=1; found=1;
theEnvironment->h_fputs("<UL><LI>");
} else if(inOption && strstr(line,HELP_ENDOPTION)) {
inOption=0;
theEnvironment->h_fputs("</LI></UL>");
break;
} else if(inOption) theEnvironment->h_puts(line);
}
if(inOption) throw "Helper::help: You're missing an @@EndOption";
if(found) break;
if(rewoundYet) {
//throw "Helper::help: Failed to find help text for option";
#ifdef DEVELOPMENT_COPY
theEnvironment->h_puts("<BR><B>Error:</B> Failed to find help text for option"); // no need to translate that (shouldn't happen)
theEnvironment->h_puts(option);
#endif
// Actually, don't really need to do anything
} else fseek(theHelpFile,0L,SEEK_SET); // Can't trust some compilers with rewind()
}
}
}

void Helper::outputHelpSwitch(CGIEnvironment* theEnvironment) {
theEnvironment->h_printf("<LABEL FOR=hlpsw><INPUT TYPE=Submit ID=hlpsw NAME=" ENV_SHOW_OPTIONS " VALUE=\"%s\"></LABEL>\n",theEnvironment->getName(ENV_INLINE_HELP,MAY_RETURN_NULL)?L_HELP_OFF:L_HELP_ON); // %s OK here
const char* oldOpts=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL);
if(oldOpts) {
theEnvironment->h_fputs("<INPUT TYPE=hidden NAME=" ENV_OLD_SHOW_OPTIONS " VALUE=\"");
theEnvironment->h_fputs(oldOpts);
theEnvironment->h_puts("\">");
}
}

void Helper::dealWithHelpSwitch(CGIEnvironment* theEnvironment) {
// Old value of ENV_SHOW_OPTIONS is in ENV_OLD_SHOW...
// (current value is L_HELP_ON or L_HELP_OFF)
// Restore ENV_SHOW_OPTIONS
// Also set or clear ENV_INLINE_HELP appropriately
const char* c=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL);
if(c) {
if(!strcmp(c,L_HELP_ON)) theEnvironment->setName(ENV_INLINE_HELP,"1");
else if(!strcmp(c,L_HELP_OFF)) theEnvironment->deleteName(ENV_INLINE_HELP);
else return; // don't bother with the next bit
const char* oldC=theEnvironment->getName(ENV_OLD_SHOW_OPTIONS,MAY_RETURN_NULL);
if(oldC) theEnvironment->setName(ENV_SHOW_OPTIONS,oldC);
}
}

void outputHelp(CGIEnvironment* theEnvironment) {
FILE* helpFile=fopen(HELP_FILENAME,"rt");
if(!helpFile) throw "Failed to open help file " HELP_FILENAME;
char line[HELP_BUFLEN+1],inBody=0;
while(fgets(line,HELP_BUFLEN,helpFile)) {
while(strlen(line) && line[strlen(line)-1]<' ') line[strlen(line)-1]=0;
if(!inBody && !strcmp(line,"<BODY>")) inBody=1;
else if(inBody && !strcmp(line,"</BODY>")) inBody=0;
else if(inBody) theEnvironment->h_puts(line);
}
fclose(helpFile);
// "Return to the form" link:
theEnvironment->h_fputs(MakeRelativeActionURL("<BR><A HREF=\"","?"));
if(theEnvironment->urlDumpCurrentOptions("")) theEnvironment->h_putchar('&');
theEnvironment->h_fputs(ENV_SHOW_OPTIONS "=1\">");
theEnvironment->h_fputs(L_HELP_RETURN);
theEnvironment->h_puts("</A>");
}
