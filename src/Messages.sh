#! /bin/sh
$EXTRACTRC *.rc *.kcfg *.ui >> rc.cpp
$XGETTEXT *.cpp qml/*.qml -o $podir/knetwalk.pot
