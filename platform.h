// This is a generic PLATFORM.H.  You need to go through
// it and edit it accordingly.

// Any line beginning // is a comment and is ignored.
// Some #defines are like this.  If you do not want them
// to be ignored, then please remove the //.

// Please leave the following two lines untouched:
#ifndef PLATFORM_H
#define PLATFORM_H

// Please delete the following line:
#error Please edit platform.h

// If you run gcc version 2.7 (ie. you have to specify "-fhandle-exceptions"),
// then please uncomment the following line:
// #define BROKEN_EXCEPTION_HANDLING

// If you define LOGIN_TOUCH, then that file will be "touched" (actually 
// overwritten with a 0-length file) each time somebody uses the gateway.  This may 
// be useful if you would like to monitor the last "login", since you can simply 
// check the time stamp of this file.
// #define LOGIN_TOUCH "loggedin.dat"

// If you really want a crude log of everything, define this,
// but note that the gateway will need write permissions.  Also,
// web servers themselves usually log all transfers, so this
// will usually be redundant.
// #define USAGE_LOGFILE "/tmp/access.log"

// Some old compilers may need you to define NEED_TERMINATE_FUNCTION, 
// but this is unlikely.
// #define NEED_TERMINATE_FUNCTION

// You will need to make sure that the value of LYNX_COMMAND
// is correct, especially on a Windows NT system.
// If lynx is in the PATH (ie. you can get Lynx by typing "lynx")
// then you can leave it as-is, but note that it may not be in
// the PATH when the web server runs it, so it might be better
// to give the full path (eg. /usr/local/bin/lynx) - type
// "which lynx" at the prompt to get this.
// Important: You MUST use the full path here if you
// set USE_POSIX_FORK below.
#define LYNX_COMMAND "lynx"

// If your version of Lynx is lynx-ssl, then you may uncomment (remove the '//'
// from) the following, to allow the gateway to get https:// URLs ("secure
// documents") with it.  The gateway itself contains no cryptographic code.
// Note that the connection is no longer secure (the user is warned about this)
// #define LYNX_SSL

// GZIP_COMMAND is similar to LYNX_COMMAND.  It must be defined, but it is only
// used if Lynx insists on asking for compressed pages (which it
// usually does on Unix).  Again, use the full path if using USE_POSIX_FORK.
#define GZIP_COMMAND "gzip"

// If you have wget on your system, you can uncomment the following #define to
// allow the gateway to use it for FTP URLs.
// wget is faster to load than lynx, but it is only used for FTP URLs
// because it follows HTTP re-directions without reporting them
// (and hence relative links wouldn't always work if it were used
// in HTTP URLs).
// If you use wget, don't forget to test that your installation can
// get FTP URLs as well as HTTP URLs, so that the use of both programs
// is tested.
// WGET_COMMAND needs paths if LYNX_COMMAND needs paths.
// #define WGET_COMMAND "wget"

// IMPORTANT NOTE: Some versions of Lynx have a bug whereby,
// if in the configuration file lynx.cfg (eg. /etc/lynx.cfg)
// there is a START_FILE line with an unqualified URL (eg.
// myhost.my.domain rather than http://myhost.my.domain/),
// Lynx prints a bogus message at the start of *all* output.
// This can cause problems with pages that are transferred over
// FTP (the bogus line gets into the output that the user sees).
// Please check your lynx.cfg file or do lynx --help | more
// in order to see if this bogus line (like
// "Looking up 'myhost.my.domain' first") is output.

// Note: Please make sure that your version of Lynx is up-to-date.
// There are problems with version 2.6.

// Newer versions of Lynx (2.8+) let you specify the USER_AGENT,
// but older ones do not.  If you define the following,
// it will not work if you have an older version of Lynx.
// You can check this by typing lynx -version
// If you're not sure, try it and see if it works.

// NOTE: If you leave this commented out, your gateway will
// have no support for remote session cookies or referral
// tracking.  This is because the only way to get Lynx to
// send an arbitrary HTTP header (such as a cookie) while
// doing a mime_header request is to append the header to
// the user agent string (after a newline) (well,
// USE_GNU_SOCKETS can do it, but that doesn't work with SSL
// - the options-processing code won't compile in
// cookie-handling stuff unless LYNX_USER_AGENT is defined).

// NOTE ALSO that some distributions of Lynx have actually
// disabled this feature as a "security bug" (see Debian's
// DSA-210-1) and on these versions cookies over SSL will be
// impaired.

// #define LYNX_USER_AGENT

// Lynx version 2.8.3rel.1 under Unix has a bug and refuses
// to run unless the TERM environment variable is set to
// something it knows how to deal with, even if we're only
// using it to dump the source and header of a web page to
// stdout.  However, putenv and setenv are non-standard C
// functions (they only conform to Unix standards).  The
// following line includes the putenv code; it should be
// harmless on systems that don't have the problem, but you
// might need to remove it if you get errors about putenv or
// setenv when you try to compile.
#define LYNX_NEEDS_TERM

// COMMAND_SPECIAL_CHAR_PREFIX should be '\\' under
// Unix (two backslashes because of the C preprocessor)
// and '^' under NT
#define COMMAND_SPECIAL_CHAR_PREFIX '\\'

// COMMAND_CR_STRING should be "\"\n\"" under Unix (default)
// or "^\n\n" under NT.  You only need to uncomment the
// next line if you are running on NT.
// You MUST uncomment the next line if you are running on NT
// otherwise people may be able to put arbitrary system
// commands in cookies.
// #define COMMAND_CR_STRING "^\n\n"

// Only define COMPILER_USES_LSB_MSB_INTS if you know
// it to be true (it enables an optimisation).  If you don't
// know then it will be all right to leave it undefined.
// #define COMPILER_USES_LSB_MSB_INTS

// Defining USE_POSIX_FORK gives more speed, but is only
// supported on Posix-compliant Unix systems.  If you're
// not sure, try it and see if it compiles.
// It is recommended that you use USE_POSIX_FORK if you are
// using cgiwrap, otherwise problems can occur when there
// are errors fetching pages.
// USE_POSIX_FORK also enables Posix signal handling, so the program
// should clean up if it gets a signal, or if it runs for too long (in case
// the web server doesn't kill it).  You may also define MAX_RUNTIME_SECONDS
// if you want to specify your own timeout.
#define USE_POSIX_FORK

// Defining USE_GNU_SOCKETS gives more speed for the
// majority of requests, on GNU Unix systems.  If you're
// not sure, try it and see if it compiles.
// Note: On some Sun systems you have to add the following
// options to the g++ command line: -lsocket -lnsl
// USE_GNU_SOCKETS causes internal code to be used for most
// requests, instead of calling an external program such as
// Lynx.  This is much more efficient.  However, there are
// some requests (such as those requiring authentication)
// that cannot be handled by the internal code, so Lynx is
// still required for these cases.
#define USE_GNU_SOCKETS

// You can get the gateway to work through a proxy by
// uncommenting (and modifying) the following.  If you
// uncomment PROXY_FTP then the proxy will also be used
// for FTP transfers (if USE_GNU_SOCKETS is defined then
// this will prevent the need to use lynx or wget for
// these), and you don't need to install wget.
// #define PROXY "some.proxy.server"
// #define PROXY_PORT 8080   // default
// #define PROXY_FTP

// Set the following if your proxy needs authentication
// #define PROXY_AUTH_ID "whatever"
// #define PROXY_AUTH_PASSWORD "whatever"

// If you want to USE_GNU_SOCKETS then you can still have
// proxy authentication, but you need to define
// PROXY_AUTH_BASE64 (below).  Set it to the MIME base64
// encoded version of the string ID:PASSWORD.  (Sorry, I
// haven't put a base64 encoder into the access gateway code
// itself.)  If you leave this out, you can still have proxy
// authentication but it won't USE_GNU_SOCKETS.

// #define PROXY_AUTH_BASE64 "whatever"

// It doesn't really matter whether the following is defined,
// but if your system supports it and you don't trust my
// programming, it calls more defensive library routines.
// #define _GNU_SOURCE

// On some 95/NT systems you may need to define the
// following (but you don't have to worry if you're
// using BRIEF_RESPONSE):
// #define WEB_SERVER_ADDS_CHARACTER_13

// The following is for special use and can probably be left alone
// #define LYNX_NEEDS_CLOSE_QUOTE

// TMP_PREFIX may or may not be defined, but if it is
// then the temporary files are placed there instead
// of the current directory.  It is a good idea to define
// it in Unix but not in NT (or if you do then note that
// backslashes must be doubled).
#define TMP_PREFIX "/tmp/"

// If you are on a Unix system and you are NOT using USE_POSIX_FORK, and
// your Lynx setup uses metamail to get the MIME types (some do and some
// don't), and the CGI script does not have write access to the current
// directory, then you need to define CD_TO_TMP_PREFIX and COMMAND_SEPARATOR
// below for Lynx to work.  Otherwise, they are irrelevant.
#define CD_TO_TMP_PREFIX
#define COMMAND_SEPARATOR ';'

// PRESET_OPTIONS lets you set some commonly-used options,
// helping to shrink long URLs.  If you would like to do
// this, use the following as a guide to the format.  It is
// harmless to leave it as it is.
// Note that checkboxes have a value of 0 instead of a
// quoted string, because different web browsers use
// different values for turning on checkboxes.
// At the end of each preset, have two more 0s as shown.
// At the end of all the presets, there is a further 0 as
// shown.
// You can set up to six presets.
// Note that the URL may still be long after you submit the
// form; to see it get shorter, follow a link.
// If you use PRESET_OPTIONS, remove the '//' comments
// from the start of the lines below.
// #define PRESET_OPTIONS { \
// "Aecb","000000","Aect","FFFF00","Aecl","00FF00","Aeca","FF0000","Aecv","0000FF","Aefs","5","Aesu",0,"Aez",0,"Aen",0,  0,0, \
// "Aelt","Japanese","Aesi",0,"Aesu",0,"Aeso",0,"Ael",0,  0,0,\
// 0}

// ********* Start of language encodings stuff
// (end is shortly below)

// If you define ENCODINGS_AVAILABLE, the program will
// accept queries for language encodings as well as
// sight-related options.  You need to also define
// ENCODINGS_VISIBLE for the options to actually be
// shown on the form
// #define ENCODINGS_AVAILABLE
// #define ENCODINGS_VISIBLE

// Leave the following line alone and at this point
#include "images.h"

// If you have a set of images available, you can
// get the program to access them by defining something
// like the following
//#define UNICODE_URL "http://" IP_ADDRESS "/ugif/"
#define UNICODE_EXTENSION ".gif"
// UNICODE_SEPARATOR may or may not be defined; if
// it is (say, to '/') then the images will be of
// the form xx/xx.gif rather than xxxx.gif
// #define UNICODE_SEPARATOR "/"

// UNICODE_HEIGHT and UNICODE_WIDTH are optional, but highly recommended.
// If one is present, they must both be present.
#define UNICODE_HEIGHT 18
#define UNICODE_WIDTH 16
// UNICODE_EXTRA_HTML may optionally be defined to specify additional
// HTML attributes
#define UNICODE_EXTRA_HTML "BORDER=0"

// MULTIPLE_STYLES_SUPPORTED is a feature of the special image server
// that can be used with the gateway (see the installation
// instructions for that image server for details, and don't uncomment
// this unless you have that image server)
//#define MULTIPLE_STYLES_SUPPORTED

// L_NO_FREQTBL may or may not be defined; it's just a message
// that is printed when there is no frequency table in a .tbl
// file
//#define L_NO_FREQTBL "<BR>I haven't yet done the frequency table for this language so detection may not be very good.<BR>"

// TBL_SUBDIR tells the program where to find its files -
// mostly the .tbl files for the language encodings (if
// available) but also help.htm.  If
// you do not define this then it will look in the current
// directory.  Define this if you don't want your cgi-bin
// directory to be cluttered with them.
// #define TBL_SUBDIR "~ssb22/tbl/"

// ********* End of language encodings stuff
// (but you do need to look at TBL_SUBDIR)

// If you define LYNX_CFG_FILE, it will be used as
// an alternative configuration file for Lynx (rather
// than the default).  It will obviously not be used when
// Lynx is not used (for example, wget or the internal code
// is used).
// #define LYNX_CFG_FILE TBL_SUBDIR "lynx.cfg"

// If you define BRIEF_RESPONSE, the program will assume that
// it is running on a server that prefers to sort out the HTTP
// headers itself.  Most modern servers need this.
#define BRIEF_RESPONSE

// You need to set the following to the IP address (or domain
// name) of the computer.  It is used to generate the links
// within the gateway.  (Domain names can be faster than IP
// addresses if you have local users with proxies and a Netscape
// PAC, since IP addresses usually go through the cache regardless,
// which is pointless for local access.)
#define IP_ADDRESS "127.0.0.1"

// You need to set CGI_LOCATION to the full path and
// file name of the access executable as seen from
// the outside world.
// This is used to generate the links
#define CGI_LOCATION "/cgi-bin/access.cgi"

// Set RELATIVE_CGI_LOCATION to the last part of
// CGI_LOCATION (i.e. how the script would refer to itself
// from within the same directory)
#define RELATIVE_CGI_LOCATION "access.cgi"

// If the gateway is installed on a webserver that can do
// both SSL and non-SSL, and you want to switch between them
// as needed (so long as the user doesn't disable it because
// the client can't do it), uncomment the following line.
// (If you always want SSL, or always non-SSL, leave it
// commented out.)
// #define CAN_SWITCH_SSL

// Please set ADMIN_EMAIL to YOUR email address.
// I cannot administrate your computer for you.
// (You can comment this out if you want to)
#define ADMIN_EMAIL "nospam@rubbish.com"

// If you only want the gateway to be available to local
// users, you need to define LOCAL_IP_PREFIX *or*
// LOCAL_DNS_SUFFIX, and edit accordingly.  0 is a
// terminator.
//#define LOCAL_IP_PREFIX { "131.111.", "129.169.", "128.232.", "194.83.240.6", "127.0.0.1", 0 }
//#define LOCAL_DNS_SUFFIX { ".cam.ac.uk", 0 }
#define MESSAGE_FOR_NON_LOCAL_USERS "Sorry, this machine only serves local users.  " \
"If you think you are a local user, the program is probably wrong so let me know.  Otherwise, you can try the gateway on the Association of C and C++ Users' site."
#define NON_LOCALS_CAN_GO_TO "http://www.accu.org/cgi-bin/accu/access/access"
#define NON_LOCALS_ORGANISATION "ACCU"

// If it's OK for non-local users to use the gateway, but
// you want the gateway to connect to a proxy that is
// outside your establishment (so it doesn't retrieve
// "local only" documents), you can specify the proxy below.
// Note that this is different from PROXY and PROXY_PORT
// elsewhere in this file (which will be used in all OTHER
// cases if you define it).  The default value of
// PROXY_PORT_FOR_NONLOCALS is to be the same as PROXY_PORT.
// Note that this proxy MUST support FTP requests as well.
// #define PROXY_FOR_NONLOCALS "some.proxy.net"
// #define PROXY_PORT_FOR_NONLOCALS 8080

// ***************************

// PROXY_FOR_NONLOCALS
// PROXY_PORT_FOR_NONLOCALS (if different)
// + port: default 8080
// (must support FTP as well)

// Some academic establishments experience problems with
// "local users only" when the gateway server has an alias.
// For example, students at the University of Cambridge (UK)
// sometimes have "ucam.org" aliases for their machines.
// Since the local proxy does not recognise this as being
// in the university, it uses its upstream proxy (the JANET
// national cache) and the request does not appear to be
// local.  Uncommenting the following line works around this
// problem as follows: If a request appears not to be local,
// and the browser specified which alias it is trying to
// connect to, then this alias is tested against IP_ADDRESS
// (assumed to be the canonical host name to be used);
// if they do not match, then an HTTP re-direction is
// returned so that the browser tries the canonical name.
// This is not done with POST requests, since not all
// browsers re-post the data.
// #define REDIRECT_NONLOCAL_IF_HTTP_HOST_NOT_MATCH_IP

// If you want the gateway to point out to users that a different copy
// might be faster in a different country, then uncomment the
// following and edit accordingly.  This is independent of the "local
// users only" feature; users from the specified domain names are not
// restricted from using this gateway, but get a short message with a
// link to what might be a more appropriate gateway.  They are still
// allowed to continue using this gateway (besides anything else, the
// other one might be down).  The following example tells users in
// Australia and Japan about the Monash University gateway, and
// (hypothetical) users on Mars about a hypothetical Martian gateway.
// Please do not try to recognise America by .com or .net (these can
// be in any country).
// Note: If you have defined USE_GNU_SOCKETS, the gateway
// will also try to check where the user really is in case
// the user is behind a proxy, if the proxy provides this
// information.
//#define MIRRORS {"Monash University","http://www.csse.monash.edu.au/cgi-bin/cgiwrap/~ssb/access",".au",".jp",0,"Mars University","http://www.uni.mars/cgi-bin/access",".mars",0,0}

// You need to make sure that the correct file is included for spawnl or execl.
// On NT you should get away with
// #include <process.h>
// but on Unix systems it varies, but it is normally
// ALL THREE of the following:
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>

// Also define the following on Unix systems:
#define STRICMP_NEEDS_IMPLEMENTING

// and you'll probably need to define this on Win32:
// #define MEMCPY_IS_IN_MEM_DOT_H

// But you should never need to define:
// #define STR_FUNCTIONS_NEED_IMPLEMENTING
// unless your compiler is really old

// Please write your own message in this:
#define UPDATE_SPECIAL_MESSAGE "The host of this gateway can put a message here."

// Please leave the following line untouched:
#include "extensions.h"

/* The rest of this file relates to extensions.  This
is an API-like thing that allows programmers to add functionality
to the gateway.  If you want to install extensions on your gateway,
you may do so here.  However, it is very easy to mess things up if you
don't know what you're doing - you can, for example, get the
gateway to run a command to format the disk every time it's called
(not recommended).  If you're not sure, please leave this bit alone.
You do not have to install extensions if you don't want to (or don't
have any).

For documentation about the extensions mechanism, please see the file extenlib.h.
*/

// Here's one to get you started:
#define Script_Extensions \
Include_Script_Extension("imgmag","<SCRIPT LANGUAGE=\"JavaScript1.1\" SRC=\"http://ssb22.joh.cam.ac.uk/~ssb22/zoomOut.js\"></SCRIPT>","Peter Belesis' NS4/IE4 DHTML for enlarging images that are clicked on (modified S Brown)");

// Please leave the following line untouched:
#endif

