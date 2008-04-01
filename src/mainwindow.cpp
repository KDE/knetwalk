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
#include <KConfigDialog>
#include <KIconLoader>
#include <KLocale>
#include <KStandardAction>
#include <KAction>
#include <KActionCollection>
#include <KStandardGameAction>
#include <KStatusBar>
#include <KNotification>
#include <KNotifyConfigWidget>
#include <KStandardDirs>
#include <KSelectAction>
#include <KMessageBox>

#include <KGameDifficulty>
#include <KGameThemeSelector>
#include <KScoreDialog>
#include <KGameClock>

#include <ctime>
#include <cmath>

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
    clickCount = 0;

    setupActions();
    
    statusBar()->insertItem("", StatusBarIndexMoves);
    statusBar()->insertItem("", StatusBarIndexTime);
    
    setAutoSaveSettings();

    createBoard();
    
    srand(time(0));

    gameClock = new KGameClock(this, KGameClock::MinSecOnly);
    connect(gameClock, SIGNAL(timeChanged(const QString&)), SLOT(updateStatusBar()));

    pixmapCache = new QPixmap(centralWidget()->size());
    invalidCache = true;

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
}

MainWindow::~MainWindow()
{
    delete pixmapCache;
}

void MainWindow::createBoard()
{
    View* view = new View(this);

    gridLayout = new QGridLayout(view);
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    setCentralWidget(view);
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
    
    KStandardAction::preferences(this, SLOT(configureSettings()), 
                                 actionCollection());
}

void MainWindow::configureSettings()
{
    if (KConfigDialog::showDialog( "settings")) {
        return;
    }
    
    KConfigDialog *dialog = new KConfigDialog(this, "settings", 
                                              Settings::self());
                                              
    dialog->addPage(new KGameThemeSelector( dialog, Settings::self(), 
                    KGameThemeSelector::NewStuffDisableDownload ), 
                    i18n("Theme"), "games-config-theme" );

    connect(dialog, SIGNAL( settingsChanged(const QString&)), this, 
            SLOT(loadSettings()));
    dialog->setHelp(QString(), "knetwalk");
    dialog->show();
}

void MainWindow::loadSettings() 
{
    if (!Renderer::self()->loadTheme(Settings::theme())) {
        KMessageBox::error(this,  
           i18n( "Failed to load \"%1\" theme. Please check your installation.",
           Settings::theme()));
        return;
    }
    
    // redraw
    invalidCache = true;
    for (int i = 0; i < cellCount(); ++i) {
        cellAt(i)->setInvalidCache();
    }
    repaint();
}

void MainWindow::showHighscores() 
{
    KScoreDialog scoreDialog(KScoreDialog::Name | KScoreDialog::Time, this);
    scoreDialog.addField(KScoreDialog::Custom1, i18n("Moves Penalty"), "moves");
    scoreDialog.setConfigGroup(KGameDifficulty::levelString());
    scoreDialog.exec();
}

void MainWindow::startNewGame()
{
    gameEnded = false;
    msgToWinGameShown = false;
    KNotification::event( "startsound", i18n("New Game") );
  
    KGameDifficulty::standardLevel l = KGameDifficulty::level();
    Settings::setSkill((int) l);
    
    Settings::self()->writeConfig();
    
    const bool isWrapped = (l == KGameDifficulty::VeryHard);
    const int size = boardSize();    
    
    QGridLayout *gridLayout = 
            static_cast<QGridLayout *>(centralWidget()->layout());
    
    for (int i = 0; i < cellCount(); ++i) {
        gridLayout->removeWidget(cellAt(i));
    }
    
    initializeGrid(size, size, (Wrapping)isWrapped);
    clickCount = -minimumMoves;
    gameClock->restart();
    
    updateStatusBar();
    
    for (int i = 0; i < cellCount(); i++) {
        gridLayout->addWidget(cellAt(i), 
                i / AbstractGrid::width(), i % AbstractGrid::width());
        
        cellAt(i)->disconnect();
        connect(cellAt(i), SIGNAL(lClicked(int)), SLOT(lClicked(int)));
        connect(cellAt(i), SIGNAL(rClicked(int)), SLOT(rClicked(int)));
        connect(cellAt(i), SIGNAL(mClicked(int)), SLOT(mClicked(int)));
        
        // called when a rotation ends
        connect(cellAt(i), SIGNAL(connectionsChanged()), 
                            SLOT(updateConnections()));
        
        cellAt(i)->setWhatsThis(i18n("<h3>Rules of Game</h3><p>You are the " 
          "system administrator and your goal is to connect each terminal and "
          "each cable to the central server.</p><p>Click the right mouse's "
          "button for turning the cable in a clockwise direction, and left "
          "mouse's button for turning the cable in a counter-clockwise "
          "direction.</p><p>Start the LAN with as few turns as possible!</p>"));
    }
    
    centralWidget()->update();
    
    // setRunning(true) on the first click
    KGameDifficulty::setRunning(false);
}

void MainWindow::updateStatusBar()
{
    QString moves = i18nc("Number of mouse clicks", "Moves: %1", clickCount);
    QString time = i18nc("Time elapsed", "Time: %1", gameClock->timeString());
    statusBar()->changeItem(moves, StatusBarIndexMoves);
    statusBar()->changeItem(time, StatusBarIndexTime);
}

void MainWindow::lClicked(int index)
{    
    if (!gameEnded) {
        KGameDifficulty::setRunning(true);
        rotate(index, false);
    }
}

void MainWindow::rClicked(int index)
{
    if (!gameEnded) {
        KGameDifficulty::setRunning(true);
        rotate(index, true);
    }
}

void MainWindow::mClicked(int index)
{
    if (!gameEnded) {
        KGameDifficulty::setRunning(true);
        cellAt(index)->setLocked(!cellAt(index)->isLocked());
    }
}

void MainWindow::rotate(int index, bool clockWise)
{
    const Directions d = cellAt(index)->cables();
    
    if ((d == None) || gameEnded || cellAt(index)->isLocked()) {
        KNotification::event( "clicksound" );
        //blink(index);
    } else {
        KNotification::event( "turnsound" );
        
        cellAt(index)->animateRotation(clockWise);
        
        // FIXME: won't work!!!
        //if (updateConnections())
        //    KNotification::event( "connectsound" );
        
        ++clickCount;
        
        updateStatusBar();
    }
}


void MainWindow::updateConnections() 
{
    QList<int> changedCells = AbstractGrid::updateConnections();
    
    foreach (int index, changedCells) {
        cellAt(index)->update();
    }  
    
    checkIfGameEnded();
}


void MainWindow::checkIfGameEnded()
{
    foreach (AbstractCell* cell, cells()) {
        if (cell->cables() != None && !cell->isConnected()){
            // there is a not empty cell that isn't connected
            if (!msgToWinGameShown && allTerminalsConnected()) {
                // don't show more that twice in one game
                QString text = i18n("Note: to win the game all terminals "
                                    "<strong>and all <em>cables</em></strong> " 
                                    "need to be connected to the server!");
                QString caption = i18n("The game is not won yet!");
                QString dontShowAgainName("dontShowGameNotWonYet"); // TODO i18n???
                KMessageBox::information(this, text, caption, dontShowAgainName);
                msgToWinGameShown = true;
            }
            return;
        }
    }
    
    KNotification::event("winsound");
    gameClock->pause();
 
    //=== calculate the score ====//
 
    double penalty = gameClock->seconds() / 2.0 * (clickCount/2 + 1);
    
    // normalize the penalty
    penalty = sqrt(penalty/cellCount());
    
    int score = static_cast<int>(100.0 / penalty);
    
    // create the new scoreInfo
    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KScoreDialog::Score].setNum(score);
    scoreInfo[KScoreDialog::Custom1].setNum(clickCount/2);
    scoreInfo[KScoreDialog::Time] = gameClock->timeString();
    
    // show the new dialog and add the new score to it
    KScoreDialog scoreDialog(KScoreDialog::Name | KScoreDialog::Time, this);
    scoreDialog.addField(KScoreDialog::Custom1, i18n("Moves Penalty"), "moves");
    scoreDialog.setConfigGroup(KGameDifficulty::levelString());
    bool madeIt = scoreDialog.addScore(scoreInfo);
    if (!madeIt) {
        QString comment = i18n("Your score was %1, you didn't make it "
                               "to the highScore list", score);
        scoreDialog.setComment(comment);
    }
    scoreDialog.exec();
    
    KGameDifficulty::setRunning(false);
    gameEnded = true;
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

Cell *MainWindow::cellAt(int index)
{
    return static_cast<Cell *>(cells()[index]);
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
        
        if (invalidCache) {
            invalidCache = false;
            
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
    invalidCache = true;
}

#include "mainwindow.moc"
