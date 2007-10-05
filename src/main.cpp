/***************************************************************************
 *   Copyright (C) 2005 Thomas Nagy <tnagyemail-mail@yahoo.fr>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation (see COPYING)            *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KDebug>
#include <KHighscore>
#include <KGlobal>

#include "settings.h"
#include "mainwindow.h"

static const char description[] =
I18N_NOOP("KNetWalk, a game for system administrators.");

static const char version[] = "1.0";

int main(int argc, char ** argv)
{
    KAboutData about("knetwalk", 0, ki18n("KNetWalk"), version, ki18n(description),
        KAboutData::License_GPL, ki18n("(C) 2004, 2005 Andi Peredri, ported to KDE by Thomas Nagy\n (C) 2007 Fela Winkelmolen"));
    about.addAuthor( ki18n("Fela Winkelmolen"), ki18n("current maintainer"), "fela.kde@gmail.com");
    about.addAuthor( ki18n("Andi Peredri"), ki18n("original author"), "andi@ukr.net" );
    about.addAuthor( ki18n("Thomas Nagy"), ki18n("KDE port"), "tnagy2^8@yahoo.fr" );

    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("Novice", ki18n( "Start in novice mode" ));
    options.add("Normal", ki18n( "Start in normal mode" ));
    options.add("Expert", ki18n( "Start in expert mode" ));
    options.add("Master", ki18n( "Start in master mode" ));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication application;

    KGlobal::locale()->insertCatalog("libkdegames");

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("Novice")) Settings::setSkill(Settings::EnumSkill::Novice);
    if (args->isSet("Normal")) Settings::setSkill(Settings::EnumSkill::Normal);
    if (args->isSet("Expert")) Settings::setSkill(Settings::EnumSkill::Expert);
    if (args->isSet("Master")) Settings::setSkill(Settings::EnumSkill::Master);

    args->clear();


    MainWindow* window = new MainWindow;
    window->show();

    return application.exec();
}

