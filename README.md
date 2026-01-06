# old-web-access-gateway
Old Web Access Gateway from https://ssb22.user.srcf.net/access
(also [mirrored on GitLab Pages](https://ssb22.gitlab.io/access) just in case)

The Web access gateway (1997-2006) was an online “browser within a browser”, giving you more control over how websites are displayed, regardless of which browser you have to use. The gateway had two main purposes:

1. To make the Web easier to access for print-disabled users (such as users with low vision or dyslexia);

2. To allow speakers of other languages to view Web pages written in them, when the encodings were not supported by their browsers.  This used non-optimal GIFs to work around the GIF patent problem: it came to my attention that both Unisys and IBM obtained patents on the Lempel Ziv Welch (LZW) data compression and decompression algorithm that is used in GIF files.  The language viewer in the access gateway used GIF files for its foreign characters.  Many users relied on this because their browsers were old or limited and could not display the PNG format.  Therefore, I modified the GIF files so that they do not use the Lempel Ziv Welch (LZW) data compression algorithm and did not infringe the patents on that algorithm.  This made the files larger.  The patents should now have expired, but I’m no longer actively maintaining the language viewer images so I have no plans to reverse this change. Web Adjuster has more modern image rendering.  See also my essay on [software patents and mental health](patents.md).

Web Access Gateway's support of Javascript is limited and is not suitable for modern “Web application” sites.

* If you have a recent desktop browser, you might be better off using my stylesheets for low vision (although some web applications manage to break even on those).

* If you still need a mediator, my [Web Adjuster](https://ssb22.user.srcf.net/adjuster/) might do what you want (especially if you can run Javascript in the browser; see for example [large-print-websites.appspot.com](http://large-print-websites.appspot.com/) or [cjk-website-renderer.appspot.com](http://cjk-website-renderer.appspot.com/)).

* On the commercial side there’s Openwave Web Adapter—I was on their team for 2 years and the American “Sprint” carrier licensed it to simplify web navigation before ‘smartphones’ became popular. Some “proxy-rendering” “microbrowsers” (Opera Mini, China’s “UC Browser”, etc) also run Javascript on a proxy but only for their browsers. I don’t know if Infogin etc ever got around to full Javascript support.

The remainder of this page is now mostly for historical interest.

Infogin managed to get two “methods patents” in the US (patent numbers 7,047,033 and 7,920,894) whose Claim 1 arguably covered my gateway even though I had published first (see ISBN 978-1-84800-050-6 p.236 and ISSN 1354-3164 Vol.11 No.1, both in 1998, and w3.org mailing-list archives 1999).  If those patents had to be examined in court, I expect my gateway could have been exhibited as “prior art” because its “move link banner to bottom” option was arguably a primitive means of “classifying said information according to its importance to a user in accordance with user-selected importance criteria”, and if the patent claims’ only addition is that the browser is on a wireless device, and if this isn’t considered a trivial addition, then I think my first public wireless-specific use was in August 2000 when I helped Enfour set it up for Japanese “i-mode” mobiles, which might have been a couple of months too late for the American patent’s priority date, but I’m not a lawyer. Anyway, my later Web Adjuster project could not infringe those patents without extensive customisation, since it has no built-in facilities to re-order the page server-side, and hopefully this is all history now because I believe the patents expired in 2021 (although the statute of limitations for past infringements does not expire until 2027, so someone might still want this prior art until then).

The gateway works by intercepting your Web browsing in such a way that the gateway computer can sort out the Web pages before you see them. It works with all browsers and operating systems as long as the browser supports forms.  You can adjust it to work the way you want to.

If you do not have a browser that supports forms, or you are browsing the web via an email gateway, you can still use the access gateway if you know how to read HTML forms and generate CGI ‘GET’ URLs. In fact, you can use the gateway to submit a ‘POST’ form using a ‘GET’ URL - the gateway itself really doesn’t care which method is used, and will pass on the request as a ‘POST’ if you set the right parameter.

The gateway can also (sometimes) be used as a rudimentary viewer of such things as Flash and WAP, if you have no other means of displaying them. In the case of WAP, the intention is that print-disabled users can use WAP sites on their normal desktop browsers.

Sites that have run the gateway publically in the past include ACCU.org, the American National Oceanic and Atmospheric Administration (NOAA), the UK Japanese Embassy (Japan 2001 project), and the Japanese Viewer Service at Monash University in Australia, as well as a few machines in Cambridge.
These shut down for various reasons. Monash remained up the longest—until December 2015—but ultimately had to close because too much of their traffic quota was being used up by people treating the gateway as a proxy to bypass Internet filtering at work, and by badly-behaved “web robots” trying to discover email addresses or Web forms for underhanded marketting activities. (Yes, the fact that those spoilsports brought down an innocent public service does bring to mind Ps.58:6a, but let’s not take it personally: the above-mentioned Web Adjuster installations are usually more suitable nowadays anyway.)

The Monash gateway is in fact still available, but at a private address that we’ll give out only to someone who convinces us they *really* need it (explaining why Web Adjuster is not a good enough replacement for them), plus promises never to link to it.

To install the access gateway on your Web server, you need a shell account with sufficient privileges. FTP access is **not** sufficient. If you do not have Unix then you have to install various Unix tools; this has been done but it would be simpler just to get a Unix shell account. Some degree of Unix competence is assumed.

Run `./install.sh`.  This script should work on most Unix systems. It will ask some questions, configure the gateway, compile it and install it.

If compiling the image server on 64-bit, you may have to replace `#ifdef NameCompareType` with `#if 0`

The gateway has an extensions mechanism, which is documented in `extenlib.h`.

Usage
-----

The [online help](help.htm) gives detailed information about its use, and many users will be able to start without having to read the help.

Some people expressed an interest in using the gateway to preprocess their pages (ie. put up pages that have already been processed). This can be used, for example, on sites that teach a language where character substitution is required. I’ve added a hack that lets you do this:

1. Go to the page in the gateway and adjust the options to your liking.

2. Append `&AP=1` (case is important) to the long URL and press Enter. You should now get a version of the page that is suitable for saving as a standalone document.

3. Save it.

You may wish to bookmark the long URL with the `&AP=1`, for when you update the page. If you have several pages that you want the gateway to process, you may wish to write yourself an update script that does all this for each page, for example, one containing commands like:

    lynx -source "http://long-URL-goes-here&AP=1" > page1-preprocessed.html

Wildcards are also possible (using for).

Citation
--------

Silas S Brown & Peter Robinson. A World Wide Web Mediator for Users with Low Vision. CHI 2001 Workshop No. 14 (Seattle, USA).  [PDF format](http://www.ics.forth.gr/proj/at-hci/chi2001/files/brown.pdf)

Legal
-----

All material © Silas S. Brown unless otherwise stated. 
Licensed under the terms of the Apache License, Version 2.0.  There is absolutely no warranty.

The author is not responsible for, and has no authority over, the websites that host the Access Gateway. There is no warranty, either express or implied. It is the user, not the author, that chooses which pages to process, and the author does not endorse (and is not responsible for) any illegal viewing or modification of data, whether the program is functioning properly or not.

Javascript is a trademark of Oracle Corporation in the US. 
Unix is a trademark of The Open Group. 
Any other trademarks I mentioned without realising are trademarks of their respective holders.
