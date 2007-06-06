/***************************************************************************
 *   Copyright (C) 2005, Thomas Nagy                                       *
 *   tnagyemail-mail@yahoo@fr                                              *
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
	{ "Novice", I18N_NOOP( "Start in novice mode" ), 0 },
	{ "Normal", I18N_NOOP( "Start in normal mode" ), 0 },
	{ "Expert", I18N_NOOP( "Start in expert mode" ), 0 },
	{ "Master", I18N_NOOP( "Start in master mode" ), 0 },
	KCmdLineLastOption
};

int main(int argc, char ** argv)
{
	KAboutData about("knetwalk", I18N_NOOP("knetwalk"), version, description,
		KAboutData::License_GPL, I18N_NOOP("(C) 2004, 2005 Andi Peredri, ported to KDE by Thomas Nagy"), 0, 
		"tnagyemail-mail@yahoo.fr");
	about.addAuthor( "Andi Peredri", 0, "andi@ukr.net" );
	about.addAuthor( "Thomas Nagy", 0, "tnagy2^8@yahoo.fr" );

	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication app;

	KGlobal::locale()->insertCatalogue("libkdegames");

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (args->isSet("Novice")) Settings::setSkill(Settings::EnumSkill::Novice);
	if (args->isSet("Normal")) Settings::setSkill(Settings::EnumSkill::Normal);
	if (args->isSet("Expert")) Settings::setSkill(Settings::EnumSkill::Expert);
	if (args->isSet("Master")) Settings::setSkill(Settings::EnumSkill::Master);
	args->clear();	
	
	KHighscore::init("knetwalk");
	KExtHighscore::ExtManager manager;


	MainWindow* wi = new MainWindow;
	app.setMainWidget(wi);
	wi->show();

	return app.exec();
}

