/***************************************************************************
 *   Copyright (C) 2004, 2005 Andi Peredri <andi@ukr.net>                  *
 *   Copyright (C) 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch>     *
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
#include <KScoreDialog>
#include <KStandardDirs>
#include <KSelectAction>
#include <KGameDifficulty>

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

	setupActions();
	
	statusBar()->insertPermanentItem("abcdefghijklmnopqrst: 0  ",1, 1);
	statusBar()->setItemAlignment(1, Qt::AlignLeft & Qt::AlignVCenter);
	setAutoSaveSettings();

	createBoard();
	
	srand(time(0));

	pixmapCache = new QPixmap(centralWidget()->size());
	m_invalidCache = true;
	m_background.load( KStandardDirs::locate( "data","knetwalk/all.svgz" ) );

	// Difficulty
	KGameDifficulty::init(this, this, SLOT(slotNewGame()));
	KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
	KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
	KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
	KGameDifficulty::addStandardLevel(KGameDifficulty::VeryHard);
	KGameDifficulty::setLevel((KGameDifficulty::standardLevel) (Settings::skill()));

	setupGUI();

	slotNewGame();
}

MainWindow::~MainWindow()
{
	delete pixmapCache;
}

void MainWindow::createBoard()
{
	QFrame* frame = new QFrame(this);
	frame->setFrameStyle(QFrame::NoFrame);
	frame->setMinimumSize(MINIMUM_WIDTH, MINIMUM_HEIGHT);

	gridLayout = new QGridLayout(frame);
	gridLayout->setMargin(0);
	gridLayout->setSpacing(0);
	setCentralWidget(frame);

	Cell::initPixmaps();
	for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	{
		board[i] = new Cell(frame, i);
		gridLayout->addWidget(board[i], i / MasterBoardSize, i % MasterBoardSize);
		//board[i]->resize(32, 32); //TODO: needed ???
		connect(board[i], SIGNAL(lClicked(int)), SLOT(lClicked(int)));
		connect(board[i], SIGNAL(rClicked(int)), SLOT(rClicked(int)));
		connect(board[i], SIGNAL(mClicked(int)), SLOT(mClicked(int)));
		board[i]->setWhatsThis(i18n("<h3>Rules of Game</h3><p>You are the system administrator and your goal is to connect each computer to the central server.</p><p>Click the right mouse's button for turning the cable in a clockwise direction, and left mouse's button for turning the cable in a counter-clockwise direction.</p><p>Start the LAN with as few turns as possible!</p>"));
	}
}

void MainWindow::setBoardSize(int size)
{
    if (!(size%2) || size > MasterBoardSize) {
        kDebug() << "Wrong size!!\n";
        return;
    }
    //TODO: boardSize = size
    int start = (MasterBoardSize - size)/2;
    for (int i = 0; i < start; ++i) {
        // TODO: set only the last rows and columns to zero not half at the beginning and half at the end
        gridLayout->setRowStretch(i, 0);
        gridLayout->setRowStretch(MasterBoardSize - 1 - i, 0);
        gridLayout->setColumnStretch(i, 0);
        gridLayout->setColumnStretch(MasterBoardSize - 1 - i, 0);
    }
    for (int i = start; i < start + size; ++i) {
        gridLayout->setRowStretch(i, 1);
        gridLayout->setColumnStretch(i, 1);
    }
}

void MainWindow::setupActions()
{
	// Game
	KStandardGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection());
	KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
	KStandardGameAction::quit(this, SLOT(close()), actionCollection());
	
	// Settings
	KStandardAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());
}


void MainWindow::showHighscores()
{
	KScoreDialog ksdialog(KScoreDialog::Name, this);
	ksdialog.exec();
}

void MainWindow::slotNewGame()
{
	KGameDifficulty::standardLevel l = KGameDifficulty::level();
	Settings::setSkill((int) l);
	
	if(l == KGameDifficulty::VeryHard) wrapped = true;
	else wrapped = false;

	Settings::self()->writeConfig();

	m_clickcount = 0;
	QString clicks = i18nc("Number of mouse clicks", "Moves: %1",m_clickcount);
	statusBar()->changeItem(clicks,1);

	KNotification::event( "startsound", i18n("New Game") );

	for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	{
		board[i]->setDirs(Cell::None);
		board[i]->setConnected(false);
		board[i]->setRoot(false);
		board[i]->setLocked(false);
	}

	const int size = boardSize();
	setBoardSize(size);

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
		//blink(index);
	}
	else
	{
		KNotification::event( "turnsound" );

		updateConnections();
		for(int i = 0; i < 18; i++)
		{
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
			QTimer::singleShot(35, board[index], SLOT(update()));
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
			board[index]->rotate(toleft ? -5 : 5);
		}

		if (updateConnections())
			KNotification::event( "connectsound" );

		m_clickcount++;
		QString clicks = i18n("Moves: %1",m_clickcount);
		statusBar()->changeItem(clicks,1);

		if (isGameOver())
		{
			KNotification::event( "winsound" );
			blink(index);
			
			KScoreDialog ksdialog(KScoreDialog::Name, this);
			ksdialog.setConfigGroup(KGameDifficulty::levelString());
			
			//ksdialog.addField(KScoreDialog::Custom1, "Num of Moves", "moves");
			//KScoreDialog::FieldInfo scoreInfo;
			//scoreInfo[KScoreDialog::Score].setNum(1000 * boardSize() * boardSize() / m_clickcount);
			//scoreInfo[KScoreDialog::Score].setNum(m_clickcount);
			
			ksdialog.addScore(m_clickcount, KScoreDialog::LessIsMore);
			
			ksdialog.exec();
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

int MainWindow::boardSize()
{
    switch (KGameDifficulty::level()) {
        case KGameDifficulty::Easy: return NoviceBoardSize;
        case KGameDifficulty::Medium: return NormalBoardSize;
        case KGameDifficulty::Hard: return ExpertBoardSize;
        default: return MasterBoardSize;
    }
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
			
			int w = pixmapCache->width();
			int h = pixmapCache->height();
			const qreal ratio = 1.02;
            	    	QRectF boundingRect((1.0-ratio)/2 * w, (1.0-ratio)/2 * h, 
                                            ratio * w, ratio * h);
			
			painter.begin(pixmapCache);
			//pixmapCache->fill(QColor(255, 0, 0)); // for testing only
			m_background.render(&painter, "background", boundingRect);
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
	
	// keep aspect ratio
	int width = centralWidget()->width();
	int height = centralWidget()->height();
	int size = qMin(width, height);
	size = static_cast<int>(size * 0.95); // add a border
	int borderLeft = (width - size)/2;
	int borderTop = (height - size)/2;
	QRect r(borderLeft, borderTop, size, size);
	qobject_cast<QFrame *>(centralWidget())->setFrameRect(r);
	
	delete pixmapCache;
	pixmapCache = new QPixmap(centralWidget()->size());
}

#include "mainwindow.moc"
