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
