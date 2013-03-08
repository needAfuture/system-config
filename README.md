
# It is free

This is an Android dictionary App. You can visit [Google Play](https://play.google.com/store/apps/details?id=com.baohaojun.crossdict) to
install it, or if your Android phone can't use Google Play, then you
can download it from [Sina Vdisk](http://vdisk.weibo.com/s/skNbH) (need sign up).

It is free software.

The dictionary data that comes with this App is extracted from [GCIDE](ftp://ftp.gnu.org/gnu/gcide/),
which is a GNU project and therefor also free. I have fixed some bugs
in the [dico](http://puszcza.gnu.org.ua/software/dico/) project and used it to extract the dictionary data, and
[here](https://github.com/baohaojun/dico) it is my changes.

But you can switch the dictionary data if you have access to another
one. For example, the American Heritage Dictionary's data can be used,
if it can be changed to CrossDict's data format.

# It is geeky

I have made some features that I think every dictionary should have, for e.g., 

-   Search the definitions
    
    For instance, you can search which words has used the word "hello"
    in their definition. This is why it is named CrossDict.

-   Search with regexp
    
    For instance, you can search with "let$" and found out all those
    small things which ends with "let". This is very useful as it allow
    you check words with similar postfix/prefix/root.

-   Context menu
    
    While in the dictionary, you can long press on a word, and a pop-up
    menu will show, which allows you to check the various things about
    this word. Thank [this project on github](https://github.com/btate/BTAndroidWebViewSelection) for this feature, and if
    you haven't noticed, the CrossDict project is a [fork](https://github.com/btate/BTAndroidWebViewSelection/network) of that one.

# It will be geekier

I am working on these features:

-   Support more devices
    
    Currently only Android 4.0 or above is supported.

-   Support word capture/history better
    
    Using the social sharing feature that is built in Android, another
    App (such as fbreader) can "share" a word with CrossDict, which is
    of course meant to look that word up or capture it for later study.

-   Separate the dictionary data with the App
    
    Or else the App will be too large and difficult to install/upgrade.

# Please support

If you used and liked CrossDict and want to support my work on it,
please click [here](http://baohaojun.github.com/donate.html) to donate.
