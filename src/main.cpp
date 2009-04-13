/*
    Copyright 2005 Thomas Nagy <tnagyemail-mail@yahoo.fr> 
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com> 
  
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KDebug>
#include <KGlobal>

#include "settings.h"
#include "mainwindow.h"

static const char description[] =
I18N_NOOP("KNetWalk, a game for system administrators.");

static const char version[] = "3.0.0";

int main(int argc, char ** argv)
{
    KAboutData about("knetwalk", 0, ki18n("KNetWalk"), version,
        ki18n(description), KAboutData::License_GPL, 
        ki18n("(C) 2004-2005 Andi Peredri, ported to KDE by Thomas Nagy\n"
        "(C) 2007-2008 Fela Winkelmolen"), KLocalizedString(), "http://games.kde.org/knetwalk" );
    
    about.addAuthor(ki18n("Fela Winkelmolen"), 
                    ki18n("current maintainer"),    
                    "fela.kde@gmail.com");
    
    about.addAuthor(ki18n("Andi Peredri"), 
                    ki18n("original author"), 
                    "andi@ukr.net");
    
    about.addAuthor(ki18n("Thomas Nagy"), 
                    ki18n("KDE port"), 
                    "tnagy2^8@yahoo.fr");
                    
    about.addCredit(ki18n("Eugene Trounev"),
                    ki18n("icon design"),
                    "eugene.trounev@gmail.com");

    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("Easy", ki18n("Start with Easy difficulty level"));
    options.add("Medium", ki18n("Start with Medium difficulty level"));
    options.add("Hard", ki18n("Start with Hard difficulty level"));
    options.add("VeryHard", ki18n("Start with Very Hard difficulty level"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication application;
    KGlobal::locale()->insertCatalog("libkdegames");

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("Easy")) {
        Settings::setSkill(Settings::EnumSkill::Easy);
    }
    if (args->isSet("Medium")) {
        Settings::setSkill(Settings::EnumSkill::Medium);
    }
    if (args->isSet("Hard")) {
        Settings::setSkill(Settings::EnumSkill::Hard);
    }
    if (args->isSet("VeryHard")) {
        Settings::setSkill(Settings::EnumSkill::VeryHard);
    }
    args->clear();


    MainWindow* window = new MainWindow;
    window->show();

    return application.exec();
}

