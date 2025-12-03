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
#include "platform.h"
#ifdef DEVELOPMENT_COPY
#include "tecont.h"
#include "strfunc.h"
#include "cgilib.h"
#include "makeform.h"
#include "htmlfilt.h"
#include "webget.h"
#include "instring.h"
#include "encoding.h"
#include <stdlib.h>
#include <ctype.h>
int dontProcessLinks=0;
const char* qsOverride=NULL;

void processArguments(int argc,char* argv[]) {
#ifdef MAKE_FREQUENCY_TABLE
void initFrequencyTable();
initFrequencyTable();
#endif
if(!getenv("REMOTE_HOST")) fprintf(stderr,"Development copy: Processing command-line arguments\n");
// tabledump ~/tbl/Japanese.tbl
// unicodeMV ~/tbl/Korean.tbl [-noiso] [-N] (the first encoding in it unless -N is used e.g. -1 for second, -2 for third; assumes ISO filenames & EUC codes, unless -noiso in which case assumes 8-bit 1st char & filenames the same)
// qs (querystring in next argument)
// textonly sourcefile > destfile (frames cause a problem!) (can set QUERY_STRING for options)
// runproject project-pathname (uses cli and stdin)

// textonly problems:
// Don't display mediator warning, robots etc
// Have options like "replace all images with their alt tags"
// BASE HREF problem (tries to substitute /tmp URLs)

for (int i=1; i<argc; i++) {
if(!stricmp(argv[i],"runproject")) {
CGIEnvironment* theEnvironment=new CGIEnvironment;
if(!theEnvironment) { puts("Error: Cannot create CGIEnvironment"); return; }
try {
const char* theCli=theEnvironment->getName("cli",MAY_RETURN_NULL);
const char* theStdin=theEnvironment->getName("stdin",MAY_NOT_RETURN_NULL);
InString sysTemp;
if(theStdin) {
sysTemp.addString("echo ");
sysTemp.addStringWithCommandPrecautions(theStdin);
sysTemp.addString(" | ");
}
sysTemp.addString(argv[++i]);
if(theCli) {
sysTemp.addCharacter(' ');
// sysTemp.addStringWithCommandPrecautions(theCli);
// Re-implementing because we want spaces to be propagated
for(int i=0; theCli[i]; i++) {
if(theCli[i]>=' ') {
if(!isalnum(theCli[i]) && !isspace(theCli[i])) sysTemp.addCharacter(COMMAND_SPECIAL_CHAR_PREFIX);
sysTemp.addCharacter(theCli[i]);
}
}
}
system(sysTemp.getString());
} catch(...) {
puts("Unhandled exception in the CGI handler (not in the program)");
}
// delete theEnvironment; // Don't - it outputs horrible stuff
} else if(!stricmp(argv[i],"qs")) qsOverride=argv[++i];
else if(!stricmp(argv[i],"textonly")) {
dontProcessLinks=1;
CGIEnvironment* theEnvironment=new CGIEnvironment;
if(!theEnvironment) { puts("Error: Cannot create CGIEnvironment"); return; }
try {
outputBodyTag(theEnvironment);
#ifdef Internal_Extensions1
ExtensionContainer* c=makeNewReturnOldExtensions();
#endif
FILE* inPage=fopen(argv[++i],"rt");
if(!inPage) { puts("Error: Cannot open input file"); return; }
setUpCharacterTranslation(theEnvironment,"text/html",inPage);
theEnvironment->setStartOfRealPage();
try {
htmlFilter(theEnvironment,inPage,argv[i]);
} catch(...) { fclose(inPage); throw; }
fclose(inPage);
#ifdef Internal_Extensions1
resetOldExtensions(c);
#endif
} catch(const char* x) {
printf("Error: Exception raised: %s\n",x); // %s OK here (development copy)
} catch(...) {
puts("Error: Unhandled exception type");
}
delete theEnvironment;
removeTempFiles();

} else if(!stricmp(argv[i],"unicodeMV")) {
TEContainer* t=new TEContainer(fopen(argv[++i],"rb"));
int toSubtract=0x8080;
if(i+1<argc && !stricmp(argv[i+1],"-noiso")) {
toSubtract=0; i++;
}
int encodingNumber=0;
if(i+1<argc && argv[i+1][0]=='-') {
encodingNumber=atoi(argv[++i]+1);
}
TableEncoding& te=t->theEncodings[encodingNumber];
fprintf(stderr,"Writing mv script for %s\n",te.theName); // %s OK here (development copy)
puts("mkdir unicode");
unsigned char data[3]; data[2]=0;
for(int d0=128; d0<256; d0++) {
data[0]=d0;
for(int d1=(toSubtract?128:32); d1<256; d1++) {
data[1]=d1;
if(te.tryAndConvert((char*)(&(data[0])))==CR_SUCCESS) {
printf("mv %04X.gif unicode/%04X.gif\n",((d0<<8)|d1) - toSubtract,te.getLastConvertedCharacter());
}
}
} delete t;
} else if(!stricmp(argv[i],"tabledump")) {
TEContainer* t=new TEContainer(fopen(argv[++i],"rb"));
int cases=t->getNumSpecialCases();
printf("Number of special cases: %d\n",cases);
for(int k=-2; k<cases; k++) {
printf("Case %d: %s\n",k,t->getDescription(k)); // %s OK here (development copy)
}
printf("Number of encodings: %d\n",t->theNumberOfEncodings);
for(int k=0; k<t->theNumberOfEncodings; k++) {
printf("Encoding number %d, %s\n",k,t->theEncodings[k].theName); // %s OK here (development copy)
printf("ISO designator of first plane: %d (%c)\n",t->theEncodings[k].theISODesignator,t->theEncodings[k].theISODesignator);
printf("Second parameter is set: %d\n",t->theEncodings[k].theSecondParameterIsSet);
printf("Maximum bytes per character: %d\n",t->theEncodings[k].theMaxBytesPerChar);
switch(t->theEncodings[k].theMaxBytesPerChar) {
case 2: {
printf("maps only 128 bytes: %d  single 7 bit valid: %d  ebcdic: %d\n",t->theEncodings[k].theMainTable.twoBytes.mapsAreOnly128Bytes,t->theEncodings[k].theMainTable.twoBytes.single7BitValidButNoMap,t->theEncodings[k].theMainTable.twoBytes.ebcdic);
printf("Valid secondary maps: ");
int from=-2,closed=1;
for(int j=t->theEncodings[k].theMainTable.twoBytes.mapsAreOnly128Bytes; j<TE_MAIN_TABLEN; j++) {
int zz=t->theEncodings[k].theMainTable.twoBytes.validSecondary[j];
if(zz) {
if(closed) { closed=0; printf("%d-",zz); }
from=zz;
} else if(!closed) { closed=1; printf("%d ",from); }
}
if(!closed) printf("%d ",from);
puts("");
} default: break;
}
} delete t;
} else printf("Unrecognised option: %s\n",argv[i]); // %s OK here (development copy)
}
}

#ifdef MAKE_FREQUENCY_TABLE
int theNewFrequencyTable[65536];
void initFrequencyTable() {
memset(theNewFrequencyTable,0,sizeof(theNewFrequencyTable));
FILE* f=fopen(MAKE_FREQUENCY_TABLE,"r");
if(f) {
fread(&theNewFrequencyTable,sizeof(theNewFrequencyTable),1,f);
fclose(f);
}
}
void exitFrequencyTable() {
FILE* f=fopen(MAKE_FREQUENCY_TABLE,"w");
if(f) {
fwrite(&theNewFrequencyTable,sizeof(theNewFrequencyTable),1,f);
fclose(f);
} else throw "Debug version: Unable to create frequency table (access problems?)";
}
#endif // MAKE_FREQUENCY_TABLE

#endif
