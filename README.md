This is a single player video strip poker game.

You play serious Texas Hold'em against 1-5 non-player characters. The purpose of serious strip poker is to win the game. Don't get distracted by the undressing girls.

Version 0.0.0  Use at your own risk.


---- Getting the videos

This distribution does not include any videos :-(

But you can use the girls, you bought from Torquemada.
This company made 4 keys public, so you can download 4 girls for free.

If don't know how to convert the proprietary files, ask your favorite search engine for vsp2movie3.


---- Getting the timestamp files

I don't know how to extract the clip markers from the original files. If you figure it out, please let me know.

So you have to assemble a text file with timestamps and clip descriptions. This program contains a clip-editor to build the timestamp files for your girls yourself.

May be, you find some timestamp files where you found the vsp2movie3 program.


---- Getting the preview images

After selecting the girls, you will see a message "no preview image".
- Click on the "Generate Preview" button.
- Or put a jpg into the directory next to the timestamp file.
- Or ignore the message.


---- Installing the program

There is a Windwos binary. Up to now, there is no installer. Just download the zip. Unpack it and start ssp.exe from your file explorer.

To run this program on Linux, you have to compile it yourself. Trolltech claims, you can also compile it in a Mac.


---- Playing Poker

For each opponent put the 3 files into the same directory.

- tory.avi
- tory.jpg
- tory.ts

Start the program, click the "number of opponents" radio-button.
Select your opponents.
-  Choose a *.tsc compilation or some *.ts timestamp files.
-  Or play serious Texas Hold'en without the distracting girls.
Click on "start".

Basically this is a modification of the PokerTH game. Please consult the user guide from https://www.pokerth.net/ .


---- Edit timestamp files

Click the "Edit Video" button.
You will get a new window. It contains a tab with a help page.

The new videos are easier. The gestures are exaggerated. And you will find enough clips for all categories.


---- Compiling the source code

You need the free QtCreator IDE.

This page describes, how to install IDE and compiler on Windows.
https://www.ics.com/blog/getting-started-qt-and-qt-creator-windows

If you don't want to install the git tools, just click die green Button on https://github.com/serious-44/ssp and select "Download Zip"

This page describes how to install the git client.
https://stackoverflow.com/questions/27708127/setup-git-in-qt-creator

On Ubuntu Linux, things get more difficult.

The qtdesigner ubuntu package is too old for a QT5 application. Trolltech changed ther download procedure, so the tips on askubuntu don't work any more.
Use http://download.qt.io/official_releases/qtcreator/4.3/4.3.0/qt-creator-opensource-linux-x86_64-4.3.0.run

It doesn't install the development libraries -- use the package manager of your distribution.
(The package names are messed up. https://askubuntu.com/questions/508503/whats-the-development-package-for-qt5-in-14-04)

This video describes how to use github with the command line tool.
https://www.youtube.com/watch?v=qlPSV7Bs3rE


---- Modifying the source code

This is Github. Just fork and let me know, whether I am allowed to pull your improvements.


---- Discussions

Probably, you will find all people interested in this program where you found vsp2movie3. So I suggest to discuss this program also at this place.

You want to share your timestamp files? Anybody volunteer to organize and disseminate the collection?

Please report bugs on github issue tracker.
https://github.com/serious-44/ssp/issues

