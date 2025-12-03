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
/* LITERALS.H

This file contains the messages that the program
uses that may need translation if the user knows
little or no English.  Messages within quotes " "
are the parts to be translated.  If I get any
translations then I'll see what I can do about
making the language switchable on-the-fly.

- See also smileys.c++ if you expect blind users
- You may also like to see const char* encodings[]
in encoding.c++ and change the names of the
languages
- You may also like to translate part or all of the
help text in help.htm
- Also in makeform.c++ the colour names
*/

#ifndef LITERALS_H
#define LITERALS_H
#define L_SET_PWD_OPTIONS ("Password")
#define L_SET_LANGUAGE_OPTIONS ("Characters")
#define L_SET_COLOUR_OPTIONS ("Colours")
#define L_SET_ACCESS_OPTIONS ("Access")
#define L_SET_MISC_OPTIONS ("Options")
#define L_SET_EXTENSION_OPTIONS ("Extensions")
#define L_SET_SIZE_OPTIONS ("Size")
#define L_EXIT ("Leave gateway")
#define L_NO_ENCODINGS ("There is no Characters button on this server.")
#define L_COLOURS_INTRO ("Note: The colours selected here may not be effective if you have selected ``Use document body attributes'' or ``Disable all sight-related access options''.")
#define L_PWD_INTRO ("Authentication: Some Web pages need a user name and password, and you may set these here.&nbsp; Please note that this is nothing to do with logins through HTML forms, but, rather, browser authentication (ie. "\
"dialogue box popping up rather than boxes in the web page).&nbsp; <B>If you don't know what this is, you probably don't need it.</B>")
#define L_LANGUAGE_INTRO ("Languages are listed in alphabetical order by English name.")
#define L_EXTENSION_INFO ("The following third-party extensions are included in this copy of the gateway (select which ones to enable):")
#define L_MISCOPTS_INTRO ("Queries do not remain in the settings; use space for AND and comma for OR (lower precidence).")
#define L_DISABLE_SIGHT_OPTIONS ("Disable all sight-related access options (select this if you are fully sighted)")
#define L_CHANGE_ACCESS_OPTIONS ("Change access options")
#define L_MORE_OPTIONS ("More options")
#define L_IMAGE_FORM_WARNING ("The access gateway is using images and has just found a FORM.&nbsp; Characters in fields and buttons may not display correctly due to a limitation in HTML.")
#define L_CONVERSION_PROMPT ("Convert characters from which language?")
#define L_CONVERSION_NONE ("No conversion")

#define L_SHOW_CHARS_AS ("Show characters as:")
#define L_IMGS        ("Images (if available for language)")
#define L_DOUBLE_IMGS ("Big images (ask browser to enlarge)")
#define L_AMPERSAND   ("HTML ampersand sequences (&amp;#n;)")
#define L_UTF8        ("UTF-8")
#define L_SHIFTJIS    ("Shift-JIS and images (for iMode handsets etc)")

#define L_UNICODE_URL ("Alternative base URL for images")
#define L_DETECTED ("Detected")
#define L_OVERRIDE ("Override")
#define L_OVERRIDDEN ("Overridden to")
#define L_PWD_USER ("User")
#define L_PWD_PWD ("Password")
#define L_PWD_USER_ACCESSKEY ("U")
#define L_PWD_PWD_ACCESSKEY ("P")
#define L_HLP_ACCESSKEY ("H")
#define L_URL_ACCESSKEY ("L")
#define L_OPTIONS ("Options")
#define L_HELP ("Help")
// That "help" is different from the following "help", so
// make sure it's worded differently
#define L_HELP_ON  ("Help on these options")
#define L_HELP_OFF ("Hide the help")
#define L_URL_PROMPT ("Enter or paste URL")
#define L_OPTS_NOSSL ("Avoid switching to/from SSL")
#define L_OPTS_EMBED ("Replace audio EMBED with BGSOUND (for old versions of Internet Explorer)")
#define L_EMBEDDED_FILE ("Embedded file")
#define L_OPTS_HEAD ("Remove document head (for problem documents)")
#define L_OPTS_RESET ("Make reset buttons say ``Reset''")
#define L_OPTS_HIDDEN ("Show hidden form fields")
#define L_OPTS_NETFONT ("Fix Netscape 4.0 font/link bug (but the fix may change some document fonts)")
#define L_OPTS_URLBOX ("URL box at top of document")
#define L_OPTS_LONGBOX ("Make URL box longer")
#define L_OPTS_ACCESSLINK ("No ``change access options'' link")
#define L_OPTS_CHARSET ("No character set controls")
#define L_OPTS_STATUSLINE ("Don't add status line code to links")
#define L_OPTS_PARASINDENT ("Replace paragraphs with indentation")
#define L_OPTS_HARDWRAP ("Make all text areas word wrap")
#define L_OPTS_NOALT ("Remove ALT tags (for non-readers)")
#define L_OPTS_1252 ("Replace Windows quotes etc")
#define L_OPTS_COOKIEOPTS ("Use cookies for the options (faster)")
#define L_OPTS_DONTSTORE ("Don't store remote session IDs")
#define L_HELP_PROMPT ("For more help, press this button")
#define L_HELP_RETURN ("Return to the form")
#define L_EMAIL_ADMIN ("Email the administrator")
#define L_REDIRECT_REQUEST ("Redirect the request")
#define L_TO ("to") // Goes with L_REDIRECT_REQUEST (the "to" is not in the link)
#define L_LEAVE_GATE_INLINK ("Leave the gateway")
#define L_LEAVE_GATE_AFTERLINK (" and retrieve the page directly")
#define L_UNGETTABLE_MSG ("The program raised the following message: ")
// L_RETRIEVE_PROBLEM is put through text to HTML conversion (see cgilib.c++)
#define L_RETRIEVE_PROBLEM ("The Web access gateway failed to process this page.  You can try without the gateway:")
#define L_RETRPROB_LINKTEXT ("Leave gateway and go to the page")
#define L_RETRIEVE_PROB_2 ("The program raised the following message: ")
#define L_RETRIEVE_PROB_3 ("This could be a problem with the gateway, but \x1 please check that the URL is complete\x2 (including all www, .com etc).  " \
"It is also possible that the page is not a HTML document, or that it is accessible only from your system or local network.")
#define SERVICE_TITLE ("Web access gateway")
#define INTERNAL_ERROR ("Something has gone wrong with the access gateway.  Don't worry - this is not your fault.  " \
"Please continue to use the gateway - this problem should be fixed later.  ")

#ifdef ADMIN_EMAIL
#define IEC_ADMIN ADMIN_EMAIL
#else
#define IEC_ADMIN "the administrator"
#endif
#define INTERNAL_ERR_CONTACT ("Please contact " IEC_ADMIN " and send the following text.  Many thanks.")

#define INTERNAL_ERR_LOGGED ("The problem has been logged for investigation.")

#define WEBSITE "www.cl.cam.ac.uk/~ssb22/access.html"
// The \n after <HR> is necessary for some alpha versions of Mozilla (otherwise it ignores the user's font for a while)
#define L_UPDATE_STRING UPDATE_SPECIAL_MESSAGE "<HR>\n" \
"This is Web Access Gateway, version " VERSION_STRING " (" UPDATE_DATE ").&nbsp; See <A HREF=\"http://" WEBSITE "\">" WEBSITE "</A> for newest version or information." \
"<BR><SMALL>&copy; 1998-" COPYRIGHT_YEAR " Silas S. Brown.&nbsp; " \
"Disclaimer: The author is not responsible for, and has no authority over, the websites that host this program.&nbsp; " \
/* (in other words, I'm not the one to sue if you want to force someone to remove their copy or whatever) */ \
"There is no warranty, either express or implied.&nbsp; " \
"It is the user, not the author, that chooses which pages to process, and the author does not endorse (and is not responsible for) any illegal viewing or modification of data, whether the program is functioning properly or not.</SMALL>"
// (in other words, if some bug clobbers half the text on your web site, don't sue me)

#define L_COMMENT_STRING "This page was processed by Web Access Gateway version " VERSION_STRING ".\nYou might be able to reach the gateway's control panel by appending &" ENV_SHOW_OPTIONS "=1 to the URL." // unless someone has saved it to a file

#define L_SPECIAL_CASE_HZ ("Hz")
#define L_SPECIAL_CASE_ZW ("zW")
#define L_SPECIAL_CASE_SJIS ("Shift-JIS or Mac")
#define L_ISO_CODES ("ISO codes")
#define L_AUTO_DETECT ("Detect automatically")
#define L_ENCODING_SUBMIT_BUTTON ("OK")
#define L_OMIT_SUB_ENCODING ("Don't decode HTML escapes before interpreting characters")
#define L_OMIT_SERVER_ENCODING ("Always ignore `charset' headers")
#define L_ISINDEX_START ("This is a searchable index.&nbsp; Enter search keywords:")
#define L_ACCESS_STATUS_PREFIX ("Access ")
#define L_LAST_MODIFIED ("Page's date stamp")
#define L_PAGE_CONTAINS_WARNING ("This page contains a <b>WARNING</b> for access gateway users")
#define L_PAGE_ALREADY_PROCESSED ("This page has already been processed by the access gateway.  You may be pointing the gateway at itself.")
#define L_OPTS_DATESTAMP ("Don't show date stamp")
#define L_NOFRAMES_MSG ("The access gateway has encountered a page that uses frames but you have not asked it to process them and your browser is not supporting them.&nbsp; Please ask the gateway to process frames or use a different browser.")
#define L_SERVER_SIDE_MAP ("The access gateway has encountered a server-side image map.&nbsp; There is nothing it can do to make this accessible so it is including the map itself below.&nbsp; Turn off map processing to suppress this message.")
#define L_UNGETTABLE_FRAME_1 ("At this point the page contained a frame with the following URL: ")
#define L_UNGETTABLE_FRAME_2 ("The access gateway failed to retrieve this.&nbsp; The program raised the following message: ")
#define L_JAVASCRIPT_WARNING ("Warning: This is a JavaScript link and probably won't work through the access gateway: ")
#define L_JAVASCRIPT_GUESS ("JavaScript contains URLs; guessing ")
#define L_JAVASCRIPT_LINK_GUESS ("Warning: Link URL has been guessed from JavaScript: ")
#define L_SECURITY_WARNING ("Warning: This %s points to a `secure' page, which won't go through the access gateway as this server doesn't have lynx-ssl: ") // %s OK
#define L_SECURITY_2 ("Warning: This %s points to a `secure' page; you have to trust the access gateway and your connection to it: ") // %s OK
#define L_SECURITY_2_SSL ("NB this %s points to a `secure' page; you have to trust the access gateway: ") // %s OK
// (above: say "connection" not "link" because the two uses of the word "link" can be confusing, 2001-08-31)
#define L_SECURITY_LINK ("link")
#define L_SECURITY_FORM ("form")
#define L_SSL_WARNING ("<B>Warning</B>: This page has been retrieved from a `secure' server, but you have to trust the access gateway and your connection to it.")
#define L_SSL_WARNING_SSL ("This page has been retrieved from a `secure' server; you have to trust the access gateway.")

#define L_TABLE_TRIM ("Delete table rows not matching query")
#define L_LINK_FOLLOW ("Process as frames links that match query")
#define L_PROXY_PROBLEM ("The access gateway has detected that you are trying to use it through a filtering proxy.&nbsp; The gateway's author is concerned that this might get him into trouble.&nbsp; Please ask your administrator to bypass the proxy.")
#define L_DEEP_LINKING ("<B>ERROR</B>: This server no longer allows ``deep linking'' into the mediator because it caused too much traffic. The mediator is <b>only</b> for people who <b>cannot</b> do without it (so for example, if you are using the mediator to read Japanese, please try to install the Japanese fonts locally first). If you still want to use the mediator then go to www.csse.monash.edu.au/~jwb/japanese.html and follow the Access-J link.")
#define L_ENCTYPE_WARNING ("<B>Warning</B>: The access gateway has found a FORM that uses `multipart' encoding.&nbsp; This is currently only partially supported; things might not work properly yet.")
#define L_WELCOME ("Welcome to the Access Gateway.")

#define L_MISC_HEADING ("Miscellaneous")
#define L_SEARCH_HEADING ("Search")
#define L_NAV_HEADING ("Navigation")
#define L_BROWSER_HEADING ("Browser quirks")
#define L_COOKIES_HEADING ("Cookies")

#define L_MIRROR_START ("The ")
#define L_MIRROR_LINK ("gateway at ")
#define L_MIRROR_END ("might be faster from where you are.")

#define L_RATING_WARNING ("Warning: The requested page has a PICS label claiming unsuitability for children.")
#define L_RATING_ABORT ("Abort")
#define L_RATING_CONTINUE ("Continue")

// The following group of messages is used by the
// sight-related functions, and need not be translated
// if the gateway is not to be used in this context.
#define L_IMG_MAP_WITHOUT_AREA ("(Image map with no area information)")
#define L_FRAME_SEP_START ("Frame separator: Start")
#define L_FRAME_SEP_END ("Frame separator: End")
#define L_START_PAGE ("Start of page")
#define L_PAGE_REFRESH ("This page periodically refreshes to ")
#define L_PAGE_NOURL_REFRESH ("This page periodically refreshes itself.")
#define L_TAB_ROW_SEP ("Table row separator")
#define L_BGCOLOUR ("Background colour")
#define L_FGCOLOUR ("Foreground colour")
#define L_LINKCOLOUR ("Link colour")
#define L_ALINK_COLOUR ("Active (selected) link colour")
#define L_VLINK_COLOUR ("Visited link colour")
#define L_HOVER_BG ("Background colour of link under mouse (if supported)")
#define L_BOLD_COLOUR ("Colour for bold text")
#define L_HEAD_COLOUR ("Colour for headings")
#define L_FONTSIZE_PROMPT ("Select a font (fount) size, if ignoring document fonts.&nbsp; If your browser doesn't support font size, try the headings below them, but that can cause formatting problems.")
#define L_BROWSER_SIZE ("Use browser size")
#define L_FONT_SIZE ("Font size")
#define L_HEADING_FONT_SIZE ("Always use heading")
#define L_ACCESS_INTRO ("Use the following checkboxes to disable selected sight-related access options (some of them <b>en</b>able the more unusual options; see help text for full details)")
#define L_A_SCRIPTS ("Enable scripts (read help before using this)")
#define L_A_NO_SCRIPT_GUESS ("If scripts not enabled, don't attempt to guess their links either")
#define L_A_NO_ABBR_URLS ("Don't abbreviate URLs when displaying links")
#define L_A_STYLE ("Enable style sheets")
#define L_A_REFRESH ("Enable delayed refresh/redirection")
#define L_A_BODY ("Use document body attributes (background etc)")
#define L_A_FONT ("Use document font settings")
#define L_A_APPLETS ("Enable applets")
#define L_A_PARAMS ("If applets not enabled, don't attempt to process their parameters either")
#define L_A_EMBED ("Enable embedded media")
#define L_A_FLASH ("Don't try to extract text from Flash")
#define L_A_BLINK ("Don't change blinking text to bold")
#define L_A_UL ("Don't change underlined text that is not a link to bold")
#define L_A_ITAL ("Don't change italic text to bold")
#define L_A_LINKS ("Don't put [ ] around links")
#define L_A_TABLES ("Leave tables as they are")
#define L_A_GENTAB ("If leaving tables, make them generic")
#define L_A_FRAMES ("Leave frames as they are")
#define L_A_GENFRAMES ("If leaving frames, make them generic")
#define L_A_MARQUEE ("Don't change marquee text into plain text")
#define L_A_SMILEY ("Perform smiley substitution (slow)")
#define L_A_SPACEDETECT ("Don't attempt to remove surplus spaces between characters")
#define L_A_LINKIMG ("Leave images in links as images")
#define L_A_IMGMAP ("Leave image maps as they are")
#define L_A_ALTREPEAT ("Repeat ALT tags after images")
#define L_A_REMOVEIMGS ("Remove ALL images")
#define L_A_ADOBE ("Don't use access.adobe.com for PDFs")
#define L_PDF_ORIG ("Original PDF file")
#define L_A_NOBR ("Don't reformat long lines")
#define L_A_NOSUBMIT ("Don't remove images from buttons")
#define L_HEADING_TYPE ("Heading")
#define L_HEADING_PROMPT ("Promote headings that are below:")
#define L_END_OF_PAGE ("End of web page.")
#define L_A_ENDPAGE ("Don't add ``End of web page.''")
#define L_A_BANNERMOVE ("Don't move link banner to bottom")
#define L_A_HEIGHTSTRIP ("Don't strip HEIGHT with percentage")
#define L_A_SPACERSTRIP ("Don't strip SPACER tags")
#define L_A_ACRONYM ("Don't expand ``Acronym'' tags")

#define L_A_SCRIPT_HEADING ("Scripts and applets")
#define L_A_FRAMES_HEADING ("Frames and tables")
#define L_A_IMG_HEADING ("Images")
#define L_A_STYLE_HEADING ("Style")
#define L_A_SPEECH_HEADING ("Screenreader")

// L_NO_FREQTBL is in platform.h

#endif
