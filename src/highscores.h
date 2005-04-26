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

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <kexthighscore.h>
#include <kdemacros.h>

namespace KExtHighscore
{

	class KDE_EXPORT ExtManager : public Manager
	{
		public:
			ExtManager();

		private:
			QString gameTypeLabel(uint gameTye, LabelType) const;
			void convertLegacy(uint gameType);
			bool isStrictlyLess(const Score &s1, const Score &s2) const;
	};
}

#endif
