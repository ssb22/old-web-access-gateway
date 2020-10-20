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
#ifdef USE_GNU_SOCKETS
#include "mylynx.h"
#include "instring.h"
#include "strfunc.h"
#include "debug.h"
#include "proxy.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFERLEN 1024

void lowLevelGetPage(const char* hostname,int port,const char* request,const char* outputFile) {
checkPoint();

// Create socket
int sock=socket(PF_INET,SOCK_STREAM,0);
if(sock<0) throw "lowLevelGetPage: Failed to create socket";
struct sockaddr_in servername;
servername.sin_family=AF_INET;
servername.sin_port=htons(port);
// Look up DNS:
// (Doesn't matter if it's an IP address)
struct hostent* hostinfo=gethostbyname(hostname);
if(!hostinfo) throw "lowLevelGetPage: Failed to resolve DNS";
servername.sin_addr=*(struct in_addr*)hostinfo->h_addr;
// Connect:
if(connect(sock,(struct sockaddr*)&servername,sizeof(servername))<0) throw "lowLevelGetPage: Failed to connect";
// Send the request:
if(write(sock,request,strlen(request))<0)
throw "lowLevelGetPage: Failed to write to server (connection established OK)";
// Create output file while the web server is responding:
int fdw=open(outputFile,O_WRONLY|O_CREAT|O_TRUNC,00644);
if(fdw<0) throw "lowLevelGetPage: Failed to create output file";
// Get the response:
char buffer[BUFFERLEN]; int nbytes;
while(1) {
nbytes=read(sock,buffer,BUFFERLEN);
if(nbytes<0) throw "lowLevelGetPage: Failed to read from server (request sent OK)";
else if(!nbytes) break;
else if(write(fdw,buffer,nbytes)<0) throw "lowLevelGetPage: Failed to write to output file";
}
close(fdw); close(sock);
}

void socketWebGet(const char* url,const char* userAgent,const char* outputFile,const char* requestBody) {
checkPoint();
const char *hostname=NULL,*slash=NULL;
int port=80; // default port for web servers
if(proxy()) {
hostname=proxy();
port=proxyPort();
slash=url;
} else {
// Omit [http:][//]
if(!strncmp(url,"http:",5)) url+=5;
if(!strncmp(url,"//",2)) url+=2;
// Parse URL into host, port and path
// At this point, we can assume that the URL ends with a
// slash if it's just a domain
const char *colon=strchr(url,':'),
*at=strchr(url,'@');
slash=strchr(url,'/');
// NEEDATTENTION IPv6 addresses have colons in them - that's going to trip up this code...
if(at && at<slash) throw "socketWebGet does not support user:pass@host:port - use callLynx";
if(colon && colon<slash) {
// URL specifies a port number
port=atoi(colon+1);
hostname=strnew(url,colon-url);
} else hostname=strnew(url,slash-url);
} // proxy
// Now ready to put the request together
int hasBody=(requestBody && *requestBody);
InString request;
request.addString(hasBody?"POST ":"GET ");
request.addString(slash);
request.addString(" HTTP/1.0");
if(!proxy()) {
request.addString(CRLF "Host: ");
// (Host: necessary for virtual servers to work)
request.addString(hostname);
// 2002-03-07: It seems some servers don't like having
// ":80" after the "Host:" header, and it breaks the
// HTTP/1.0 spec.  I'm not sure why this code was here.
// request.addCharacter(':'); request.addString(port);
}
request.addString(CRLF "Connection: Close" CRLF "Accept: */*" CRLF "Accept-Charset: *");

request.addString(CRLF "User-Agent: ");
request.addString(userAgent);
#ifdef PROXY_AUTH_HEADER
request.addString(CRLF PROXY_AUTH_HEADER);
#endif
if(hasBody) {
request.addString(CRLF "Content-type: application/x-www-form-urlencoded" CRLF "Content-length: ");
char buf[10]; sprintf(buf,"%d",strlen(requestBody));
request.addString(buf);
request.addString(CRLF CRLF);
request.addString(requestBody);
} else request.addString(CRLF CRLF);
lowLevelGetPage(hostname,port,request.getString(),outputFile);
if(!proxy()) delete[] hostname;
}

#endif
