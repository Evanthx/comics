Version 3 beta...most info below is correct, essentially just dup all this into a directory together and run as below.
On with v2 docs...this will be updated as I finish v3.

FIRST - for the windows version you need to have Winsock 2.0!  You can get it here: 
http://www.microsoft.com/windows95/downloads/contents/wuadmintools/s_wunetworkingtools/w95sockets2/default.asp
Install that and hopefully the program will work properly.  The program will tell you if you need to do this or not.

SECOND - if you have any comics you want to add, feature requests, or want to tell me about a comic that isn't working, email me at evan@evan.org.  I'll get back to you pretty quickly, the length of time before I implement the fix or request varies.  If you get impatient, please remember that I'm doing this in my spare time and giving it away for free.  

LAST - if you make changes to the ini files, they will be wiped out when you get the next ini file update.  This is because your current ini file gets merged with my updated ini file, and I can't tell which entries you changed - the program just sees that there is a difference, and updates it according to the updated ini file.  So if you make changes, SEND THEM TO ME!  Then I can put them into the updated ini file, and it'll work for everyone.  Note that changes you make to the order are kept...which is why new comics end up at the bottom of the list, out of alphabetical order.

---------

This is the version 3 of my comics application.  Hopefully it'll be really easy to use.  Version 3 is meant to make it easier to maintain the comics lists.  See the bottom for full instructions.

Unzip the two executables (ComicSetup and Comics) into a directory together.  You might want to create a shortcut to comics on your desktop - drag it to your desktop with your RIGHT mouse button and choose "Create Shortcut Here".

Useage is simple.  Run ComicSetup.  Choose any comics you want to read.  Click OK.  Then run Comics.  That will run for a second (it has to fetch some information from the web) and then will launch your web browser with the days comics.

If you use a proxy server, you will need to give the comics executable arguments.  The first argument is the proxy server name, and the second is the port number.  I've created a script to do this for you.  Simply edit comicproxy to show your proxy server name and port number, and run it instead of comics.  You can get your proxy settings from your web browser.

Check for any updates at http://www.evan.org/Software/Comics.html.  Source code for this is available there as well.   There are versions and source code for Windows and BeOS.

The rest of the ini file is for those who want to know what's happening in the ini files.  If you want to add a comic or fix a broken link yourself, read on.

Maintaining the ini files
-------------------------
There are two ini files - ComicSetup.ini and Comics.ini.  The handing for them isn't too robust yet, so if you break them, tough.  Back them up before you tinker with them so a restoration is easy.  Don't leave any empty lines.  Don't forget your | tags.  I'll beef up my error handling at some point, until then if you hose your ini files I'm not going to predict what the program will do.
IF YOU MAKE ANY SUCCESSFUL CHANGES TO THESE FILES, PLEASE E-MAIL THEM TO ME AT EVAN@EVAN.ORG SO WE CAN SHARE THE FIX WITH OTHERS!  Besides, if you don't, then the next time you get an update file to fix the changes I have, it will stomp your changes.  Not the new order you set up, but any html changes you make will get undone.

ComicSetup.ini
Note that if you make any changes to this file, you must re-run ComicSetup to have them take effect.  This file defines each comic the program knows about, tells the homepage of the comic, and defines where the image sits.  It also says whether you view this comic or not.

First, and most simple to change, the order of comics in this file tells the order of the buttons turning the comic on and off, and the order of the comics you view.  If you move your favorite comic to the top, it will be the first button and the first comic you view.  Ever wonder why Sherman's Lagoon is first, and there are several more out of alphabetical order?  Those are my favorites, and I want to read them first - so that's the way I wrote the original program, and that's the way I ordered the new ini file.  Now you have the same ability to order comics.

So let's look at a example line and explain what's going on:

Sherman's Lagoon|<p><a href="http://www.slagoon.com">Sherman's Lagoon</a></p><p><img src="http://www.slagoon.com/dailies/SL0YEAR.0MONTH.0DAY.gif"></p>|N

There are three fields in it, separated by vertical bars (|).
The first field is the name of the comic.  That's simple enough.
The last field is either Y or N, saying whether you want this comic or not (yes or no.)
The middle field is a bit of HTML that adds a link to the comic home page, and then defines where the comic image is.  It's easiest to alter it by cutting and pasting into the parts set off by quotations.  The first one of those, "http://www.slagoon.com", tells us the home page of the comic.  That bit, put into a browser without quotes, takes us to the comic's home page.  The text after that (Sherman's Lagoon, the second instance of that text in the line above) is the text bit that you click on in the comics page to go to the home page.  It's usually the same as the name of the comic, but the name might be altered a bit to fit the button in the setup program.

Finally, we have the line that defines where the comic itself sits.  
img src="http://www.slagoon.com/dailies/SL0YEAR.0MONTH.0DAY.gif"
The part you would alter is http://www.slagoon.com/dailies/SL0YEAR.0MONTH.0DAY.gif
To alter it, start by getting the comic picture by itself in the browser.  No web page, just the comic image.  You will have to look at the HTML page containing the comic to find that information.  Paste that line between the quotes, so you have:
img src="YOUR PASTED BIT"
Now figure out which part of the line changes from day to day.  That bit is going to have to be replaced with a special tag.  The tag is defined in comics.ini, which is explained next.  
http://www.slagoon.com/dailies/SL00.12.03.gif would show me a single Sherman strip for Dec. 3, 2000.  Each day is in the same place, but the actual file name (SL0012.03.gif here) changes.  So I have to place tags for the date.  I replaced 00 with 0YEAR, 12 with 0MONTH, and 03 with ODAY.  Try to reuse existing tags.  It simplifies things.

Some comics use a code that isn't related to the date and can't be figured out simply.  For those, you have to actually download the web page for the comic and parse out the code.  Fortunately, companies use the same code for each comic.  So Dilbert and Robotman, for example, both have a code like 2000112180201.  If I download and parse out one, I have the other.  It's much faster to do that than it is to fetch each separate web page.  So I can get the code for Robotman, and using that I can get all of United Media's strips.  The downside is that if they don't update Robotman one day (it happens) then you won't get any other current United Media strips either.

----------------------
Comics.ini
This file defines those codes.  There are two parts to it, separated by a URL| tag.
The first part defines date base codes, as in my Sherman example.  Let's look at one:
0DAY|0|%d
The first part is the tag that matches whatever is in ComicSetup.ini.  Big warning - say the next tag is 0DAY2.  Looks good, but the first tag will find and replace the 0DAY part.  Then you'll have that replacement and a leftover 2.  So keep that in mind when naming your tag.
The second part says how many days ago this tag is for.  Some comics use current dates.  Some use dates a week ago, and some use two week old dates.  0DAY says to take a day 0 days ago.
The last tag is as defined by the C++ strftime call:

%d - Day of month as decimal number (01 to 31)
%m - month as decimal number (01 to 12)
%y - Year without century, as decimal number (00 to 99)
%Y - Year with century, as decimal number

There are more, but those are the only four I've ever seen used.
So let's look at this example:
ONE_WEEK_CODE|7|%y%m%d
This is for the tag ONE_WEEK_CODE.  It uses the date from 7 days ago.  It will place a two digit year, then the month, then the day of the year all together:
001104 for November 4, 2000.

Two more things.  Some tags start with SUNDAY, and other start with WEEKDAY.  Those are special.  They really refer to Sunday and non-Sunday, Saturdays fall under WEEKDAY.  Essentially, some comics do things differently on Sundays.  They will use a jpg on Sundays and a gif on Weekdays, or append a G to Sunday strips, that sort of thing.  So any replacement tag starting with SUNDAY in only applied on Sundays.  Ones starting with WEEKDAY are applied on other days.
So look at these two examples:
comiclocation/name.SUNDAYJPGWEEKDAYGIF
which use these two definitions:
SUNDAYJPG|0|jpg
WEEKDAYGIF|0|gif

On Sunday, we look for name.jpg, and on other days we look for name.gif.  On Sunday, WEEKDAYGIF is simply removed.

Now, past the URL| tag, we handle files that need downloads.
Here's the line to get the United Media tag:
UMEDIA|www.unitedmedia.com|/comics/robotman/index.html|/comics/robotman/archive/images/robotman

First is UMEDIA.  This is the tag name.
Next is www.unitedmedia.com.  This is the domain name.  Note no slashes, no http://.  Just the domain name.
Third is the page to get in that domain, /comics/robotman/index.html in this case.  If there is no page,you just want the default page for that domain, just put a / here.  Otherwise, this is the page containing the tag you want.
The fourth and last bit is the bit of text to search for in order to find the tag.  Everything AFTER your search bit and BEFORE a "." is taken.  The dot is the extension for the file name, and since it's always there is seemed appropriate to use it for a ending delimiter.  In this case, I found the comic name by clicking on "save as" when the image was on my screen.  Copy that into your clipboard.  View source in your browser, and search for that token.  Paste enough text before it to be unique into this field, and you should be set.

So in our example, we go to the domain www.unitedmedia.com, fetch the page /comics/robotman/index.html, and search for the string /comics/robotman/archive/images/robotman.  Everything AFTER that string and BEFORE the first "." that is found is used for the token UMEDIA.
Try to reuse these as much as possible.  They are slow - it would be MUCH faster for you to reuse a token than to do an extra unneeded fetch from the internet.  It makes the program much faster, even if it's at the expense of not handling comics that aren't daily perfectly.  If those bother you, then you can alter them to do a fetch every day - I just decided I'd rather see a broken link image than wait for useless fetches.

E-mail me at evan@evan.org with any questions or suggestions on making this explanation better.


