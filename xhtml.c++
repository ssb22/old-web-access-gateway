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
#include <string>
#include <list>
#include <stdio.h>
#include "strfunc.h"
#include "tagstuff.h"
#include "subhtml.h"
#include "ampersnd.h"

class TagCloser {
public:
TagCloser() {};
~TagCloser() {};

// How many tags would you need to close BEFORE doing
// this opening tag (+ add it to the stack),  or
// How many tags (from the stack) would you need to
// close INCLUDING this closing tag (0 if rubbish tag)
int getCloseCount(const char* tagName);

// If the following returns true, then close / pop
// immediately (e.g. <BR>, <IMG> etc)
bool needToCloseImmediately() const;

// Below is the only way to pop from the stack
string pop();

void close(FILE* out); // Writes </TAG>
void closeAll(FILE* out);

private:
list<string> theStack;
bool tagCanContain(const char* container,const char* contained) const;
};

int TagCloser::getCloseCount(const char* tagName) {
int numToClose=0;
if(tagName[0]=='/') {
// It's a closing tag
bool found=false;
for (list<string>::const_iterator i=theStack.begin();
i != theStack.end(); i++) {
numToClose++;
if(!stricmp((*i).c_str(),tagName+1)) { found=true; break; }
}
if(!found) {
// Trying to close a tag that wasn't opened
// (NEEDATTENTION Probably need to imitate the
// browser bugs a bit more closely here)
numToClose=0;
}
} else {
// Opening or solitary tag
// If the tag we're in can't take this as a subtag,
// lots of closing tags implied
for (list<string>::const_iterator i=theStack.begin();
i != theStack.end(); i++) {
if(tagCanContain((*i).c_str(),tagName)) break;
else numToClose++;
}
theStack.push_front(tagName);
// puts("Content-type: text/plain\n"); for (list<string>::const_iterator i=theStack.begin(); i != theStack.end(); i++) puts((*i).c_str()); puts(""); // Debugger
}
return numToClose;
}

// The following is alphabetical order of
// "tag / what it can contain"
// Tags that are standalone (i.e. don't have closing tags) end in "/!"
// Derived from
// http://www.w3.org/MarkUp/html-spec/L2index.html
static const char* tagGrammar[]={
"A/H1/H2/H3/H4/H5/H6/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"ADDRESS/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I/P",
"B/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"BASE/!",
"BLOCKQUOTE/H1/H2/H3/H4/H5/H6/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I/P/UL/OL/DIR/MENU/DL/PRE/XMP/LISTING/BLOCKQUOTE/FORM/ISINDEX/HR/ADDRESS",
"BODY/H1/H2/H3/H4/H5/H6/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I/P/UL/OL/DIR/MENU/DL/PRE/XMP/LISTING/BLOCKQUOTE/FORM/ISINDEX/HR/ADDRESS",
"BR/!",
"CITE/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"CODE/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"DD/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I/P/UL/OL/DIR/MENU/DL/PRE/XMP/LISTING/BLOCKQUOTE/FORM/ISINDEX",
"DIR/LI",
"DL/DT/DD",
"DT/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"EM/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"FORM/H1/H2/H3/H4/H5/H6/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I/P/UL/OL/DIR/MENU/DL/PRE/XMP/LISTING/BLOCKQUOTE/ISINDEX/HR/ADDRESS/INPUT/SELECT/TEXTAREA",
"H1/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"H2/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"H3/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"H4/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"H5/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"H6/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"HEAD/TITLE/ISINDEX/BASE/META/NEXTID/LINK",
"HR/!",
"HTML/HEAD/BODY/PLAINTEXT",
"I/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"IMG/!","INPUT/!","ISINDEX/!",
"KBD/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"LI/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I/P/UL/OL/DIR/MENU/DL/PRE/XMP/LISTING/BLOCKQUOTE/FORM/ISINDEX",
"LINK/!",
"LISTING", // MAY CONTAIN DATA!
"MENU/LI",
"META/!","NEXTID/!",
"OL/LI",
"OPTION", // MAY CONTAIN DATA
"P/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"PLAINTEXT", // MAY CONTAIN DATA
"PRE/A/HR/BR/TT/B/I/EM/STRONG/CODE/SAMP/KBD/VAR/CITE",
"SAMP/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"SELECT/OPTION",
"STRONG/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"TEXTAREA", // MAY CONTAIN DATA
"TITLE", // MAY CONTAIN DATA
"TT/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"UL/LI",
"VAR/A/IMG/BR/EM/STRONG/CODE/SAMP/KBD/VAR/CITE/TT/B/I",
"XMP", // MAY CONTAIN DATA
0};

bool TagCloser::tagCanContain(const char* container,const char* contained) const {
int len=strlen(container);
for(int i=0; tagGrammar[i]; i++) {
int r=strnicmp(tagGrammar[i],container,len);
if(!r) {
// Found one
const char* c=strchr(tagGrammar[i],'/');
len=strlen(contained);
while(c) {
c++; if(!(*c)) throw "tagGrammar trailing slash";
if(!strnicmp(c,contained,len)) return true;
// (doesn't matter if 'contained' is '!' -
// that would be an unknown tag, in which case
// we want to return true anyway)
c=strchr(c,'/');
}
// If got here, either 'contained' can't be
// contained by 'container', or 'contained' is an
// unknown tag.
bool found=false;
for(int j=0; tagGrammar[j]; j++) {
if(!strnicmp(tagGrammar[i],contained,len)) {
found=true; break;
}
}
// Assume unknown tags can be contained by anything
return !found;
}
// else if(r>0) break; // Gone past it
// (No - the / might be earlier than 'len')
}
// Unknown container - assume it can contain anything
return true;
}

bool TagCloser::needToCloseImmediately() const {
const char* container=theStack.front().c_str();
int len=strlen(container);
for(int i=0; tagGrammar[i]; i++) {
if(!strnicmp(tagGrammar[i],container,len)) {
return (tagGrammar[i][strlen(tagGrammar[i])-1]=='!');
}
}
return false;
}

string TagCloser::pop() {
string retVal=theStack.front();
theStack.pop_front();
// for (list<string>::const_iterator i=theStack.begin(); i != theStack.end(); i++) puts((*i).c_str()); puts(""); // Debugger
return retVal;
}

void TagCloser::close(FILE* out) {
string closedTag=pop();
fputs("</",out); fputs(closedTag.c_str(),out);
fputc('>',out);
}

void TagCloser::closeAll(FILE* out) {
while(!theStack.empty()) close(out);
}

void html2xhtml(FILE* in,FILE* out) {
InString textBetweenTags, temp;
InString htmlLineInput; int lineInputPtr=0;
TagCloser theCloser;
int ch=0;
fputs("<?xml version=\"1.0\"?>",out);
while(ch!=EOF) {
ch=subHtmlFgetc(in,htmlLineInput,lineInputPtr);
if(ch=='<' || ch==EOF) {
ampersandFilter(textBetweenTags.getString(),temp,1);
fputs(temp.getString(),out);
textBetweenTags.clear(); temp.clear();
if(ch=='<') {
HTMLTag theTag(in,htmlLineInput,lineInputPtr);
if(!theTag.wasOnlyAComment()) {
const char* tagName=theTag.getName();
if(*tagName) {
if(tagName[0]!='!') { // Don't write out !DOCTYPE etc
int toClose=theCloser.getCloseCount(tagName);
for(int i=0; i<toClose; i++) theCloser.close(out);
if(tagName[0]!='/') {
theTag.outputTo(out);
if(theCloser.needToCloseImmediately() && !theTag.getAttribute("/"))
theCloser.close(out);
}
// (If it is '/' then it has already been
// dealt with above)
// 2002-03-22: If already XHTML, tell the
// closer that it's closed:
if(theTag.getAttribute("/")) theCloser.pop();
} // (end of if it's not DOCTYPE)
} else theTag.outputTo(out); // &lt; hack
// (see 2000-10-25 hack in tagstuff.c++)
}
}
} else textBetweenTags.addCharacter(ch);
} // End of while !eof
theCloser.closeAll(out);
subHtmlDone(); // so only converts once (otherwise would
// need to re-detect the character encoding)
}

void html2xhtml(const char* in,const char* out) {
FILE *i=fopen(in,"r"), *o=fopen(out,"w");
if(!i) throw "html2xhtml: Failed to open input file";
if(!o) throw "html2xhtml: Failed to open output file";
html2xhtml(i,o);
fclose(o); fclose(i);
}
