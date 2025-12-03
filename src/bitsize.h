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
