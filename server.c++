// (setq fill-column (- (window-width) 1))

// Compile with -DData_File_Pathname=\"whatever\"
// also maybe -DSYSLOG or -DNOLOG
// and maybe -DDEBUG_VERSION
// and maybe -DPrint_All_Incoming_Data

#define HTTP_VERSION "1.0"
// #define HTTP_VERSION "1.1"
// Testing HTTP/1.1 (for pipelining)
// Might not have large enough o/p buffers?  (Don't want to block on write() or fail it - myWrite retries)
// http://ssb22.joh.cam.ac.uk/cgi-bin/access?Ac=@&Aelt=Japanese&Au=http://ssb22.joh.cam.ac.uk/~ssb22/test/jis.html

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

// Some compilers need sys/time.h for FD_ stuff
#ifndef FD_SETSIZE
#include <sys/time.h>
#endif

// POSIX 1.1 has O_NONBLOCK; BSD has O_NDELAY
#ifndef O_NONBLOCK
#ifdef O_NDELAY
#define O_NONBLOCK O_NDELAY
#else
#error O_NONBLOCK not defined (O_NDELAY not defined either)
#endif
#endif

#ifdef DEBUG_VERSION
#define stderr stdout
#define DebugLog(x,y) printf("Debugger: " x "\n",y)
#else
#define DebugLog(x,y) ((void)0)
#endif

#ifdef SYSLOG
#include <syslog.h>
#define LogError(x) syslog(LOG_ERR,x)
#define LogError2(x,y,z) syslog(LOG_ERR,x,y,z)
#define LogWarning(x,y) syslog(LOG_WARNING,x,y)
#define LogInfo(x,y) syslog(LOG_INFO,x,y)
#else
#ifdef NOLOG
#define LogError(x) 0
#define LogError2(x,y,z) 0
#define LogWarning(x,y) 0
#define LogInfo(x,y) 0
#else
#define LogError(x) fputs(x "\n",stderr)
#define LogError2(x,y,z) fprintf(stderr,x "\n",y,z)
#define LogWarning(x,y) fprintf(stderr,x "\n",y)
#define LogInfo(x,y) fprintf(stderr,x "\n",y)
#endif
#endif

enum Constants {
#ifdef DEBUG_VERSION
  DefaultPort=7081,
#else
  DefaultPort=7080,
#endif
  BacklogSize=5, // Max number of connections waiting for
  // accept() (on some systems, max outstanding SYNs).
  // Some BSD-derived systems limit it to 5.
  CloseTimeout=10, // seconds (before giving up on getting
  // an ACK for the FIN packet (which may contain data))
  MaxConnections=768, // Must not exceed FD_SETSIZE, which
  // is normally about 1024
  // (Note: Some web browsers, eg. Netscape, make about 4
  // separate TCP connections to this thing)
  MaxSingleIP=10, // Maximum connections allowed from any
  // single IP address.  If this is reached and another
  // connection arrives from the same IP, then the oldest
  // connection from that IP is dropped.
  MAXMSG=2048, // Must not exceed SSIZE_MAX (usually
 // INT_MAX).  Some browsers send a rediculous number of
 // headers with their requests, so it's worth making this
 // about 2K (smaller doesn't matter but the read would have
 // to be fragmented).  Also make sure this is big enough to
 // hold the response messages (eg stats)
  MaxLineLen=25, // Each line of the request is truncated to
 // this length, if the request is sent in fragmented bytes
 // and we need to buffer it (need to limit the amount of
 // buffering we do, otherwise denial of service attacks
 // become easier).  25 should be plenty for this
 // application.
  StyleIDLen = 1,
  StyleNameLen = 20
};
#define ContentType "image/gif"
#define LastModified "Tue, 1 Jan 1980 00:00:01 GMT"
#define Expires "Wed, 1 Dec 2036 23:59:59 GMT"
#define NotFoundMsg "404 Not found"
#define IndexDocument "indx" // sentenel (value doesn't really matter as long as it's not a valid file and it's NameLen bytes long)
#define BannerType "text/html" // In case someone gets the
 // root document in a browser (unlikely, but if they do
 // then they'll be wanting to know what this thing is, so
 // let's tell them)
#ifndef UPDATE_DATE
#define UPDATE_DATE "Unknown"
#endif
#define Banner1 "<HTML><HEAD><TITLE>Image server</TITLE></HEAD><BODY><H1>Image server</H1>This is an image server for the <A HREF=\"http://www.flatline.org.uk/~silas/access.html\">access gateway</A>."
#define Banner2 "<HR>\n<SMALL>&copy; 2001 Silas S. Brown. Disclaimer: The author is not responsible for, and has no authority over, the websites that host this program. There is no warranty, either express or implied.</SMALL></BODY></HTML>"

#define NameLen 4
#define QuotedNameLen "4"
struct IndexRecord {
  char name[NameLen];
  int len;
  const void* dat;
};

// Check the sizes of some of the types (we need to know if
// any of them have exactly NameLen bytes, since this leads
// to a faster comparison) (although this is slightly
// excessive, since 20,000 characters can be binary searched
// in only 15 comparisons and there's going to be a bit of
// string processing anyway...)
#if CHAR_BIT<8
#error This program assumes a compiler that supports 8-bit chars
#endif
#define byte1(c) ((c)&0xFF)
#define byte2(c) byte1((c)>>8)
#define byte3up(c) ((c)>>16)
#define TwoByteFail(c,min2,min1) (!byte3up(c) && (byte2(c)<min2 || byte1(c)<min1))

#if TwoByteFail(UINT_MAX,0xFF,0xFF)
#error This program assumes a compiler that supports 16-bit ints
// (otherwise the preprocessor stuff below might not work)
// (NB we don't mind about 'short' here)
#endif

#if (NameLen == 2)
#if (USHRT_MAX == 0xFFFF)
#define NameCompareType short
#elseif (UINT_MAX == 0xFFFF)
#define NameCompareType int
#endif
#elseif (NameLen == 4)
#define word1(c) ((c)&0xFFFF)
#define word2(c) word1((c)>>16)
#define word3up(c) ((c)>>32)
#define TwoWordFail(c,min2,min1) (!word3up(c) && (word2(c)<min2 || word1(c)<min1))
#if TwoWordFail(ULONG_MAX,0xFFFF,0xFFFF)
// #error This program assumes a compiler that supports 32-bit longs
#else
#if (UINT_MAX == 0xFFFFFFFF)
#define NameCompareType int
#elseif (ULONG_MAX == 0xFFFFFFFF)
#define NameCompareType long
#endif
#endif // TwoWordFail
#endif // NameLen == 4

// Now NameCompareType, if defined, has NameLen bytes

int compare(const void* key,const void* data) {
#ifdef NameCompareType
  return(*((NameCompareType*)key)-*((NameCompareType*)(((const IndexRecord*)data)->name)));
#else
  return strncmp((const char*)key,((const IndexRecord*)data)->name,NameLen);
#endif
}

int strnicmp(const char* s1,const char* s2,int n) {
  // One problem with this code is it may return non-0 when both
  // strings are equal and their length is less than n.
  // The way it's used, this does not matter.
  int r;
  for(;n;--n) {
    if((r=toupper(*s2)-toupper(*s1))!=0) return(r);
    s1++; s2++;
  }
  return(0);
}

int readInt(FILE* f) {
  int i=(fgetc(f)&0xFF);
  i|=((fgetc(f)&0xFF)<<8);
  i|=((fgetc(f)&0xFF)<<16);
  i|=((fgetc(f)&0xFF)<<24);
  return i;
}
int readBytes(void* dat,size_t n,FILE* f) {
  if(fread(dat,1,n,f)!=n) {
    LogError("Corrupt data file (truncated?)");
    return 1;
  } else return 0;
}

struct Style {
  IndexRecord* idx;
  int numElements;
  int counter;
  char id[StyleIDLen];
  char name[StyleNameLen];
}* theStyles=NULL; int theNoOfStyles=0;

/// theStats: Used for keeping some stats
struct {
  int connectionsReceived;
  int imagesServed;
  int err404;
  int droppedConnections;
  int acceptFailures;
  int greaterThanSetsize;
  int writeErrors;
  int indexCounter;
  char launchTime[26];
  time_t launchTimeT;
} theStats;

int loadData(FILE* f) {
  if(fgetc(f)!=1) {
    LogError("Incorrect data file version");
    return 1;
  }
  int lenOfData=readInt(f);
  char* dat=new char[lenOfData]; // Never deleted (OK in this application)
  if(readBytes(dat,lenOfData,f)) return 1;
  theNoOfStyles=readInt(f);
  theStyles=new Style[theNoOfStyles]; // Never deleted (OK in this application)
  for(int i=0; i<theNoOfStyles; i++) {
    if(readBytes(theStyles[i].id,StyleIDLen,f)
       || readBytes(theStyles[i].name,StyleNameLen,f))
      return 1;
    theStyles[i].numElements=readInt(f);
    theStyles[i].counter=0;
    theStyles[i].idx=new IndexRecord[theStyles[i].numElements];
    for(int j=0; j<theStyles[i].numElements; j++) {
      if(readBytes(theStyles[i].idx[j].name,NameLen,f)) return 1;
      int len=readInt(f);
      theStyles[i].idx[j].len=len;
      theStyles[i].idx[j].dat=dat;
      dat += len;
    }
  }
  return 0;
}

/// preferredStyle can point to an empty string, otherwise
//it is of length up to StyleIDLen and not necessarily null
//terminated
const void* getData(const char* url,const char* preferredStyle,int &len) {
  DebugLog("getData(%s)",url);
  const void* element=NULL;
  // Starting i at 1 because 0 is the default style
  for(int i=1; i<theNoOfStyles; i++) {
    if(!strncmp(preferredStyle,theStyles[i].id,StyleIDLen)) {
      element=bsearch(url,theStyles[i].idx,theStyles[i].numElements,sizeof(IndexRecord),compare);
      if(element) theStyles[i].counter++;
      break; // out of finding the style
    }
  }
  if(!element) {
    // Either they requested a style that doesn't exist, or
    // they didn't request a style, or the character they
    // wanted was not in the style that they requested.  In
    // any case, check to see if the character exists in the
    // default "catch-all" style.
    element=bsearch(url,theStyles[0].idx,theStyles[0].numElements,sizeof(IndexRecord),compare);
    if(element) theStyles[0].counter++;
  }
  DebugLog("getData returning",NULL);
  if(element) {
    theStats.imagesServed++;
    len=((const IndexRecord*)element)->len;
    return((const IndexRecord*)element)->dat;
  } else return NULL;
}

int myWrite(int sock,const void* data,size_t len) {
  DebugLog("Calling write, size %d",len);
  int r=write(sock,data,len);
  DebugLog("write returned %d",r);
  if(r<0) {
    DebugLog("errno=%d",errno);
    if(errno!=EPIPE) {
      // (We won't count EPIPE errors, because they're
      // just caused by clients pulling out)
      theStats.writeErrors++;
      LogError("send() error");
    }
    return r;
  } else return 0;
  // NEEDATTENTION Some of these "errors" need retrying later (is this applicable to this application?)
  /*
When  the message does not fit into the send buffer of the socket, send normally blocks, unless the socket  has  been placed  in non-blocking I/O mode.  In non-blocking mode it would return EAGAIN in this case.  The select(2) call  may be  used  to  determine  when  it is possible to send more data.

  //EWOULDBLOCK or EAGAIN might be defined (maybe not both)
EAGAIN or EWOULDBLOCK or ENOBUFS - operation would have blocked; try again later
(EINTR - signal occurred; ENOMEM - no memory)
EPIPE - other side has closed down
EMSGSIZE - messages sent atomically and this was too big
   */
}
inline int myWrite(int sock,const char* data) { return myWrite(sock,data,strlen(data)); }

/// PartialRequest: Used for keeping state information for a
//partially-completed request (in case the packets are
//fragmented to the extent that we need to put the pieces
//back together while also servicing other requests)
class PartialRequest {
public:
  PartialRequest() {};
  ~PartialRequest() {};
  void init() {
    *requestedURL=0; *preferredStyle=0;
    *currentLine=0;
    theUseKeepAlive= /*1*/ 0; // 2000-11-24: Only use Keep-Alive if the browser explicitly requests it (otherwise could break some browsers)
    theUseHeaders=1; theFinished=0;
  }
  void handleIncomingData(const char* data) {
#ifdef Print_All_Incoming_Data
    puts(data);
#endif
    DebugLog("In handleIncomingData, data=%s",data);
    const char* newLine;
    do {
      newLine=strchr(data,'\n');
      int currentLinePtr=strlen(currentLine);
      if(newLine) {
        // Might need to append to currentLine
        int lenToCopy=newLine-data,maxLenToCopy=MaxLineLen-currentLinePtr;
        if(lenToCopy>maxLenToCopy) lenToCopy=maxLenToCopy;
        if(lenToCopy>0) strncpy(currentLine+currentLinePtr,data,lenToCopy);
        // Need to look at currentLine
        if(!*currentLine) theFinished=1; // Blank line
        else if(!strnicmp(currentLine,"Connection: Keep-Alive",22)) theUseKeepAlive=1;
        else if(!strnicmp(currentLine,"Connection: Close",17) || strstr(currentLine,"Mozilla/2")) theUseKeepAlive=0;
        // There are "known problems" with Mozilla/2
        // keepalive (there are also problems with
        // MSIE4.0b2, but only where redirects are involved)
        else if(currentLine[0]=='G' &&
                currentLine[1]=='E' &&
                currentLine[2]=='T' &&
                currentLine[3]==' ' &&
                currentLine[4]=='/') {
          const char* base=currentLine+5;
          const char* c=strchr(base,' ');
          const char* nextSlash=strchr(base,'/');
          if(nextSlash && (nextSlash<c || !c)) {
            // The URL includes a "directory"
            // (preferred font)
            int numToCopy=StyleIDLen;
            if(nextSlash-base<numToCopy) numToCopy=nextSlash-base;
            strncpy(preferredStyle,base,numToCopy);
            base=nextSlash+1;
          } else *preferredStyle=0;
          int numToCopy=NameLen;
          if(c && c-base<numToCopy) numToCopy=c-base;
          strncpy(requestedURL,base,numToCopy);
          if(!hasRequestedURL()) strncpy(requestedURL,IndexDocument,NameLen); // (need this as otherwise they'll get the connection closed)
          if(!c) {
            // It's possible that they're speaking a version
            // of HTTP less than 1.0 (which means the
            // request has now finished); it's also possible
            // that they requested a long URL (which we
            // don't have) and we've truncated it.  The
            // latter is not really a problem - they've
            // probably come to this server by mistake, so
            // they probably won't mind our not supporting
            // keep-alive in this case.
            theUseKeepAlive=0; theUseHeaders=0;
            theFinished=1;
          }
        }
        // There might be more lines in the data
        data=newLine+1;
        *currentLine=0;
      } else {
        // Might need to append to currentLine
        if(currentLinePtr<MaxLineLen) strncpy(currentLine+currentLinePtr,data,MaxLineLen-currentLinePtr);
      }
    } while(newLine);
    DebugLog("Finished handleIncomingData","");
  }
  int hasFinished() const { return theFinished; }
  int useKeepAlive() const { return theUseKeepAlive; }
  int useHeaders() const { return theUseHeaders; }
  int hasRequestedURL() const { return *requestedURL; }
  const char* getRequestedURLNoNull() const { return requestedURL; }
  // int hasPreferredFont() const { return *preferredStyle; }
  const char* getPreferredFontNoNull() const { return preferredStyle; }
private:
  char requestedURL[NameLen]; // don't need terminator here
  // + make sure requestedURL is first in the structure
  // (more likely to be byte-aligned; used in the binary
  // search)
  int theUseKeepAlive,theUseHeaders;
  int theFinished;
  char preferredStyle[StyleIDLen]; // don't need terminator here
  char currentLine[MaxLineLen+1];
};

/// LeastRecentlyUsed: Used to track the open (keep-alive)
//connections and say which is the least recently used (in
//case the server needs to close one).  Also (now) used to
//track the PartialRequest objects (as an array, since we
//don't want the overhead of new/delete all the time)
class LeastRecentlyUsed {
public:
  LeastRecentlyUsed() { fifoPtr=0; }
  ~LeastRecentlyUsed() {};
  int numConnections() const { return fifoPtr; }
  void used(int sock) {
    if(fifo[0]!=sock || !fifoPtr) {
      for(int i=1; i<fifoPtr; i++) {
        if(fifo[i]==sock) {
          // Swap it into 0
          memmove(fifo+1,fifo,sizeof(int)*i);
          fifo[0]=sock; return;
        }
      }
      // Add it
      if(fifoPtr<MaxConnections) {
        memmove(fifo+1,fifo,sizeof(int)*fifoPtr);
        fifo[0]=sock;
        requestIndex[fifoPtr]=sock;
        requests[fifoPtr].init();
        fifoPtr++;
      } else LogError("Internal error!  fifo overflow");
    } // else already at the top
  }
  void close(int sock) {
    ::close(sock);
    int found=0;
    for(int i=0; i<fifoPtr; i++) {
      if(fifo[i]==sock) {
        memmove(fifo+i,fifo+i+1,sizeof(int)*(fifoPtr-i));
        found=1;
      }
      if(requestIndex[i]==sock) {
        memmove(requestIndex+i,requestIndex+i+1,sizeof(PartialRequest)*(fifoPtr-i));
        // No need to say found=1 again
      }
    }
    if(found) fifoPtr--;
  }
  int popLRU() {
    if(fifoPtr) return removeFromRequests(fifo[--fifoPtr]);
    // (OK to call removeFromRequests even though fifoPtr
    // may or may not have been decremented at that time; if
    // the PartialRequest is at the last element then it'll
    // be dropped anyway)
    else { LogError("Internal error!  fifo underflow"); return -1; }
  }
  int checkIpAddress(int newsock) {
    // Check how many existing sockets have the same IP
    // address as newsock.  If this >= MaxSingleIP,
    // return the least recently used, else return -1.
    // NEEDATTENTION This is inefficient - getpeerbyname()
    // copies memory all the time.
    int count=0,toReturn=-1;
    struct sockaddr_in thisName,newName;
    size_t size=sizeof(newName);
    if(!getpeername(newsock,(struct sockaddr*)(&newName),(socklen_t*)&size)) {
      for(int i=0; i<fifoPtr; i++) {
        size=sizeof(thisName);
        if(!getpeername(fifo[i],(struct sockaddr*)(&thisName),(socklen_t*)&size)) {
          if(thisName.sin_addr.s_addr==newName.sin_addr.s_addr) {
            count++;
            toReturn=fifo[i];
          }
        } // else error on i's getpeername
      }
    } // else error on newsock's getpeername
    if(count>=MaxSingleIP) return toReturn;
    else return -1;
  }
  /// getPartialRequestPtr: Note that it might not remain
  //valid after operations on the LeastRecentlyUsed
  PartialRequest* getPartialRequestPtr(int sock) {
    for(int i=0; i<fifoPtr; i++) {
      if(requestIndex[i]==sock) return requests+i;
    }
    return NULL;
  }
private:
  int fifo[MaxConnections];
  int fifoPtr;
  int requestIndex[MaxConnections];
  PartialRequest requests[MaxConnections];
  int removeFromRequests(int sock) {
    for(int i=0; i<fifoPtr; i++) {
      if(requestIndex[i]==sock) {
        memmove(requestIndex+i,requestIndex+i+1,sizeof(PartialRequest)*(fifoPtr-i));
        // Don't need to decrement fifoPtr, but DO need to
        // go right up to fifoPtr (not fifoPtr-1) - see
        // comments in popLRU() above
      }
    }
    return sock;
  }
} theLRU;

/// Some code for the statistics
inline int niceDivision(int num,int denom) {
  // Integer division rounding to nearest instead of down
  if(denom) return ((num+(denom>>1))/denom);
  else return -1; // Don't throw any kind of signal
  // (this is only the stats page and we don't want anybody
  // to bring down the server by asking at the wrong moment)
}

int niceMultDivision(int mult,int num,int denom) {
  // Calculate mult*num/denom (ie. num/(denom/mult))
  // without overflow and with nice rounding
  int i=INT_MAX/mult;
  while(num>i) {
    // would overflow; drop the least significant bit
    num >>= 1;
    denom >>= 1;
  }
  return niceDivision(mult*num,denom);
}

int writeStats(int sock) {
  time_t t=time(NULL);
  struct passwd* p=getpwuid(getuid());
  char buffer[MAXMSG];
  sprintf(buffer,"<UL><LI>Program version: " UPDATE_DATE"\n"
"<UL><LI>Server time at launch: %s\n"
"<LI>Server time now: %s\n"
"<LI>Launched by %s (%s)</UL>\n"
"<LI>%d images served (%d per hour)<UL>",
          theStats.launchTime,
          ctime(&t),
          p->pw_name, p->pw_gecos,
          theStats.imagesServed,
          niceMultDivision(3600,theStats.imagesServed,t-theStats.launchTimeT));
  if(myWrite(sock,buffer)) return -1;
  for(int i=0; i<theNoOfStyles; i++) {
    sprintf(buffer,"<LI>%d requested %s style",theStyles[i].counter,theStyles[i].name);
    if(myWrite(sock,buffer)) return -1;
  }
  sprintf(buffer,"</UL><LI>%d connections received (average images served per connection: %d)\n"
"<UL><LI>%d connections currently open\n"
"<LI>%d idle connections dropped\n"
"<LI>%d connections dropped due to `send' errors\n"
"<LI>%d `accept' failures\n"
"<LI>%d FD_SETSIZE errors"
"<LI>Last error: %s"
"<LI>%d hits for this page</UL>\n"
"<LI>%d `not found' errors</UL>",
          theStats.connectionsReceived,
          niceDivision(theStats.imagesServed,theStats.connectionsReceived),
          theLRU.numConnections(),
          theStats.droppedConnections,
          theStats.writeErrors,
          theStats.acceptFailures,
          theStats.greaterThanSetsize,
          strerror(errno),
          theStats.indexCounter,
          theStats.err404);
  if(myWrite(sock,buffer)) return -1;
  return 0;
}

int handleRequest(int sock) {
  char buffer[MAXMSG+1];
  DebugLog("Calling read(%d)",sock);
  int nbytes=read(sock,buffer,MAXMSG);
  DebugLog("read returned %d",nbytes);
  if(nbytes<0) {
#ifndef NOLOG
#ifdef ECONNRESET
    if(errno!=ECONNRESET)
      // ("Connection reset by peer" is OK)
      // (2000-10-26: But NB we also get "No route to host", "Resource temporarily unavailable" and "Connection timed out")
#endif
      perror("read");
#endif
    return -1;
  } else if (nbytes == 0) {
    // LogError("Got EOF");
    return -1;
  } else {
    // Remove all \r's, and add a terminating null:
    int ofs=0;
    for(int i=0; i<nbytes; i++) {
      do {
        if(ofs) buffer[i]=buffer[i+ofs];
        if(buffer[i]=='\r') { ofs++; nbytes--; }
      } while(i<nbytes && buffer[i]=='\r');
    }
    buffer[nbytes]=0;
    // fputs(buffer,stderr);
    PartialRequest* pr=theLRU.getPartialRequestPtr(sock);
    if(!pr) { LogError("Internal error!  Null PartialRequest"); return -1; }
    pr->handleIncomingData(buffer);
    if(pr->hasFinished()) {
      int useKeepAlive=pr->useKeepAlive();
      if(pr->hasRequestedURL()) {
        int len; const void* data=getData(pr->getRequestedURLNoNull(),pr->getPreferredFontNoNull(),len);
        if(data) {
          if(pr->useHeaders()) {
            sprintf(buffer,"HTTP/" HTTP_VERSION
                    " 200 OK\nConnection: %s\n"
                    "Content-type: " ContentType "\n"
                    "Expires: " Expires "\n"
                    "Last-Modified: " LastModified "\n"
                    "Content-length: %d\n\n",useKeepAlive?"Keep-Alive":"Close",len);
            if(myWrite(sock,buffer)) return -1; // (For now, close on errors)
          }
          if(myWrite(sock,data,len)) return -1;
          // theStats.imagesServed++; // now done in getData itself
          //fsync(sock);
          pr->init();
          theLRU.used(sock); // Don't do it until actually returning a page (and note that this might invalidate 'pr')
        } else if(!strncmp(pr->getRequestedURLNoNull(),IndexDocument,NameLen)) {
          // The index document.  There is little point in
          // establishing a keep-alive connection for this
          // (so we don't need to worry about
          // content-length either)
          theStats.indexCounter++;
          if(pr->useHeaders()) {
            sprintf(buffer,"HTTP/1.0 200 OK\nContent-type: " BannerType "\n\n");
            if(myWrite(sock,buffer)) return -1;
          }
          // (For now, if any write errors, close immediately)
          if(myWrite(sock,Banner1)
             || writeStats(sock)
             || myWrite(sock,Banner2)
             ) return -1;
          //fsync(sock);
          // pr->init(); // No point - going to close
          return -1;
        } else {
          // A 404
          LogWarning("Error 404 on URL \"%." QuotedNameLen "s\"",pr->getRequestedURLNoNull()); // (must do before overwriting buffer)
          if(pr->useHeaders()) {
            sprintf(buffer,"HTTP/" HTTP_VERSION " 404 Not Found\nConnection: %s\nContent-type: text/plain\nContent-length: %d\n\n",useKeepAlive?"Keep-Alive":"Close",(int)strlen(NotFoundMsg));
            if(myWrite(sock,buffer)) return -1;
          }
          if(myWrite(sock,NotFoundMsg)) return -1;
          theStats.err404++;
          //fsync(sock);
          pr->init();
        }
        return(useKeepAlive?0:-1); // Close the connection
 // if not using keep-alive (in case they don't close it -
 // they shouldn't be using it again) (and also to signify a
 // definite "end of data" for browsers that can't
 // understand Content-Length)
      } else return -1; // If they didn't request a URL,
 // close the connection (should we log this?  Probably not)
    } // else not finished
    return 0;
  }
}

int main(int argc,char* argv[]) {
#ifdef SYSLOG
  // Open system log
  openlog(argv[0],/*LOG_PERROR*/0,LOG_USER);
#endif
  // Init the stats
  memset(&theStats,0,sizeof(theStats));
  theStats.launchTimeT=time(NULL);
  strcpy(theStats.launchTime,ctime(&theStats.launchTimeT));

  // Load data file
  FILE* fp=fopen(Data_File_Pathname,"rb");
  if(!fp) {
    LogError("Failed to open data file");
    exit(EXIT_FAILURE);
  }
  if(loadData(fp)) exit(EXIT_FAILURE); // (error already logged)
  fclose(fp);

  // Don't exit on SIGPIPE (if someone disconnects when
  // there is still data to write)
  signal(SIGPIPE,SIG_IGN);
  // Also ignore "alarm clock" signals (e.g. cron on monash)
  signal(SIGALRM,SIG_IGN);

  // Get the listening socket
  int portNumber=DefaultPort;
  if(argc>1) {
    portNumber=atoi(argv[1]);
    if(!portNumber) {
      LogWarning("Syntax: %s [port]",argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  int sock=socket(PF_INET,SOCK_STREAM,0);
  if (sock<0) {
    LogError("socket failure");
    perror("socket");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in name;
  name.sin_family=AF_INET;
  name.sin_port=htons(portNumber);
  name.sin_addr.s_addr=htonl(INADDR_ANY);

  // Set reuse addresses so doesn't have bind failures if another server recently died (2000-10-26: But japan2001 seems to have that problem anyway.  That could mean that do-japan2001-img is not a good idea to run.  Test other servers & make sure connecting? eg. enfour)
  // All gone into FIN_WAIT1
  // Send-Q  - "The count of bytes not acknoledged by the remote host." - all set to "1"
  // Took some 30mins for them all to clear
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&sock /* an integer containing something other than 0 */,sizeof(int));

  if(bind(sock,(struct sockaddr*)&name,sizeof(name))<0) {
    LogError("bind failure");
    perror ("bind");
    exit (EXIT_FAILURE);
  }     
  if(listen(sock,BacklogSize)<0) {
    LogError("listen failure");
    perror("listen");
    exit(EXIT_FAILURE);
  } else LogInfo("Waiting for connections on port %d",portNumber);

  // Initialise the set of active sockets
  fd_set active_fd_set,read_fd_set;
  FD_ZERO(&active_fd_set);
  FD_SET(sock,&active_fd_set);

  while (1) {
    // Block until input arrives on one or more active
    // sockets
    read_fd_set=active_fd_set;
    DebugLog("Calling select()","");
    if(select(FD_SETSIZE,&read_fd_set,NULL,NULL,NULL)<0) {
      // Invalid file descriptor (errno=EBADF), interrupted,
      // or invalid timeout
      perror("select");
      exit(EXIT_FAILURE);
    }
    DebugLog("select() returned","");
     
    // Service all the sockets with input pending
    for(int i=0; i<FD_SETSIZE; ++i) {
      if(FD_ISSET(i,&read_fd_set)) {
        if(i==sock) {
          // New connection
          DebugLog("Dealing with new connection (sock=%d)",
                   sock);
          theStats.connectionsReceived++;
          size_t size=sizeof(name);
          int newsock=accept(sock,(struct sockaddr*)&name,(socklen_t*)&size);
          if(newsock<0) {
            LogError("'accept' failure");
            DebugLog("errno=%d",errno);
            theStats.acceptFailures++;
#ifndef NOLOG
            perror("accept");
#endif
            // exit(EXIT_FAILURE); // No!
            // FD_CLR(sock,&active_fd_set); // so closes a few connections if exceeded max open files (shouldn't if system is correctly configured, but Monash did...)
            // No, don't do that - it relies on clients actually closing stuff.  Instead, do it here.
            if(theLRU.numConnections()) {
              int toClose=theLRU.popLRU();
              close(toClose);
              FD_CLR(toClose,&active_fd_set);
              theStats.droppedConnections++; // NEEDATTENTION Do we want a different statistic here?
            } else LogError("'accept' failure with no open connections!"); // (This may well be "exit" material)
            // Should try to accept() again next time round the loop
          } else {
            // accept() was successful
            LogInfo("%s",inet_ntoa(name.sin_addr));
            if(newsock>=FD_SETSIZE) {
              close(newsock);
              theStats.greaterThanSetsize++;
              LogError2("Ooops, socket file descriptor is %d but FD_SETSIZE is only %d; had to drop that connection",newsock,FD_SETSIZE);
            } else {
              // When we close this socket, we don't want
              // the call to block while data gets written
              struct linger l; l.l_onoff=0;
              l.l_linger=CloseTimeout;
              setsockopt(newsock,SOL_SOCKET,SO_LINGER,&l,sizeof(l));
              // When we write to this socket, we don't want
              // the call to block if the buffer is full
              fcntl(newsock,F_SETFL,O_NONBLOCK);
              // Add it to our set of active connections
              FD_SET(newsock,&active_fd_set);
              // Check the number of connections already
              // open from that IP address (if it's too many
              // then we may have to drop the least recently
              // used one)
              if(theLRU.numConnections()>=MaxSingleIP) {
                int toClose=theLRU.checkIpAddress(newsock);
                if(toClose>=0) {
                  theLRU.close(toClose);
                  FD_CLR(toClose,&active_fd_set);
                  theStats.droppedConnections++; // NEEDATTENTION Do we want a different statistic here?
                }
              }
              // Add it to theLRU (after checking)
              if(theLRU.numConnections()>=MaxConnections) {
                // We'll have to drop one
                // (Should we log this?  Probably not)
                int toClose=theLRU.popLRU();
                close(toClose);
                FD_CLR(toClose,&active_fd_set);
                //FD_SET(sock,&active_fd_set); // (see above FD_CLR)
                theStats.droppedConnections++;
              }
              theLRU.used(newsock); // Better (even though hasn't sent data yet), otherwise it won't be recorded by theLRU
            }
          }
          DebugLog("Finished dealing with new connection",
                   "");
        } else { // if i != sock,
          if(handleRequest(i)<0) {
            DebugLog("handleRequest(%d) returned <0",i);
            theLRU.close(i);
            FD_CLR(i,&active_fd_set);
            //FD_SET(sock,&active_fd_set); // See above FD_CLR
          }
        }
      }
    }
  }
}
