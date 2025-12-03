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
#include "subhtml.h"
#include <ctype.h>
#include "strfunc.h"
#include "ampersnd.h"
#include "debug.h"

#ifdef ENCODINGS_AVAILABLE

TEContainer* encodingsInCurrentUse=NULL;

int subHtmlFeof(FILE* in,InString &htmlLineInput,int lineInputPtr) {
return(feof(in) && htmlLineInput.length()==lineInputPtr);
}

#endif

int text_fgetc(FILE* stream) {
checkPoint();
// Deal with all text formats
int c=0; while(!c) c=fgetc(stream); // Ditch the nulls (NEEDATTENTION What of Unicode sites?)
if(c=='\r') {
if((c=fgetc(stream))!='\n') ungetc(c,stream);
return('\n');
} else if(c=='\n') {
if((c=fgetc(stream))!='\r') ungetc(c,stream);
return('\n');
} else return(c);
}

char* text_fgets(char*buf,size_t len,FILE* f) {
checkPoint();
int c; size_t read=0;
while(read<len-1) {
c=text_fgetc(f);
if(c==EOF) {
if(!read) return NULL;
else break;
} else buf[read++]=c;
if(c=='\n') break;
}
buf[read]=0;
return buf;
}

#ifdef ENCODINGS_AVAILABLE
bool doneConversionAlready=false;
int subHtmlFgetc(FILE* in,InString &htmlLineInput,int &lineInputPtr) {
checkPoint();
int ch=((const unsigned char*)htmlLineInput.getString())[lineInputPtr++];
// (2000-12-14: must be unsigned, otherwise get problems with character 255 (= -1, = EOF)
if(!ch) {
htmlLineInput.clear(); lineInputPtr=0;
if(encodingsInCurrentUse && !doneConversionAlready) {
InString rawInput;
int x;
if((x=encodingsInCurrentUse->convertLine(in,rawInput))!=0) ch=EOF;
else {
ampersandFilter(rawInput.getString(),htmlLineInput);
if(x) text_fgetc(in); // Just to really make sure that feof(in) is set
ch='\n';
}
} else ch=htmlLineInput.addLineFromFile(in);
}
if(ch==EOF) text_fgetc(in); // Just to really make sure that feof(in) is set
return(ch);
}

#endif

int ignoreSpacesSubHtmlFgetc(FILE* in,InString &htmlLineInput,int &lineInputPtr) {
checkPoint();
int c=' ';
while(c!=EOF && isspace(c)) c=subHtmlFgetc(in,htmlLineInput,lineInputPtr);
return(c);
}

int subHtmlTryGetString(const char* stringToMatch,int len,InString htmlLineInput,int&lineInputPtr) {
checkPoint();
if(!strnicmp(htmlLineInput.getString()+lineInputPtr,stringToMatch,len)) {
lineInputPtr+=len; return(1);
} else return(0);
}
