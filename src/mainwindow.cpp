/***************************************************************************
 *   Copyright (C) 2004, 2005 Andi Peredri <andi@ukr.net                   *
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

#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QEventLoop>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QToolButton>
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>

#include <QCloseEvent>

#include <KGlobal>
#include <KIconLoader>
#include <KLocale>
#include <KScoreDialog>
#include <KHighscore>
#include <KStandardAction>
#include <KAction>
#include <KActionCollection>
#include <KStandardGameAction>
#include <KStatusBar>
#include <KNotification>
#include <KNotifyConfigWidget>
#include <KExtHighscore>
#include <KStandardDirs>
#include <KSelectAction>

#include <time.h>

#include "settings.h"
#include "cell.h"

static QMap<Cell::Dirs, Cell::Dirs> contrdirs;

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
	m_clickcount = 0;

	contrdirs[Cell::U] = Cell::D;
	contrdirs[Cell::R] = Cell::L;
	contrdirs[Cell::D] = Cell::U;
	contrdirs[Cell::L] = Cell::R;

	// Game
	KStandardGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection());
	KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
	KStandardGameAction::quit(this, SLOT(close()), this);
	
	// Preferences
	KStandardGameAction::configureHighscores(this, SLOT(configureHighscores()), actionCollection());
	KStandardAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());

	m_levels = KStandardGameAction::chooseGameType(0, 0, actionCollection());
	QStringList lst;
	lst += i18n("Novice");
	lst += i18n("Normal");
	lst += i18n("Expert");
	lst += i18n("Master");
	m_levels->setItems(lst);

	//setFixedSize(minimumSizeHint());

	statusBar()->insertItem("abcdefghijklmnopqrst: 0  ",1);
	setAutoSaveSettings();
	createGUI();
	connect(m_levels, SIGNAL(triggered(int)), this, SLOT(newGame(int)));


	this->setWhatsThis( i18n("<h3>Rules of Game</h3>"
			"<p>You are the system administrator and your goal"
			" is to connect each computer to the central server."
			"<p>Click the right mouse's button for turning the cable"
			" in a clockwise direction, and left mouse's button"
			" for turning the cable in a counter-clockwise direction."
			"<p>Start the LAN with as few turns as possible!"));

	//const int cellsize = KGlobal::iconLoader()->loadIcon("knetwalk/background.png", K3Icon::User, 32).width();
	const int cellsize = 32;
	//const int gridsize = cellsize * MasterBoardSize + 2;

	QFrame* frame = new QFrame(this);
	frame->setFrameStyle(QFrame::NoFrame);
	//frame->setFixedSize(gridsize, gridsize);
	QGridLayout* gridLay = new QGridLayout(frame);
	gridLay->setMargin(0);
	gridLay->setSpacing(0);
	setCentralWidget(frame);

	Cell::initPixmaps();
	for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	{
		board[i] = new Cell(frame, i);
		gridLay->addWidget(board[i], i / MasterBoardSize, i % MasterBoardSize);
		board[i]->resize(cellsize, cellsize);
		connect(board[i], SIGNAL(lClicked(int)), SLOT(lClicked(int)));
		connect(board[i], SIGNAL(rClicked(int)), SLOT(rClicked(int)));
		connect(board[i], SIGNAL(mClicked(int)), SLOT(mClicked(int)));
	}
	srand(time(0));

	pixmapCache = new QPixmap(frame->size());
	m_invalidCache = true;
	m_background.load( KStandardDirs::locate( "data","knetwalk/all.svgz" ) );

	slotNewGame();
}

MainWindow::~MainWindow()
{
	delete pixmapCache;
}

void MainWindow::configureHighscores()
{
	KExtHighscore::configure(this);
}

void MainWindow::showHighscores()
{
	KExtHighscore::show(this);
}

void MainWindow::slotNewGame()
{
	newGame( Settings::skill() );
}

void MainWindow::newGame(int sk)
{
	if (sk==Settings::EnumSkill::Novice || sk==Settings::EnumSkill::Normal
			|| sk==Settings::EnumSkill::Expert || sk==Settings::EnumSkill::Master)
	{
		Settings::setSkill(sk);
	}

	if(Settings::skill() == Settings::EnumSkill::Master) wrapped = true;
	else wrapped = false;

	KExtHighscore::setGameType(Settings::skill());

	Settings::writeConfig();

	m_clickcount = 0;
	QString clicks = i18n("Click: %1",m_clickcount);
	statusBar()->changeItem(clicks,1);

        KNotification::event( "startsound", i18n("New Game") );

	for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	{
		board[i]->setDirs(Cell::None);
		board[i]->setConnected(false);
		board[i]->setRoot(false);
		board[i]->setLocked(false);
	}

	const int size = (Settings::skill() == Settings::EnumSkill::Novice) ? NoviceBoardSize :
		(Settings::skill() == Settings::EnumSkill::Normal) ? NormalBoardSize :
		(Settings::skill() == Settings::EnumSkill::Expert) ? ExpertBoardSize : MasterBoardSize;

	const int start = (MasterBoardSize - size) / 2;
	const int rootrow = rand() % size;
	const int rootcol = rand() % size;

	root = board[(start + rootrow) * MasterBoardSize + start + rootcol];
	root->setConnected(true);
	root->setRoot(true);

	while(true)
	{
		for(int row = start; row < start + size; row++)
			for(int col = start; col < start + size; col++)
				board[row * MasterBoardSize + col]->setDirs(Cell::Free);

		CellList list;
		list.append(root);
		if(rand() % 2) addRandomDir(list);

		while(!list.isEmpty())
		{
			if(rand() % 2)
			{
				addRandomDir(list);
				if(rand() % 2) addRandomDir(list);
				list.erase(list.begin());
			}
			else
			{
				list.append(list.first());
				list.erase(list.begin());
			}
		}

		int cells = 0;
		for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
		{
			Cell::Dirs d = board[i]->dirs();
			if((d != Cell::Free) && (d != Cell::None)) cells++;
		}
		if(cells >= MinimumNumCells) break;
	}

	for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
		board[i]->rotate((rand() % 4) * 90);
	updateConnections();
}

bool MainWindow::updateConnections()
{
	bool newconnection[MasterBoardSize * MasterBoardSize];
	for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
		newconnection[i] = false;

	CellList list;
	if(!root->isRotated())
	{
		newconnection[root->index()] = true;
		list.append(root);
	}
	while(!list.isEmpty())
	{
		Cell* cell = list.first();
		Cell* ucell = uCell(cell);
		Cell* rcell = rCell(cell);
		Cell* dcell = dCell(cell);
		Cell* lcell = lCell(cell);

		if((cell->dirs() & Cell::U) && ucell && (ucell->dirs() & Cell::D) &&
				!newconnection[ucell->index()] && !ucell->isRotated())
		{
			newconnection[ucell->index()] = true;
			list.append(ucell);
		}
		if((cell->dirs() & Cell::R) && rcell && (rcell->dirs() & Cell::L) &&
				!newconnection[rcell->index()] && !rcell->isRotated())
		{
			newconnection[rcell->index()] = true;
			list.append(rcell);
		}
		if((cell->dirs() & Cell::D) && dcell && (dcell->dirs() & Cell::U) &&
				!newconnection[dcell->index()] && !dcell->isRotated())
		{
			newconnection[dcell->index()] = true;
			list.append(dcell);
		}
		if((cell->dirs() & Cell::L) && lcell && (lcell->dirs() & Cell::R) &&
				!newconnection[lcell->index()] && !lcell->isRotated())
		{
			newconnection[lcell->index()] = true;
			list.append(lcell);
		}
		list.erase(list.begin());
	}

	bool isnewconnection = false;
	for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	{
		if(!board[i]->isConnected() && newconnection[i])
			isnewconnection = true;
		board[i]->setConnected(newconnection[i]);
	}
	return isnewconnection;
}

void MainWindow::addRandomDir(CellList& list)
{
	Cell* cell = list.first();
	Cell* ucell = uCell(cell);
	Cell* rcell = rCell(cell);
	Cell* dcell = dCell(cell);
	Cell* lcell = lCell(cell);

	typedef QMap<Cell::Dirs, Cell*> CellMap;
	CellMap freecells;

	if(ucell && ucell->dirs() == Cell::Free) freecells[Cell::U] = ucell;
	if(rcell && rcell->dirs() == Cell::Free) freecells[Cell::R] = rcell;
	if(dcell && dcell->dirs() == Cell::Free) freecells[Cell::D] = dcell;
	if(lcell && lcell->dirs() == Cell::Free) freecells[Cell::L] = lcell;
	if(freecells.isEmpty()) return;

	CellMap::ConstIterator it = freecells.constBegin();
	for(int i = rand() % freecells.count(); i > 0; --i) ++it;

	cell->setDirs(Cell::Dirs(cell->dirs() | it.key()));
	it.value()->setDirs(contrdirs[it.key()]);
	list.append(it.value());
}

Cell* MainWindow::uCell(Cell* cell) const
{
	if(cell->index() >= MasterBoardSize)
		return board[cell->index() - MasterBoardSize];
	else if(wrapped)
		return board[MasterBoardSize * (MasterBoardSize - 1) + cell->index()];
	else return 0;
}

Cell* MainWindow::dCell(Cell* cell) const
{
	if(cell->index() < MasterBoardSize * (MasterBoardSize - 1))
		return board[cell->index() + MasterBoardSize];
	else if(wrapped)
		return board[cell->index() - MasterBoardSize * (MasterBoardSize - 1)];
	else return 0;
}

Cell* MainWindow::lCell(Cell* cell) const
{
	if(cell->index() % MasterBoardSize > 0)
		return board[cell->index() - 1];
	else if(wrapped)
		return board[cell->index() - 1 + MasterBoardSize];
	else return 0;
}

Cell* MainWindow::rCell(Cell* cell) const
{
	if(cell->index() % MasterBoardSize < MasterBoardSize - 1)
		return board[cell->index() + 1];
	else if(wrapped)
		return board[cell->index() + 1 - MasterBoardSize];
	else return 0;
}

void MainWindow::lClicked(int index)
{
	rotate(index, true);
}

void MainWindow::rClicked(int index)
{
	rotate(index, false);
}

void MainWindow::mClicked(int index)
{
	board[index]->setLocked( !board[index]->isLocked() );
}

void MainWindow::rotate(int index, bool toleft)
{
	const Cell::Dirs d = board[index]->dirs();
	if((d == Cell::Free) || (d == Cell::None) || isGameOver() || board[index]->isLocked() )
	{
		KNotification::event( "clicksound" );
		blink(index);
	}
	else
	{
		KNotification::event( "turnsound" );

		board[index]->rotate(toleft ? -3 : 3);
		updateConnections();
		for(int i = 0; i < 29; i++)
		{
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
			QTimer::singleShot(20, board[index], SLOT(update()));
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
			board[index]->rotate(toleft ? -3 : 3);
		}

		if (updateConnections())
			KNotification::event( "connectsound" );

		m_clickcount++;
		QString clicks = i18n("Click: %1",m_clickcount);
		statusBar()->changeItem(clicks,1);

		if (isGameOver())
		{
			KNotification::event( "winsound" );
			blink(index);

			KExtHighscore::Score score(KExtHighscore::Won);
			score.setScore(m_clickcount);
			KExtHighscore::submitScore(score, this);
		}
	}
}

void MainWindow::blink(int index)
{
	for(int i = 0; i < board[index]->width() * 2; i += 2)
	{
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
		QTimer::singleShot(20, board[index], SLOT(update()));
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents |
				QEventLoop::WaitForMoreEvents);
		board[index]->setLight(i);
	}
	board[index]->setLight(0);
}

bool MainWindow::isGameOver()
{
	for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	{
		const Cell::Dirs d = board[i]->dirs();
		if((d != Cell::Free) && (d != Cell::None) && !board[i]->isConnected())
			return false;
	}
	return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	event->accept();
}

void MainWindow::configureNotifications()
{
	KNotifyConfigWidget::configure(this);
}

void MainWindow::paintEvent(QPaintEvent* e)
{
	if (e->rect().intersects(centralWidget()->geometry()))
	{
		QPainter painter;
		QRect updateRect = e->rect().intersect(centralWidget()->geometry());

		if (m_invalidCache)
		{
			m_invalidCache = false;
			painter.begin(pixmapCache);
			m_background.render(&painter, "background");
			painter.end();
		}

		QPoint p = centralWidget()->mapFromParent(QPoint(updateRect.x(), updateRect.y()));
		QRect pixmapRect(p.x(), p.y(), updateRect.width(), updateRect.height());
		painter.begin(this);
		painter.drawPixmap(updateRect, *pixmapCache, pixmapRect);
		painter.end();
	}
}

void MainWindow::resizeEvent(QResizeEvent*)
{
	m_invalidCache = true;
	delete pixmapCache;
	pixmapCache = new QPixmap(centralWidget()->size());
}

#include "mainwindow.moc"
