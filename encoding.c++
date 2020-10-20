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
#include "encoding.h"
#include "webget.h"
#include "tagstuff.h" // for stricmp on some platforms
#include "images.h"
#include "help.h"
#include "subhtml.h"
#include "action.h"
#include "!debug.h"
#include <string.h>
#include <stdlib.h>
#ifdef ENCODINGS_AVAILABLE

/// Maps TBL filenames to ISO language names
/// (not all TBL files have to appear here)
const char* isoLanguageNames[]={
"Arabic",   "ar",
"Chinese",  "zh",
"Croatian", "hr",
"Cyrillic", "ru",
"Danish",   "da",
"Farsi",    "fa",
"Finnish",  "fi", // NEEDATTENTION Swedish also uses Finnish.tbl
"French",   "fr",
"CanadFr",  "fr-CA",
"German",   "de",
"Greek",    "el",
"Hebrew",   "he",
"Iceland",  "is",
"Italian",  "it",
"Japanese", "ja",
"Korean",   "ko",
"Norway",   "no",
"Portgse",  "pt",
"Romanian", "ro",
"Spanish",  "es",
"Thai",     "th",
"Turkish",  "tr",
"Ukraine",  "uk",
"Urdu",     "ur",
"Vietnam",  "vi",
0};

/*
The following can go in, but they're no good yet because
we don't have mapping tables for them:
Afrikaans [af]
Albanian [sq]
Basque [eu]
Bulgarian [bg]
Byelorussian [be]
Catalan [ca]
Chinese [zh] [zh-CN] [zh-TW]
Croatian [hr]
Czech [cs]
Danish [da]
Dutch [nl]
Dutch/Belgium [nl-BE]
en, en-GB, en-US
Faeroese fo
Finnish fi
French fr
fr-BE fr-CA fr-FR fr-CH
Galician gl
German de
de-AT (austria), de-DE de-CH
Greek el
Hungarian hu
Icelandic is
Indonesian id
Irish ga
Italian it
ja, ko
Macedonian mk
Norwegian no
Polish pl
Portugese pt
[-BR brazil]
Romanian ro
Russian ru
Scots gaelic gd
Serbian sr
Slovak sk
Slovenian sl
spanish es
-AR argentina -CO colombia -MX mexico -ES spain
swedish sv
turkish tr
ukrainian uk
*/

/// Lists the names of the languages, and the .tbl file for each if different from the name
const char* encodings[]={
"Arabic",0,
"Austrian","German",
"Baltic",0,
"Catalan","Spanish",
"Central European","CentEuro",
"Chinese",0, // A hack in tecont.c++ depends on this file being called "Chinese" (fix it if it's changed)
"Croatian",0,
"Danish",0,
"East European","EastEuro",
"Estonian",0,
"Farsi",0, // (I.e. Persian)
"Finnish",0,
"French",0,
"French (Canadian)","CanadFr",
"German",0,
"Greek",0,
"Hebrew",0,
"Icelandic","Iceland",
"Italian",0,
"Japanese",0,
"Korean",0, // A hack in tecont.c++ depends on this file being called "Korean" (fix it if it's changed)
"Latvian",0,
"Lithuanian","Latvian",
"Norwegian","Norway",
"Portugese","Portgse",
"Romanian",0,
"Russian","Cyrillic",
"Spanish",0,
"Swedish","Finnish",
"Thai",0,
"Turkish",0,
"Ukrainian","Ukraine",
"Urdu",0,
"Vietnamese","Vietnam",
NO_CONVERS_VAL};

const char* latin1encodings[]={
"Danish","Finnish","French","German",
"CanadFr", // I think
// "Dutch", "English", "Faeroese", French, German, Irish
"Iceland","Italian","Norway","Portgse","Spanish",0};

extern int formID,checkOptionJustDumps;
void outputCheckOption(const char* option,const char* description,CGIEnvironment* theEnvironment);

void outputRadioOption(const char* option,const char* description,int value,int defaultValue,CGIEnvironment* theEnvironment) {
formID++;
const char* c=theEnvironment->getName(option,MAY_RETURN_NULL);
int currentValue=(c?atoi(c):defaultValue);
theEnvironment->h_printf("<BR><LABEL FOR=%d><INPUT TYPE=radio NAME=%s ID=%d USESTYLE VALUE=\"%d\"%s>%s</LABEL>\n",formID,option,formID,value,(value==currentValue)?" checked":"",description); // %s OK here
}

/// Outputs the encoding-related options to the options form
int outputEncodingOptions(CGIEnvironment* theEnvironment) {
int retVal=0;
const char* tmp=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL); if(tmp && !strcmp(tmp,L_SET_LANGUAGE_OPTIONS)) {
retVal=1;
Helper::outputHelpSwitch(theEnvironment);
theEnvironment->h_fputs("<BR>"); theEnvironment->h_puts(L_LANGUAGE_INTRO);
const char* selectedEncoding=theEnvironment->getName(ENV_LANGUAGE_TABLE,MAY_RETURN_NULL);
formID++; theEnvironment->h_printf("<p><LABEL FOR=%d>",formID); theEnvironment->h_puts(L_CONVERSION_PROMPT);
theEnvironment->h_printf(" <SELECT NAME=" ENV_LANGUAGE_TABLE " ID=%d>\n",formID);
theEnvironment->h_fputs("<OPTION VALUE=" NO_CONVERS_VAL ">"); theEnvironment->h_fputs(L_CONVERSION_NONE); theEnvironment->h_puts("</OPTION>");
int selThisOne=0; const char *e1,*e2;
for(int lp=0; strcmp(encodings[lp],NO_CONVERS_VAL); lp+=2) {
e1=encodings[lp]; e2=encodings[lp|1];
if(!e2) e2=e1;
if(selectedEncoding) {
selThisOne=!stricmp(selectedEncoding,e2);
if(selThisOne) selectedEncoding=NULL;
}
theEnvironment->h_printf("<OPTION VALUE=%s%s>%s</OPTION>\n",e2,selThisOne?" SELECTED":"",e1); // %s OK here
selThisOne=0;
}
theEnvironment->h_puts("</SELECT></LABEL>");

theEnvironment->h_fputs("<BR>");
theEnvironment->h_puts(L_SHOW_CHARS_AS);
outputRadioOption(ENV_LANG_SHOW_CHARS_AS,L_IMGS,CharsIMG,CharsIMG,theEnvironment);
#ifdef UNICODE_HEIGHT
outputRadioOption(ENV_LANG_SHOW_CHARS_AS,L_DOUBLE_IMGS,CharsDbl,CharsIMG,theEnvironment);
#endif
outputRadioOption(ENV_LANG_SHOW_CHARS_AS,L_AMPERSAND,CharsAmp,CharsIMG,theEnvironment);
outputRadioOption(ENV_LANG_SHOW_CHARS_AS,L_UTF8,CharsUtf,CharsIMG,theEnvironment);
outputRadioOption(ENV_LANG_SHOW_CHARS_AS,L_SHIFTJIS,CharsSJIS,CharsIMG,theEnvironment);

formID++; theEnvironment->h_printf("<BR><LABEL FOR=%d>%s: <INPUT TYPE=text USESTYLE ID=%d NAME=" ENV_UNICODE_URL " VALUE=\"",formID,L_UNICODE_URL,formID); // %s OK here
const char* defaultURL=theEnvironment->getName(ENV_UNICODE_URL,MAY_RETURN_NULL); if(!defaultURL) defaultURL="";
theEnvironment->h_fputs(defaultURL); theEnvironment->h_puts("\"></LABEL>");
Helper::hackyGlobalHelper()->help(ENV_UNICODE_URL,theEnvironment);
} else {
theEnvironment->formDumpCurrentOption(ENV_LANGUAGE_TABLE);
theEnvironment->formDumpCurrentOption(ENV_LANG_SHOW_CHARS_AS);
theEnvironment->formDumpCurrentOption(ENV_UNICODE_URL);
}
checkOptionJustDumps=!retVal;
outputCheckOption(ENV_OMIT_SUB_ENCODING,L_OMIT_SUB_ENCODING,theEnvironment);
checkOptionJustDumps=0;
theEnvironment->formDumpCurrentOption(ENV_ENCODING_DESCRIPTION);
theEnvironment->formDumpCurrentOption(ENV_OMIT_SERVER_ENCODING);
return retVal;
}

const char* getIsoLanguageNameOrNull(const CGIEnvironment* theEnvironment) {
const char* n=theEnvironment->getName(ENV_LANGUAGE_TABLE,MAY_RETURN_NULL);
if(n) {
for(int i=0; *(isoLanguageNames[i]); i+=2) {
if(!stricmp(n,isoLanguageNames[i])) return isoLanguageNames[i|1];
}
}
return NULL;
}

void setUpCharacterTranslation(CGIEnvironment* encodingEnvironment,const char* contentType,FILE* htmlInput) {
const char* lang=encodingEnvironment->getName(ENV_LANGUAGE_TABLE,MAY_RETURN_NULL);
if(!encodingsInCurrentUse) {
if(lang && strcmp(lang,NO_CONVERS_VAL)) {
if(strchr(lang,'.') || strchr(lang,'/') || strchr(lang,'\\')) throw "setUpCharacterTranslation: Invalid language";
InString s;
#ifdef TBL_SUBDIR
s.addString(TBL_SUBDIR);
#endif
s.addString(lang);
s.addString(".tbl"); FILE* eTbl=fopen(s.getString(),"rb");
if(!eTbl) throw "Could not open encodings datafile for given language";
encodingsInCurrentUse=new TEContainer(eTbl);
#ifdef DEBUGGING_WEBGET_FN
encodingEnvironment->h_puts("Debugger: Created encodingsInCurrentUse<BR>");
#endif
fclose(eTbl);
#ifdef DEBUGGING_WEBGET_FN
encodingEnvironment->h_puts("Debugger: Closed eTbl<BR>");
#endif
}
}
if(encodingsInCurrentUse) {
int charsetMethod=TEC_InvalidMethod;
if(encodingEnvironment->getName(ENV_OMIT_SUB_ENCODING,MAY_RETURN_NULL)) encodingsInCurrentUse->omitSubEncoding();
else {
const char* y=stristr(contentType,"charset="); if(y) {
// Bit of nasty coding here:
y=strchr(y,'=')+1; // there's GOING to be an '='
if(strchr(y,';')) ((char*)strchr(y,';'))[0]=0;
#ifdef DEBUGGING_WEBGET_FN
encodingEnvironment->h_printf("Debugger: MIME charset is %s<BR>\n",y); // %s OK (debugging)
#endif
charsetMethod=encodingsInCurrentUse->setAutoDetectMimeCharsetIfCharsetIsAppropriateToLanguage(y);
}
}
const char* desc=encodingEnvironment->getName(ENV_ENCODING_DESCRIPTION,MAY_RETURN_NULL);
int method=-2;
if(desc) method=encodingsInCurrentUse->getMethodByDescription(desc);
int specifiedMethod=method;
if(method==-2) {
if (charsetMethod!=TEC_InvalidMethod) method=charsetMethod;
else method=encodingsInCurrentUse->
autoDetectFrom(htmlInput
#ifdef DEBUGGING_SCORES
,encodingEnvironment
#endif
);
}
if(!preprocessOnly(encodingEnvironment)
&& !(encodingEnvironment->getName(DONT_SHOW_CHARSET,MAY_RETURN_NULL))) {
encodingEnvironment->h_printf("<BR>%.99s: %.99s %.99s",lang,(specifiedMethod==-2)?L_DETECTED:L_OVERRIDDEN,encodingsInCurrentUse->getDescription(method));
if(encodingEnvironment->getName(ENV_SHOW_ENCODING_OPTIONS,MAY_RETURN_NULL)) {
int j=encodingsInCurrentUse->getNumSpecialCases();
encodingEnvironment->h_puts(MakeRelativeActionURL("<FORM ACTION=\"","\" METHOD=\"GET\">"));
for(int i=-2; i<j; i++) {
const char* x=encodingsInCurrentUse->getDescription(i);
encodingEnvironment->h_printf("<BR><LABEL FOR=\"x%d\"><INPUT TYPE=\"radio\" USESTYLE ID=\"x%d\" NAME=\"" ENV_ENCODING_DESCRIPTION "\" VALUE=\"%.99s\"%.99s>%.99s</LABEL>\n",i,i,x,(specifiedMethod==i)?" CHECKED":"",x);
}
outputCheckOption(ENV_OMIT_SERVER_ENCODING,L_OMIT_SERVER_ENCODING,encodingEnvironment);
encodingEnvironment->formButOneDumpCurrentOptions(ENV_PREFIX,ENV_ENCODING_DESCRIPTION);
encodingEnvironment->formDumpCurrentOption(URL_ENV);
encodingEnvironment->h_puts("<BR>");
if(encodingEnvironment->getName(DONT_SHOW_OPTIONS_LINK,MAY_RETURN_NULL)) {
// "More Options" (because we didn't have
// "change access options" and the user might
// not know about them if they're following
// somebody else's link, but they did press
// "override" which means they might want to
// explore)
encodingEnvironment->h_fputs(MakeRelativeActionURL("<A HREF=\"","?"));
if(encodingEnvironment->urlDumpCurrentOptions("")) encodingEnvironment->h_putchar('&');
encodingEnvironment->h_fputs(ENV_SHOW_OPTIONS "=");
encodingEnvironment->h_fputs(L_SET_LANGUAGE_OPTIONS);
encodingEnvironment->h_fputs("\" TARGET=\"_top\">");
encodingEnvironment->h_fputs(L_MORE_OPTIONS);
encodingEnvironment->h_fputs("</A>");
}
encodingEnvironment->h_fputs("<INPUT TYPE=Submit USESTYLE VALUE=\"");
encodingEnvironment->h_fputs(L_ENCODING_SUBMIT_BUTTON);
encodingEnvironment->h_puts("\"></FORM>");
// <BR> and outputLastUpdate no longer necessary
// here, now it can be reached through "more
// options"
} else {
encodingEnvironment->h_fputs(MakeRelativeActionURL(" [<A HREF=\"","?"));
if(encodingEnvironment->urlDumpCurrentOptions("")) encodingEnvironment->h_putchar('&');
encodingEnvironment->h_fputs(ENV_SHOW_ENCODING_OPTIONS "=1\">");
encodingEnvironment->h_fputs(L_OVERRIDE);
encodingEnvironment->h_puts("</A>]");
}
#ifdef L_NO_FREQTBL
if(!encodingsInCurrentUse->hasGotFreqTable()) encodingEnvironment->h_puts(L_NO_FREQTBL);
#endif
} // !preprocessOnly && can show charset control
encodingsInCurrentUse->setMethod(method);
#ifdef MULTIPLE_STYLES_SUPPORTED
encodingEnvironment->setStyle(encodingsInCurrentUse->getRecommendedStyle(lang));
#endif
} // encodingsInCurrentUse
}

int inLatin1(const CGIEnvironment* encodingEnvironment) {
// Used by the windows-1252 thing
const char* selectedEncoding=encodingEnvironment->getName(ENV_LANGUAGE_TABLE,MAY_RETURN_NULL);
if(selectedEncoding) {
for(int i=0; latin1encodings[i]; i++) {
if(!strcmp(selectedEncoding, latin1encodings[i]))
return 1;
}
return 0;
} else return 1; // assume OK if not using languages
}

#endif
