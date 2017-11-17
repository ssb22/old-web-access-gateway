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
#ifndef BITSIZE_H
#define BITSIZE_H

/*
In response to Jon Jagger's article "Compile Time Assertions in C" (C Vu 11.3),
I wonder if it is really necessary to have a general-purpose assertion in
order to achieve what he wants?  Besides the problems Francis mentions, it
would be nice if the error messages were more meaningful.  But CHAR_BIT can
be tested directly:
*/

#include <limits.h>
#if CHAR_BIT<8
#error This program assumes a compiler that supports 8-bit chars
#endif

/*
Unfortunately limits.h does not define the number of bits in the other types,
only the maximum values, and the preprocessor can wrap around values that are
larger than these.  So to check 'int', once you have established that
there are 8-bit chars you have to do a painstaking byte-for-byte comparison:
*/

#define byte1(c) ((c)&0xFF)
#define byte2(c) byte1((c)>>8)
#define byte3up(c) ((c)>>16)
#define TwoByteFail(c,min2,min1) (!byte3up(c) && (byte2(c)<min2 || byte1(c)<min1))

#if TwoByteFail(USHRT_MAX,0xFF,0xFF)
#error This program assumes a compiler that supports 16-bit shorts
#endif
#if TwoByteFail(UINT_MAX,0xFF,0xFF)
#error Strange - it seems your 'int' is shorter than your 'short'
#endif

/*
but once a minimum 16-bit word length is established, you can take advantage
of it when checking 'long':
*/

#define word1(c) ((c)&0xFFFF)
#define word2(c) word1((c)>>16)
#define word3up(c) ((c)>>32)
#define TwoWordFail(c,min2,min1) (!word3up(c) && (word2(c)<min2 || word1(c)<min1))

#if TwoWordFail(ULONG_MAX,0xFFFF,0xFFFF)
#error This program assumes a compiler that supports 32-bit longs
#endif

/*
Of course, this is still no good for checking the sizes of compiler-provided
structs and typedefs.  If you are concerned about them then another approach
is to write an entirely separate conformance test program, that can give
meaningful error messages when things are wrong, and insist that the user
compile and run it before proceeding with installing your program.  You could
get it to return an error code through exit(), so any batch install script
can stop if necessary.
*/
#endif
