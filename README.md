This is a single player video strip poker game.

You play serious Texas Hold'em against 1-5 non-player characters. The purpose of serious strip poker is to win the game. Don't get distracted by the undressing girls.


--- Nerve-racking bugs

On Windows the videos are too faulty.

There are jumps and flashes. The HD videos have black areas. The clip-editor doesn't show the correct frame. Previev images extrected from the videos are messed up. And sometimes the video don't start. You have to resize the window.

I'm told, the program just needs a different video-library. But this is not on my top priority list, I am using Linux. Are there Windows programmers, who want to play serious strip poker?

Version 0.0.0  Use at your own risk.


---- Installing the program

Up to now, there is no distribution. You have to compile it yourself.

Compiling has been testet on Ubuntu Linux and Windows7. Trolltech claims, you can also compile it on a Mac.


---- Getting the videos

This distribution does not include any videos :-(

But you can use the girls, you bought from Torquemada.
This company made 4 keys public, so you can download 4 girls for free.

If you don't know how to convert the proprietary files, ask your favorite search engine for vsp2movie3.


---- Getting the timestamp files

I don't know how to extract the clip markers from the original files. If you figure it out, please let me know.

So you have to assemble a text file with timestamps and clip descriptions. This program contains a clip-editor to build the timestamp files for your girls yourself.

You find some timestamp files in a different Github repository. https://github.com/serious-44/ssp-data

You want to share your timestamp files? In my opinion, the most easy way is to fork and upload to Github


---- Getting the preview images

After selecting the girls, you will see a message "no preview image".
- Click on the "Generate Preview" button.
- Or put a jpg into the directory next to the timestamp file.
- Or ignore the message.


---- Playing Poker

For each opponent put the 3 files into the same directory.

- tory.avi
- tory.jpg
- tory.ts

Start the program. Select your opponents - Choose a *.tsc compilation or some *.ts timestamp files. And click on "start".

Basically this is a modification of the PokerTH game. Please consult the user guide from https://www.pokerth.net/ .


---- Cheating

The "cheat" button on the statistics tab calulates the probability to win this hand. But it doesn't know the cards that have not yet been dealed.

In the long run, you will win the game. And it doesn't spoil the fun.


---- Edit timestamp files

Click the "Edit Video" button.
You will get a new window. It contains a tab with a help page.

The recent videos are easier. The gestures are exaggerated. And you will find enough clips for all categories.


---- Compiling the source code

You need the free QtCreator IDE.

This page describes, how to install IDE and compiler on Windows.
https://www.ics.com/blog/getting-started-qt-and-qt-creator-windows

If you don't want to install the git tools, just click die green Button on https://github.com/serious-44/ssp and select "Download Zip"

This page describes how to install the git client.
https://stackoverflow.com/questions/27708127/setup-git-in-qt-creator

On Ubuntu Linux, intalling QtCreator is more complex.

The qtcreator ubuntu package is too old for a QT5 application. Trolltech changed ther download procedure, so the tips on askubuntu don't work any more.
Use http://download.qt.io/official_releases/qtcreator/4.3/4.3.0/qt-creator-opensource-linux-x86_64-4.3.0.run

It doesn't install the development libraries -- use the package manager of your distribution.
(The package names are messed up. https://askubuntu.com/questions/508503/whats-the-development-package-for-qt5-in-14-04)

This video describes how to use the git command line tools wit qtcreator and github.
https://www.youtube.com/watch?v=qlPSV7Bs3rE


---- Modifying the source code

This is Github. Just fork and let me know, whether I am allowed to pull your improvements.


---- Discussions

Probably you'll find all those who are interested in this game where you found vsp2movie3. So I suggest you discuss it in this forum.

Please report bugs and ideas for improvements on the github issue tracker.
https://github.com/serious-44/ssp/issues

