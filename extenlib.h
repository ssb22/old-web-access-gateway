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
#ifndef EXTENLIB_H
#define EXTENLIB_H

/*
WEB ACCESS GATEWAY EXTENSIONS MECHANISM
=======================================

Extensions should be placed in platform.h, after the
#include "extensions.h" directive.

SCRIPT EXTENSIONS

There are three types of extensions: Internal, external
and script.  Script extensions are the easiest to deal
with - they just involve putting a line onto the end of
each page (presumably a useful piece of DHTML - yes
these things can sometimes be used for accessibility
but are of course browser-dependent).  To add script
extensions, you need to remove the comments (//) from
the lines that follow, and modify the example line to
suit your extension (you can then add more lines before
the semicolon).  Note that if you want quotes (")
within the quotes then you should escape them ie. \"

IF YOU UNCOMMENT THIS THEN YOU SHOULD MOVE IT TO
platform.h (after the #include "extensions.h").
DO NOT LEAVE IT HERE.

// #define Script_Extensions \
// 	Include_Script_Extension("uid","HTML that the extension adds","Description, possibly including HTML tags")\
//	 ;

An example "script" extension is included in the
default platform.h (it might not be in your platform.h
if you used the installer script).  You CANNOT define
Script_Extensions more than once, so modify or remove
it (don't just add another one).

The "uid" is a 'unique' ID consisting of letters only,
and is used for portability between gateways (ie. if
you redirect the request to another access gateway that
has the same extension, it should still work).  For the
other types of extensions it is assumed that the
filename or class name is this ID.  Don't worry too
much about this.

EXTERNAL EXTENSIONS

External extensions are the next easiest to deal with -
they are programs on the disk that are run by the
gateway, and get to process the HTML before the gateway
does.  To add external extensions, you need to remove
the comments (//) from the lines that follow, and
modify the example line to suit your extension (you can
then add more lines before the semicolon; the
extensions are called in the order that you specify).
Note that a program of name my_filename (or whatever
you choose) should be in the executable path (i.e. it
should work if you pass it to 'system').  Please choose
a sensible filename without spaces etc.

Before calling your program, the gateway decompresses
the HTML (if it is compressed), reads the MIME headers
(and META tags if any), and detects the character set.

The program will be called with a single extra
parameter, which will be a filename for a file
containing the HTML that it is to process.  Its
standard output will be captured.  If instead you want
the program to take the HTML from its standard input,
you can say Include_Filter_Extension instead of
Include_External_Extension (but still put it under the
#define External_Extensions).  You can of course mix
the two types.

XHTML: If you have a program that requires XHTML, use
Include_XHTML_External_Extension and/or
Include_XHTML_Filter_Extension.  The HTML will be
converted to XHTML before your program is run.
You can of course mix HTML and XHTML extensions.  When
converting to XHTML, the gateway translates the
character set, so hopefully your parser does not have
to be internationalised.

IF YOU UNCOMMENT THIS THEN YOU SHOULD MOVE IT TO
platform.h (after the #include "extensions.h").
DO NOT LEAVE IT HERE.

// #define External_Extensions \
// 	Include_External_Extension("my_filename","Description, possibly including HTML tags")\
// 	;

INTERNAL EX|TEN|SIONS

The other type of extension is the internal extension,
which involves adding C++ code to the gateway program
itself.  Since you have the source, it is possible for
you to make any modifications you like, but if you want
to do this then I'd rather you did it in accordance
with this extension specification, since then it will
obviously be an extension (see legal stuff) and also it
should continue to work in future versions of the
gateway.

To make an extension, you need to write a class of the
following form, where MyName is the name of your
extension:

class ext_MyName : public Extension {
public:
ext_MyName();
virtual ~ext_MyName();
void tag(HTMLTag* t);
void startingSecondPass();
};

You should add a #include directive to platform.h, to
cause the definition of your class to be read in.  This
must be after the #include "extensions.h".  Note
that your class definition must NOT include inline code
that requires methods of classes like HTMLTag to be
called, since including extenlib.h will try to include
this platform.h and chaos could ensue.  Please put the
code in a C++ file, and make sure that #include
"extenlib.h" is on the *first* line of it.

The gateway processes its pages in two passes.
However, it may process more than one page on each run
(if, for example, someone has fetched a page with
frames and each frame needs to be fetched).  It
recursively calls itself to process these also, each
with two passes.

When it begins to process the page, it will construct
an instance of your class.  The instance will be
destroyed when it has finished.  There may be more than
one instance present if recursion is being used.  In
both of its passes, it calls your tag() method for each
non-comment tag that it finds.  You are at liberty to
change any of these tags, although changes made during
the first pass will not persist to the second pass, and
the only changes that directly affect the output would
be made in the second pass.  Between the passes, the
gateway will call your startingSecondPass().

Note that you should at no time write to the standard
output, since this is not the way that the gateway
outputs things.

Important: You may not necessarily be given all of the
tags in the second pass, since the gateway may be
missing out some of them.  For example, if it has been
instructed to remove JavaScript, whenever it encounters
a SCRIPT tag it will read to the next /SCRIPT,
discarding everything in between *and* discarding the
/SCRIPT, which you will never see.  (Please contact me
if this will cause a problem.)

The parameter given to your method is of type HTMLTag*.
The methods of HTMLTag that you may call are:

const char* getName() const
Returns the name of the tag (eg. IMG).

const char* getAttribute(const char* attribute) const
Returns the value of one of the tag's attributes
(eg. the value of SRC), or NULL if the tag does not
have that attribute.  If the tag has more than one
attribute of that name, then the last one is
returned.

const char* getAttributeFromLeft(const char* attribute) const
Like getAttribute, but if the tag has more than one
attribute of that name then the first one is
returned.  This is recommended for dealing with
HTML errors such as the following, which prevented
an older version of the gateway from treating a
Chinese university site as an HTML document:
<meta HTTP-EQUIV="Content-Type" CONTENT="text/html; &gt;
&lt;meta NAME=" GENERATOR"
CONTENT="Microsoft FrontPage 3.0">

void addAttribute(const char* lhs,const char* rhs)
Adds an attribute, with a given value, to the tag.
Set rhs to the empty string ("") if there is no
value for the attribute.

void setNameTo(const char* newName)
Changes the name of the tag.

void deleteAllByName(const char* name)
Removes all attributes that have the given name.
Does nothing if there are none.

All of these methods are case-insensitive, but
case-remembering.

Please make sure that any identifier you write does not
cause a naming conflict with the rest of the program.
If you do, you will have problems compiling it.  There
will also be problems if you try to add two extensions
of the same name.  As a precaution, the prefix "ext_"
is added to the start of your class name.

The format of specifying internal extensions to include
is similar to that of external ones - just uncomment
and modify the following.  Unfortunately you need to
say things twice; sorry but this is a limitation of
C++.  You should then copy your *.c++ files into the
same directory as the access gateway's source, so that
they are compiled into the gateway.  Remember to make
sure that your update script is modified accordingly
(i.e. don't let it delete your extension).

IF YOU UNCOMMENT THIS THEN YOU SHOULD MOVE IT TO
platform.h (after the #include "extensions.h").
DO NOT LEAVE IT HERE.

// #define Internal_Extensions1 \
// 	Include_Internal_Extension1(my_class)\
// 	;

// #define Internal_Extensions2 \
// 	Include_Internal_Extension2(my_class,"Description, possibly including HTML tags")\
// 	;

*/

#include "platform.h"
#include "extensions.h"
#include "tagstuff.h"

#endif
