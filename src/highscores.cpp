/***************************************************************************
 *   Copyright (C) 2005 Thomas Nagy                                        *
 *   tnagyemail-mail@yahoo.fr                                                          *
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

#include "highscores.h"

#include <KUrl>
#include <KLocale>
#include <KConfig>
#include <KGlobal>
#include <KConfigGroup>
#include <KGameDifficulty>

#include "defines.h"
#include "settings.h"

namespace KExtHighscore
{
	ExtManager::ExtManager() : Manager(4)
	{
		setScoreType(Normal);
		/*
		setWWHighscores(KUrl( HOMEPAGE ), VERSION);
		setShowStatistics(true);
		*/
		const uint RANGE[16] = { 0, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160 };
		QVector<uint> s;
		for(int i=0;i<16;++i)
			s.append(RANGE[i]);
		setScoreHistogram(s, ScoreNotBound);
		//Item *item = new Item((uint)0, i18n("Clicks"), Qt::AlignRight);
		//addScoreItem("nb_actions", item);
	}

	QString ExtManager::gameTypeLabel(uint gameType, LabelType /*type*/) const
	{
		/*const Level::Data &data = Level::DATA[gameType];
		switch (type) {
			case Icon:
			case Standard: return data.label;
			case I18N:     return i18n(level[gameType]);
			case WW:       return data.wwLabel;
		}
		return QString::null;*/
		return i18n(levels[gameType]);
	}

	void ExtManager::convertLegacy(uint)
	{
		QString group = KGameDifficulty::levelString();

		KConfigGroup cg(KGlobal::config(), group);
		QString name = cg.readEntry("Name", QString());
		if ( name.isNull() ) return;
		if ( name.isEmpty() ) name = i18n("anonymous");
		int score = cg.readEntry("score", 0);
		if ( score<=0 ) return;
		Score s(Won);
		s.setScore(score);
		s.setData("name", name);
		submitLegacyScore(s);
	}

	bool ExtManager::isStrictlyLess(const Score &s1, const Score &s2) const
	{
		if ( s1.score()==s2.score() )
			// when time is same, favor more clicks (it means auto-reveal
			// didn't help so much):
			return true; //s1.data("nb_actions").toUInt()<s2.data("nb_actions").toUInt();
		return ! Manager::isStrictlyLess(s1, s2);
	}
}

