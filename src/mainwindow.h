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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kapplication.h>
#include <kmainwindow.h>

class Cell;
class QAction;
class QSound;
class QLCDNumber;
class KSelectAction;

class MainWindow : public KMainWindow
{
	Q_OBJECT
	public:
		MainWindow(QWidget *parent=0, const char* name=0, WFlags fl=0);
	protected:
		virtual void closeEvent(QCloseEvent*);
	private:
		//enum Skill { Novice, Normal, Expert, Master };
		enum BoardSize
		{
			NoviceBoardSize = 5,
			NormalBoardSize = 7,
			ExpertBoardSize = 9,
			MasterBoardSize = 9
		};
		enum
		{
			NumHighscores   = 10,
			MinimumNumCells = 20
		};
		typedef QValueList<Cell*> CellList;
	public slots:
		void  slotNewGame();
		void  newGame(int);
		
		void  lClicked(int index);
		void  rClicked(int index);
		void  mClicked(int index);
		
		void  showHighscores();
		void  configureHighscores();
		void  configureNotifications();
		
	private:
		Cell* uCell(Cell* cell) const;
		Cell* dCell(Cell* cell) const;
		Cell* lCell(Cell* cell) const;
		Cell* rCell(Cell* cell) const;
		bool  isGameOver();
		bool  startBrowser(const QString& url);
		bool  updateConnections();
		void  blink(int index);
		void  rotate(int index, bool toleft);
		void  addRandomDir(CellList& list);
		void  dialog(const QString& caption, const QString& text);
	private:
		bool        wrapped;
		Cell*       root;
		Cell*       board[MasterBoardSize * MasterBoardSize];
		
		QSound*     clicksound;
		QSound*     connectsound;
		QSound*     startsound;
		QSound*     turnsound;
		QSound*     winsound;
		
		QString     username;
		QString     soundpath;
		QAction*    soundaction;
		QStringList highscores;
		QLCDNumber* lcd;
		QPopupMenu* gamemenu;
		QPopupMenu* skillmenu;

		int m_clickcount;
		KSelectAction* m_levels;
};

#endif // MAINWINDOW_H
