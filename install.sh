#!/bin/bash

#  Access Gateway - a mediator for the World Wide Web -
#  "quick" install script for POSIX-compliant Unix systems

# (C) 1998-2006 Silas S. Brown http://ssb22.user.srcf.net

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# ######################################################

export SSB_Email=ssb22@cam.ac.uk

# Check for dependencies
if ! which lynx >/dev/null; then
  echo "Sorry, you need a copy of lynx."
  echo "The gateway can manage without lynx for most requests, but it still needs it for certain types of request."
  echo "Please make sure that lynx is in the PATH and try again."
  echo "Aborting."
  exit 1
fi
if ! which gzip >/dev/null; then
  echo "Sorry, you need a copy of gzip."
  echo "Please make sure that gzip is in the PATH and try again."
  echo "Aborting."
  exit 1
fi
if ! which g++ >/dev/null; then
  echo "Sorry, you need a copy of g++."
  echo "Please make sure that g++ is in the PATH and try again."
  echo "Aborting."
  exit 1
fi
if ! which tar >/dev/null; then
  echo "Sorry, you need a copy of tar."
  echo "Please make sure that tar is in the PATH and try again."
  echo "Aborting."
  exit 1
fi
if ! which sed >/dev/null; then
  echo "Sorry, you need a copy of sed to run this install script."
  echo "Please make sure that sed is in the PATH and try again."
  echo "Aborting."
  exit 1
fi
if ! which killall >/dev/null; then
  export KillAll=pkill
  if ! which pkill >/dev/null; then
    echo "Sorry, you need a copy of killall or pkill."
    # NEEDATTENTION This is wrong - only need pkill if installing an image server
    echo "Please make sure that pkill is in the PATH and try again."
    echo "Aborting."
    exit 1
  fi
else
  export KillAll=killall
fi

if lynx --help 2>&1 | grep "Looking up" >/dev/null; then
  echo "Please remove the START_FILE entry from the Lynx configuration file (see the annotated platform.h for details of the problem it causes)"
  echo "Aborting"
  exit 1
fi

if lynx -version |grep \ 2\.8 >/dev/null; then
  export Lynx_Is_28=true
elif lynx -version |grep \ 2\.7 >/dev/null; then
  export Lynx_Is_27=true
  echo "SECURITY ALERT: Your version of Lynx is 2.7.  You are strongly advised
to upgrade to at least version 2.8.1, since there is a security flaw in
versions 2.7.* that allows users to run arbitrary commands by forming the
right URL. (Since I do not have information about exactly how to do this, I
can't get the gateway to protect against it.)"
  echo "Press Ctrl-C to abort or Enter to continue"
  read
else
  echo "Sorry, I need Lynx version 2.7 or 2.8 (preferably 2.8)."
  echo "(If the Lynx version is greater than 2.8, this script needs updating!)"
  echo "Aborting."
  exit 1
fi

if g++ -v 2>&1|grep \ 2\.7 >/dev/null; then
  export HandleExceptions=-fhandle-exceptions
  export BrokenHandling="#define BROKEN_EXCEPTION_HANDLING"
else
  export HandleExceptions=
  export BrokenHandling=
fi

# Get user input
echo "I need to ask some questions.  Please don't give silly answers because
this script doesn't check them.  Use Ctrl-C to abort installation."
unset DomainName; while test "a$DomainName" = a; do
  echo "What is the host's domain name? (eg. www.myserver.com)"
  echo "(you can write host:port if you like)"
  read DomainName
done
export DomainOnly=`echo $DomainName|sed -e "s/:.*//"`
unset CgiOutside; while test "a$CgiOutside" = a; do
  echo "What is the CGI directory, as seen from the outside world? (eg. cgi-bin)"
  read CgiOutside
done
unset CgiInside; while test "a$CgiInside" = a; do
  echo "Where is this directory really located? (eg. /usr/lib/cgi-bin)
(Please enter a FULL path, INCLUDING the leading /)"
  read CgiInside
done
echo "If the gateway is installed on a webserver that can do
both SSL and non-SSL, and you want to switch between them
as needed (so long as the user doesn't disable it because
the browser can't do it), say y here.
(If you always want SSL, or always non-SSL, just press return.)"
echo "Enable SSL switching?"
read CanSwitchSSL
echo "Please enter a full directory name for the gateway to keep its data files.  Programs run by the Web server should be able to read this directory.  If it does not exist, the install script will try to create it.  Default is $CgiInside/data"
unset CgiData; read CgiData
if test "a$CgiData" = a; then export CgiData=$CgiInside/data; fi
if ! test -e $CgiData; then mkdir $CgiData && echo "Created directory $CgiData"; fi
echo "What do you want to call the program? (default access.cgi)"
read CgiName
if test "a$CgiName" = a; then export CgiName=access.cgi; fi
echo "Would you like the program to fetch pages through a proxy? (recommended)"
echo "Note: Using a proxy also means you can (if
you wish) restrict the sites that the gateway
can process, by configuring the proxy
accordingly.  If you want the gateway to process
only 1 server then you don't even need a proxy;
simply set that server's port 80 as 'proxy'."
echo "Use a proxy?"
echo "Please enter y or n (default y)"
read ProxyYN
if ! test "a$ProxyYN" = an; then
  echo "Please enter the name of the proxy (e.g. cache.myisp.net)"
  read ProxyName
  echo "Please enter the port (e.g. 8080 or 3128)"
  read ProxyPort
  echo "Is the proxy capable of FTP also?  (y/n, default y)"
  echo "(Nearly all proxies are capable of FTP)"
  read ProxyFTP
fi
echo "Would you like to install an image server too?"
echo "If you do not install an image server, the gateway can use an image server on another machine.  If you do install an image server then you will need a spare port number that is not blocked by your firewall.  The image server runs as a daemon (you can get some statistics by pointing your browser at http://<machine>:<port>)."
echo "Install an image server?  Please enter y or n (default y)"
read ImgServerYN
if ! test "a$ImgServerYN" = an; then
  echo "What port number do you want to use for the image server?  (Default 7080)"
  read ImagePort
  if test "a$ImagePort" = a; then export ImagePort=7080; fi
else
  echo "Which image server would you like to use?  Please specify host and port."
  unset ImgServer; read ImgServer
fi
echo "What is your email address? (You can leave this blank if you want)"
read AdminEmail
echo "What additional HTML do you want displayed on the opening screen?
(eg. This copy hosted by ...)  (Can be left blank, but please do not include
quotes.  You only get one line with this script, but it can be long.)"
read ExtraText

echo "Configuring; please wait."

export UpdateScript=./compile.sh
echo \#!/bin/bash > $UpdateScript
if ! test -e $UpdateScript; then
  echo "Failed to create $UpdateScript"
  echo "Check your permissions to this directory and try again."
  echo "Aborting."
  exit 1
fi

if ! test "a$ImgServerYN" = an; then
  export ImgUpdateScript=./img-compile.sh
  echo \#!/bin/bash > $ImgUpdateScript
  if ! test -e $ImgUpdateScript; then
    echo "Failed to create $ImgUpdateScript"
    echo "Check your permissions to this directory and try again."
    echo "Aborting."
    exit 1
  fi
  echo "export PATH=$PATH" >> $ImgUpdateScript
  echo "export TERM=vt320" >> $ImgUpdateScript
  echo "mkdir server" >> $ImgUpdateScript
  echo "cd server" >> $ImgUpdateScript
  echo "gzip -d ../imgserver.tar.gz 2>&1 >/dev/null" >> $ImgUpdateScript
  echo "tar -xf ../imgserver.tar || exit 1" >> $ImgUpdateScript
  echo "rm -f ../imgserver.tar" >> $ImgUpdateScript
  echo "g++ server.c++ -oimgserver -O2 -DData_File_Pathname=\\\"imgserver.dat\\\" -DNOLOG -D_ISOC99_SOURCE $HandleExceptions || exit 1" >> $ImgUpdateScript
  # echo "rm -f server.c++" >> $$ImgUpdateScript
  echo "if test a\$1 = a; then echo \"(Re)starting image server...\"; fi" >> $ImgUpdateScript
  echo "$KillAll imgserver 2>/dev/null >/dev/null ; ulimit -n 1024 ; ./imgserver $ImagePort 2>/dev/null >/dev/null &" >> $ImgUpdateScript
  chmod +x $ImgUpdateScript
fi

echo "export PATH=$PATH" >> $UpdateScript
# Check for changes (errors to /dev/null because of intermittent accu.org downtime)
echo "export TERM=vt320" >> $UpdateScript # in case it's a version of lynx
# that refuses to do anything without TERM (even -source)
echo "g++ *.c++ -O2 -o$CgiName $HandleExceptions || exit 1" >> $UpdateScript
echo "mv $CgiName $CgiInside" >> $UpdateScript
# echo "rm *.c++ *.tar *.h" >> $UpdateScript
# echo "mv * $CgiData" >> $UpdateScript # help.htm, *.tbl
echo "cp help.htm *.tbl $CgiData" >> $UpdateScript
chmod +x $UpdateScript

# Create a platform.h
echo "// This is a PLATFORM.H created by $0 on $(date)" > platform.h
echo \#ifndef PLATFORM_H >> platform.h
echo \#define PLATFORM_H >> platform.h
echo $BrokenHandling >> platform.h
echo \#define LYNX_NEEDS_TERM >> platform.h
echo \#define LYNX_COMMAND \"$(which lynx)\" >> platform.h
echo \#define GZIP_COMMAND \"$(which gzip)\" >> platform.h
if ! test a$Lynx_Is_28 = a; then
  echo \#define LYNX_USER_AGENT \"access\" >> platform.h
fi
echo \#define COMMAND_SPECIAL_CHAR_PREFIX \'\\\\\' >> platform.h
echo \#define USE_POSIX_FORK >> platform.h
echo \#define USE_GNU_SOCKETS >> platform.h
echo \#define TMP_PREFIX \"/tmp/\" >> platform.h
if ! test "a$ProxyYN" = an; then
  echo \#define PROXY \"$ProxyName\" >> platform.h
  echo \#define PROXY_PORT $ProxyPort >> platform.h
  if ! test "a$ProxyFTP" = an; then
    echo \#define PROXY_FTP >> platform.h
  fi
fi
echo \#define TBL_SUBDIR \"$CgiData/\" >> platform.h
echo \#define ENCODINGS_AVAILABLE >> platform.h
echo \#define ENCODINGS_VISIBLE >> platform.h
echo \#include \"images.h\" >> platform.h
echo \#define UNICODE_URL \"$ImgServer\" >> platform.h
echo \#define UNICODE_EXTENSION \".gif\" >> platform.h
echo \#define UNICODE_EXTRA_HTML \"BORDER=0\" >> platform.h
echo \#define UNICODE_HEIGHT 18 >> platform.h
echo \#define UNICODE_WIDTH 16 >> platform.h
echo \#define MULTIPLE_STYLES_SUPPORTED >> platform.h

echo \#define BRIEF_RESPONSE >> platform.h
echo \#define IP_ADDRESS \"$DomainName\" >> platform.h
echo \#define CGI_LOCATION \"/$CgiOutside/$CgiName\" >> platform.h
echo \#define RELATIVE_CGI_LOCATION \"$CgiName\" >> platform.h
if ! test "a$CanSwitchSSL" = a; then
  echo \#define CAN_SWITCH_SSL >> platform.h
fi
if ! test "a$AdminEmail" = a; then
  echo \#define ADMIN_EMAIL \"$AdminEmail\" >> platform.h
fi
echo \#include \<unistd.h\> >> platform.h
echo \#include \<sys/types.h\> >> platform.h
echo \#include \<sys/wait.h\> >> platform.h
echo \#define STRICMP_NEEDS_IMPLEMENTING >> platform.h
echo \#define UPDATE_SPECIAL_MESSAGE "\"$ExtraText\"" >> platform.h
echo \#include \"extensions.h\" >> platform.h
echo \#include \"bitsize.h\" >> platform.h
echo \#endif >> platform.h

# Run the update script
echo "Configuration complete.  Installing the gateway..."
$UpdateScript
if ! test "a$ImgServerYN" = an; then
  echo "Installing the image server..."
  $ImgUpdateScript
fi

# Print the URL
if ! test "a$ImgServerYN" = an; then
  echo "You can get statistics about your image server from"
  echo $ImgServer
fi
echo "You can test your installation by going to"
echo http://$DomainName/$CgiOutside/$CgiName
echo "If you tell me this URL, I can link to your copy."
# Can we send mail?
if which mail >/dev/null; then
  echo "Would you like this script to email the URL to me now?"
  echo "Please answer y or n (default y)"
  read Response
  if ! test a$Response = an; then
    echo "Many thanks.  I hope your 'mail' command works!"
    echo "(If not please send me the URLs by hand, thanks)"
    echo "Trying...."
    if echo http://$DomainName/$CgiOutside/$CgiName | mail $SSB_Email -s "install.sh: New gateway installation"; then
      echo "The mail command seemed to work."
    else echo "There was a problem sending the mail; please send the URLs by hand, thanks."
    fi
  fi # not answered 'n'
fi # mail

# Can we install a crontab entry?
if which crontab >/dev/null; then
  unset NoCrontab; unset NoPermissions; unset EditWarning
  if crontab -l 2>&1 | head -1 | grep -i "^no crontab for " >/dev/null; then export NoCrontab=true
  elif ! crontab -l 2>&1 >/dev/null; then export NoPermissions=true
  elif crontab -l | head -1 | grep "DO NOT EDIT THIS FILE" >/dev/null; then export EditWarning=true
  # 2001-06-20: Not necessarily starting at beginning of line; some versions of cron put a "# " before it
  fi
  if test a$NoPermissions = a; then
    # It seems that we can go ahead
    # (just check with the user)
    export ScratchFile=$(mktemp /tmp/gatewayCronXXXXXX)
    unset NeedToRunCron
    if ! test "a$ImgServerYN" = an; then
      echo "Would you like a cron entry to periodically start the image server if it is not running?  This is highly recommended, especially if the machine reboots.  Please answer y or n (default y)."
      read Response
      if ! test "a$Response" = an; then
        export NeedToRunCron=y
        echo "* * * * * ulimit -n 1024 ; $(pwd)/server/imgserver $ImagePort 2>/dev/null >/dev/null" >> $ScratchFile
      fi
    fi # ImgServerYN
    if test a$NeedToRunCron = ay; then
      if test a$EditWarning = atrue; then
        # Need to strip the first three lines
        crontab -l | tail -$(echo "$(crontab -l|wc -l)-3"|bc) >> $ScratchFile
      elif test a$NoCrontab = a; then
        crontab -l >> $ScratchFile
      fi
      cat $ScratchFile | crontab -
      rm $ScratchFile
    fi # NeedToRunCron
  else # NoPermissions
    if ! test "a$ImgServerYN" = an; then
      echo "WARNING: You do not seem to have permission to modify crontab.  It is HIGHLY RECOMMENDED that you add the following line to your crontab:"
      echo "* * * * * ulimit -n 1024 ; $(pwd)/server/imgserver $ImagePort 2>/dev/null >/dev/null"
      echo "This will periodically start the image server if it is not already running (useful if the machine goes down or the server dies)."
    fi
  fi # NoPermissions
else # which crontab
  if ! test "a$ImgServerYN" = an; then
    echo "WARNING: You do not seem to have a crontab command.  It is HIGHLY RECOMMENDED that you add the following line to your crontab:"
    echo "* * * * * ulimit -n 1024 ; $(pwd)/server/imgserver $ImagePort 2>/dev/null >/dev/null"
    echo "This will periodically start the image server if it is not already running (useful if the machine goes down or the server dies)."
  fi
fi # which crontab

echo "install.sh has finished."
