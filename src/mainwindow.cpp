/*
    Copyright 2004-2005 Andi Peredri <andi@ukr.net>   
    Copyright 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch> 
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

#include "mainwindow.h"

#include <QEventLoop>
#include <QPushButton>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QGridLayout>
#include <QCloseEvent>
#include <QApplication>

#include <KGlobal>
#include <KIconLoader>
#include <KLocale>
#include <KScoreDialog>
#include <KStandardAction>
#include <KAction>
#include <KActionCollection>
#include <KStandardGameAction>
#include <KStatusBar>
#include <KNotification>
#include <KNotifyConfigWidget>
#include <KStandardDirs>
#include <KSelectAction>
#include <KGameDifficulty>

#include <time.h>

#include "consts.h"
#include "settings.h"
#include "cell.h"
#include "view.h"

#include "renderer.h"
#include "abstractgrid.h"

static QMap<Directions, Directions> contrdirs;

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    kDebug() << Settings::skill();
    m_clickcount = 0;

    contrdirs[Up] = Down;
    contrdirs[Right] = Left;
    contrdirs[Down] = Up;
    contrdirs[Left] = Right;

    setupActions();
    
    statusBar()->insertPermanentItem("abcdefghijklmnopqrst: 0  ", 1, 1);
    statusBar()->setItemAlignment(1, Qt::AlignLeft & Qt::AlignVCenter);
    setAutoSaveSettings();

    createBoard();
    
    srand(time(0));

    pixmapCache = new QPixmap(centralWidget()->size());
    m_invalidCache = true;

    // default values of KConfig XT don't seem to work
    // this works around it. TODO: see why
    
    // Difficulty
    KGameDifficulty::init(this, this, SLOT(startNewGame()));
    KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::VeryHard);
    
    if (Settings::skill() == 0) {
        KGameDifficulty::setLevel(KGameDifficulty::Easy);
    } else { 
        KGameDifficulty::setLevel(
                (KGameDifficulty::standardLevel) (Settings::skill()) );
    }
    kDebug() << KGameDifficulty::levelString() << Settings::skill();

    setupGUI();

    startNewGame();
}

MainWindow::~MainWindow()
{
    delete pixmapCache;
}

void MainWindow::createBoard()
{
    View* view = new View(this);
    view->setFrameStyle(QFrame::NoFrame);
    view->setMinimumSize(MinimumWidth, MinimumHeight);

    gridLayout = new QGridLayout(view);
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    setCentralWidget(view);

    Cell::initPixmaps();
    for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++) {
        board[i] = new Cell(view, i);
        gridLayout->addWidget(board[i], i / MasterBoardSize, i % MasterBoardSize);
        //board[i]->resize(32, 32); //TODO: needed ???
        connect(board[i], SIGNAL(lClicked(int)), SLOT(lClicked(int)));
        connect(board[i], SIGNAL(rClicked(int)), SLOT(rClicked(int)));
        connect(board[i], SIGNAL(mClicked(int)), SLOT(mClicked(int)));
        
        connect(board[i], SIGNAL(connectionsChanged()), 
                SLOT(updateConnections()));
        
        board[i]->setWhatsThis(i18n("<h3>Rules of Game</h3><p>You are the " 
        "system administrator and your goal is to connect each terminal and "
        "each cable to the central server.</p><p>Click the right mouse's "
        "button for turning the cable in a clockwise direction, and left "
        "mouse's button for turning the cable in a counter-clockwise "
        "direction.</p><p>Start the LAN with as few turns as possible!</p>"));
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
    KStandardGameAction::gameNew(this, SLOT(startNewGame()), 
                                 actionCollection());
    
    KStandardGameAction::highscores(this, SLOT(showHighscores()), 
                                    actionCollection());
    
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    
    // Settings
    KStandardAction::configureNotifications(this, 
                SLOT(configureNotifications()), actionCollection());
}


void MainWindow::showHighscores()
{
    KScoreDialog ksdialog(KScoreDialog::Name, this);
    ksdialog.exec();
}

void MainWindow::startNewGame()
{
    gameEnded = false;
    
    KGameDifficulty::standardLevel l = KGameDifficulty::level();
    Settings::setSkill((int) l);
    
    if (l == KGameDifficulty::VeryHard) {
        wrapped = true;
    } else {
        wrapped = false;
    }

    Settings::self()->writeConfig();

    m_clickcount = 0;
    QString clicks = i18nc("Number of mouse clicks", "Moves: %1", m_clickcount);
    statusBar()->changeItem(clicks, 1);

    KNotification::event( "startsound", i18n("New Game") );

    for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++) {
        board[i]->setDirs(None);
        board[i]->setConnected(false);
        board[i]->setRoot(false);
        board[i]->setLocked(false);
    }

    const int size = boardSize();
    setBoardSize(size);
    
    // TODO: enable wrapped
    AbstractGrid grid;
    grid.initializeGrid(size, size, (Wrapping)wrapped);

    const int start = (MasterBoardSize - size) / 2;
    
    int i = 0; // index of grid
    for (int r = start; r < start+size; ++r)
    for (int c = start; c < start+size; ++c) {
        int index = r * MasterBoardSize + c; // index of board
        board[index]->setDirs(grid.cells()[i]->cables());
        board[index]->setConnected(false);
        board[index]->setRoot(false);
        board[index]->setLocked(false);
        
        if (grid.cells()[i]->isServer()) {
            board[index]->setConnected(true); // TODO: put in Cell::setRoot()
            board[index]->setRoot(true);
            root = board[index];
        }
        ++i;
    } // for for
    /*
    const int start = (MasterBoardSize - size) / 2;
    const int rootrow = rand() % size;
    const int rootcol = rand() % size;

    root = board[(start + rootrow) * MasterBoardSize + start + rootcol];
    root->setConnected(true);
    root->setRoot(true);

    while (true)
    {
        for (int row = start; row < start + size; row++)
            for(int col = start; col < start + size; col++)
                board[row * MasterBoardSize + col]->setDirs(None);

        CellList list;
        list.append(root);
        if (rand() % 2) addRandomDir(list);

        while (!list.isEmpty())
        {
            if(rand() % 2)
            {
                addRandomDir(list);
                if (rand() % 2) addRandomDir(list);
                list.erase(list.begin());
            }
            else
            {
                list.append(list.first());
                list.erase(list.begin());
            }
        }

        int cells = 0;
        for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
        {
            Directions d = board[i]->dirs();
            if ((d != None) && (d != Cell::None)) cells++;
        }
        if (cells >= MinimumNumCells) break;
    }

    for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
        board[i]->rotate((rand() % 4) * 90);
    */
    updateConnections();
    KGameDifficulty::setRunning(false); // setRunning(true) on the first click
}

void MainWindow::updateConnections()
{
    bool newconnection[MasterBoardSize * MasterBoardSize];
    for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++) {
        newconnection[i] = false;
    }

    CellList list;
    if (!root->isRotated()) {
        newconnection[root->index()] = true;
        list.append(root);
    }
    
    while (!list.isEmpty()) {
        Cell* cell = list.first();
        Cell* ucell = uCell(cell);
        Cell* rcell = rCell(cell);
        Cell* dcell = dCell(cell);
        Cell* lcell = lCell(cell);
        
        if ((cell->dirs() & Up) && ucell && (ucell->dirs() & Down) &&
                !newconnection[ucell->index()] && !ucell->isRotated()) {
            newconnection[ucell->index()] = true;
            list.append(ucell);
        }
        if ((cell->dirs() & Right) && rcell && (rcell->dirs() & Left) &&
                !newconnection[rcell->index()] && !rcell->isRotated()) {
            newconnection[rcell->index()] = true;
            list.append(rcell);
        }
        if ((cell->dirs() & Down) && dcell && (dcell->dirs() & Up) &&
                !newconnection[dcell->index()] && !dcell->isRotated()) {
            newconnection[dcell->index()] = true;
            list.append(dcell);
        }
        if ((cell->dirs() & Left) && lcell && (lcell->dirs() & Right) &&
                !newconnection[lcell->index()] && !lcell->isRotated()) {
            newconnection[lcell->index()] = true;
            list.append(lcell);
        }
        list.erase(list.begin());
    }

    bool newConnections = false;
    for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++) {
        if (!board[i]->isConnected() && newconnection[i]) {
            newConnections = true;
        }
        board[i]->setConnected(newconnection[i]);
    }
    
    if (newConnections) checkIfGameEnded();
}

Cell* MainWindow::uCell(Cell* cell) const
{
    if (cell->index() >= MasterBoardSize) {
        return board[cell->index() - MasterBoardSize];
    } else if (wrapped) {
        return board[MasterBoardSize * (MasterBoardSize - 1) + cell->index()];
    } else {
        return 0;
    }
}

Cell* MainWindow::dCell(Cell* cell) const
{
    if (cell->index() < MasterBoardSize * (MasterBoardSize - 1)) {
        return board[cell->index() + MasterBoardSize];
    } else if (wrapped) {
        return board[cell->index() - MasterBoardSize * (MasterBoardSize - 1)];
    } else {
        return 0;
    }
}

Cell* MainWindow::lCell(Cell* cell) const
{
    if (cell->index() % MasterBoardSize > 0) {
        return board[cell->index() - 1];
    } else if (wrapped) {
        return board[cell->index() - 1 + MasterBoardSize];
    } else {
        return 0;
    }
}

Cell* MainWindow::rCell(Cell* cell) const
{
    if (cell->index() % MasterBoardSize < MasterBoardSize - 1) {
        return board[cell->index() + 1];
    } else if (wrapped) {
        return board[cell->index() + 1 - MasterBoardSize];
    } else {
        return 0;
    }
}

void MainWindow::lClicked(int index)
{
    KGameDifficulty::setRunning(true);
    rotate(index, false);
}

void MainWindow::rClicked(int index)
{
    KGameDifficulty::setRunning(true);
    rotate(index, true);
}

void MainWindow::mClicked(int index)
{
    KGameDifficulty::setRunning(true);
    board[index]->setLocked( !board[index]->isLocked() );
}

void MainWindow::rotate(int index, bool clockWise)
{
    const Directions d = board[index]->dirs();
    if ((d == None) || gameEnded || board[index]->isLocked()) {
        KNotification::event( "clicksound" );
        //blink(index);
    } else {
        KNotification::event( "turnsound" );
        
        board[index]->animateRotation(clockWise);
        
        // FIXME: won't work!!!
        //if (updateConnections())
        //    KNotification::event( "connectsound" );
        
        m_clickcount++;
        QString clicks = i18n("Moves: %1",m_clickcount);
        statusBar()->changeItem(clicks,1);
    }
}

void MainWindow::blink(int index)
{
    for (int i = 0; i < board[index]->width() * 2; i += 2) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        QTimer::singleShot(30, board[index], SLOT(update()));
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents |
        QEventLoop::WaitForMoreEvents);
        board[index]->setLight(i);
    }
    
    board[index]->setLight(0);
}

void MainWindow::checkIfGameEnded()
{
    bool ended = true;
    for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++) {
        const Directions d = board[i]->dirs();
        if ((d != None) && !board[i]->isConnected())
            ended = false;
    }
    
    if (ended) {
        KNotification::event( "winsound" );
        //blink(index);
        
        KScoreDialog ksdialog(KScoreDialog::Name, this);
        ksdialog.setConfigGroup(KGameDifficulty::levelString());
        
        //ksdialog.addField(KScoreDialog::Custom1, "Num of Moves", "moves");
        //KScoreDialog::FieldInfo scoreInfo;
        //scoreInfo[KScoreDialog::Score].setNum(1000 * boardSize() * boardSize() / m_clickcount);
        //scoreInfo[KScoreDialog::Score].setNum(m_clickcount);
        
        ksdialog.addScore(m_clickcount, KScoreDialog::LessIsMore);
        ksdialog.exec();
        
        KGameDifficulty::setRunning(false);
        gameEnded = true;
    }
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
    if (e->rect().intersects(centralWidget()->geometry())) {
        QPainter painter;
        QRect updateRect = e->rect().intersect(centralWidget()->geometry());
        
        if (m_invalidCache) {
            m_invalidCache = false;
            
            // keep aspect ratio
            int width = centralWidget()->width();
            int height = centralWidget()->height();
            
            delete pixmapCache;
            pixmapCache = new QPixmap(width, height);
            *pixmapCache =
                    Renderer::self()->backgroundPixmap(centralWidget()->size());
            
            // calculate background overlay bounding rect
            int size = qMin(width, height);
            
            // add a border
            size = static_cast<int>(size * (1.0 - 2*OverlayBorder));
            int borderLeft = (width - size)/2;
            int borderTop = (height - size)/2;
            
            QPixmap overlay = Renderer::self()->backgroundOverlayPixmap(size);
            
            painter.begin(pixmapCache);
            painter.drawPixmap(borderLeft, borderTop, overlay);
            painter.end();
        }

        QPoint p = centralWidget()->mapFromParent(QPoint(updateRect.x(),
                                                  updateRect.y()));
        QRect pixmapRect(p.x(), p.y(), updateRect.width(), updateRect.height());
        painter.begin(this);
        
        painter.drawPixmap(updateRect, *pixmapCache, pixmapRect);
        painter.end();
    }
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    m_invalidCache = true;
}

#include "mainwindow.moc"
