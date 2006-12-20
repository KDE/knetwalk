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

#include <kurl.h>
#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>

#include "defines.h"
#include "highscores.h"
#include "settings.h"

namespace KExtHighscore
{
	ExtManager::ExtManager() : Manager(4)
	{
		setScoreType(Normal);
		/*
		setWWHighscores(KURL( HOMEPAGE ), VERSION);
		setShowStatistics(true);
		*/
		const uint RANGE[16] = { 0, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160 };
		QMemArray<uint> s;
		s.duplicate(RANGE, 16);
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

	void ExtManager::convertLegacy(uint gameType)
	{
		QString group;
		switch (gameType)
		{
			case Settings::EnumSkill::Novice: group = "Novice level"; break;
			case Settings::EnumSkill::Normal: group = "Normal level"; break;
			case Settings::EnumSkill::Expert: group = "Expert level"; break;
			case Settings::EnumSkill::Master: group = "Master level"; break;
			default: Q_ASSERT(false);
		}

		KConfigGroupSaver cg(kapp->config(), group);
		QString name = cg.config()->readEntry("Name", QString::null);
		if ( name.isNull() ) return;
		if ( name.isEmpty() ) name = i18n("anonymous");
		int score = cg.config()->readNumEntry("score", 0);
		if ( score<=0 ) return;
		Score s(Won);
		s.setScore(score);
		s.setData("name", name);
		submitLegacyScore(s);
	}

	bool ExtManager::isStrictlyLess(const Score &s1, const Score &s2) const
	{
		if ( s1.score()==s2.score() )
			// when time is same, favour more clicks (it means auto-reveal
			// didn't help so much):
			return true; //s1.data("nb_actions").toUInt()<s2.data("nb_actions").toUInt();
		return ! Manager::isStrictlyLess(s1, s2);
	}
}

