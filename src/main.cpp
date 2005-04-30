/***************************************************************************
 *   Copyright (C) 2004, 2005 Andi Peredri                                 *
 *   andi@ukr.net                                                          *
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

//#include <qtextcodec.h>
//#include <qtranslator.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>
#include <khighscore.h>

#include "highscores.h"
#include "settings.h"
#include "mainwindow.h"

static const char description[] =
    I18N_NOOP("KNetWalk, a game for system administrators.");

static const char version[] = "1.0";

static KCmdLineOptions options[] =
{
        { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
        KCmdLineLastOption
};

int main(int argc, char ** argv)
{
        KAboutData about("knetwalk", I18N_NOOP("knetwalk"), version, description,
                        KAboutData::License_GPL, "(C) 2004, 2005 Andi Peredri, ported to KDE by Thomas Nagy", 0, "tnagyemail-mail@yahoo.fr");
	about.addAuthor( "Andi Peredri", 0, "andi@ukr.net" );
        about.addAuthor( "Thomas Nagy", 0, "tnagy2^8@yahoo.fr" );

        KCmdLineArgs::init(argc, argv, &about);
        KCmdLineArgs::addCmdLineOptions(options);

	KHighscore::init("knetwalk");
	
        KApplication app;
	
	if ((argc > 1) && QString(argv[1]) == "-help")
	{
		kdWarning("Usage: knetwalk [OPTIONS]\n"
				"KNetWalk is a game for system administrators.\n"
				"  -novice     set the skill Novice\n"
				"  -amateur    set the skill Amateur\n"
				"  -expert     set the skill Expert\n"
				"  -master     set the skill Master\n"
				"  -help       display this help and exit\n");
		return 1;
	}

	KGlobal::locale()->insertCatalogue("libkdegames");
	
	//KHighscore::init("knetwalk");
	KExtHighscore::ExtManager manager;
	
	MainWindow* wi = new MainWindow;
	app.setMainWidget(wi);
	wi->show();
	
	if (kapp->argc() > 1)
	{
		int skill=Settings::EnumSkill::Novice;
		for(int i = 1; i < kapp->argc(); i++)
		{
			QString argument = kapp->argv()[i];
			if(argument == "-novice")       skill = Settings::EnumSkill::Novice;
			else if(argument == "-amateur") skill = Settings::EnumSkill::Normal;
			else if(argument == "-expert")  skill = Settings::EnumSkill::Expert;
			else if(argument == "-master")  skill = Settings::EnumSkill::Master;
			else kdWarning()<<"Unknown option: '"<< argument << "'. Try -help."<<endl;
		}
		wi->newGame(skill);
	}

	return app.exec();
}

