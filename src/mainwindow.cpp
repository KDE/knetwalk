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

#include <KConfigDialog>
#include <KLocale>
#include <KStandardAction>
#include <KAction>
#include <KToggleAction>
#include <KActionCollection>
#include <KStandardGameAction>
#include <KStandardDirs>
#include <KMessageBox>
#include <KStatusBar>

#include <KgDifficulty>
#include <KgThemeSelector>
#include <KGameRenderer>
#include <KScoreDialog>
#include <KGameClock>
#include <KgSound>

#include <ctime>
#include <cmath>

#include "globals.h"
#include "settings.h"
#include "cell.h"
#include "abstractgrid.h"
#include "scene.h"
#include "view.h"
#include "fielditem.h"

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent), m_clickCount(0)
{
    m_scene = new KNetWalkScene(this);
    connect(m_scene->fieldItem(), SIGNAL(gameWon()), this, SLOT(gameOver()));
    connect(m_scene->fieldItem(), SIGNAL(rotationPerformed()), this, SLOT(rotationPerformed()));

    m_view = new KNetWalkView(m_scene, this);
    m_view->setCacheMode(QGraphicsView::CacheBackground);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setFrameStyle(QFrame::NoFrame);
    m_view->setOptimizationFlags(QGraphicsView::DontClipPainter |
                                 QGraphicsView::DontSavePainterState);
    m_view->setCacheMode(QGraphicsView::CacheBackground);
    setCentralWidget(m_view);

    statusBar()->insertItem(QLatin1String( "" ), StatusBarIndexMoves, 1);
    statusBar()->insertItem(QLatin1String( "" ), StatusBarIndexTime, 1);

    // Difficulty
    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::Easy, KgDifficultyLevel::VeryHard
    );
    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), SLOT(startNewGame()));

    KgThemeProvider* provider = Renderer::self()->themeProvider();
    m_selector = new KgThemeSelector(provider);
    connect(provider, SIGNAL(currentThemeChanged(const KgTheme*)), SLOT(loadSettings()));

    setupActions();

    setupGUI();

    setAutoSaveSettings();

    srand(time(0));

    m_gameClock = new KGameClock(this, KGameClock::MinSecOnly);
    connect(m_gameClock, SIGNAL(timeChanged(QString)), SLOT(updateStatusBar()));

    m_soundStart = new KgSound(KStandardDirs::locate("appdata", "sounds/start.wav"));
    m_soundWin = new KgSound(KStandardDirs::locate("appdata", "sounds/win.wav"));

    startNewGame();
}

void MainWindow::setupActions()
{
    // Game
    KStandardGameAction::gameNew(this, SLOT(startNewGame()),
                                 actionCollection());

    m_pauseAction = KStandardGameAction::pause(this, SLOT(pauseGame(bool)),
                                               actionCollection());

    KStandardGameAction::highscores(this, SLOT(showHighscores()),
                                    actionCollection());

    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    // Settings
    KStandardAction::preferences(m_selector, SLOT(showAsDialog()),
                                 actionCollection());

    m_soundAction = new KToggleAction(i18n("&Play Sounds"), this);
    connect(m_soundAction, SIGNAL(triggered(bool)), this, SLOT(setSounds(bool)));
    actionCollection()->addAction( QLatin1String( "toggle_sound" ), m_soundAction);
    m_soundAction->setChecked(Settings::playSounds());

    KAction* action = new KAction(i18n("Keyboard: Field right"), this);
    action->setShortcut(Qt::Key_Right);
    connect(action, SIGNAL(triggered()), m_scene->fieldItem(), SLOT(kbGoRight()));
    actionCollection()->addAction( QLatin1String( "kb_go_right" ), action);

    action = new KAction(i18n("Keyboard: Field left"),this);
    action->setShortcut(Qt::Key_Left);
    connect(action, SIGNAL(triggered()), m_scene->fieldItem(), SLOT(kbGoLeft()));
    actionCollection()->addAction( QLatin1String( "kb_go_left" ), action);

    action = new KAction(i18n("Keyboard: Field up"),this);
    action->setShortcut(Qt::Key_Up);
    connect(action, SIGNAL(triggered()), m_scene->fieldItem(), SLOT(kbGoUp()));
    actionCollection()->addAction( QLatin1String( "kb_go_up" ), action);

    action = new KAction(i18n("Keyboard: Field down"),this);
    action->setShortcut(Qt::Key_Down);
    connect(action, SIGNAL(triggered()), m_scene->fieldItem(), SLOT(kbGoDown()));
    actionCollection()->addAction( QLatin1String( "kb_go_down" ), action);

    action = new KAction(i18n("Keyboard: Turn clockwise"),this);
    action->setShortcut(Qt::Key_Return);
    connect(action, SIGNAL(triggered()), m_scene->fieldItem(), SLOT(kbTurnClockwise()));
    actionCollection()->addAction( QLatin1String( "kb_turn_clockwise" ), action);

    action = new KAction(i18n("Keyboard: Turn counterclockwise"),this);
    action->setShortcut(Qt::CTRL + Qt::Key_Return);
    connect(action, SIGNAL(triggered()), m_scene->fieldItem(), SLOT(kbTurnCounterclockwise()));
    actionCollection()->addAction( QLatin1String( "kb_turn_counterclockwise" ), action);

    action = new KAction(i18n("Keyboard: Toggle lock"),this);
    action->setShortcut(Qt::Key_Space);
    connect(action, SIGNAL(triggered()), m_scene->fieldItem(), SLOT(kbLock()));
    actionCollection()->addAction( QLatin1String( "kb_lock" ), action);
}

void MainWindow::loadSettings()
{
    m_view->resetCachedContent();
    m_scene->resizeScene(m_scene->sceneRect().size());
}

void MainWindow::showHighscores()
{
    KScoreDialog scoreDialog(KScoreDialog::Name | KScoreDialog::Time, this);
    scoreDialog.addField(KScoreDialog::Custom1, i18n("Moves Penalty"), QLatin1String( "moves" ));
    scoreDialog.initFromDifficulty(Kg::difficulty());
    scoreDialog.exec();
}

void MainWindow::setSounds(bool val)
{
    Settings::setPlaySounds(val);
    Settings::self()->writeConfig();
}

void MainWindow::startNewGame()
{
    if(Settings::playSounds())
        m_soundStart->start();

    const KgDifficultyLevel::StandardLevel l = Kg::difficultyLevel();

    const bool isWrapped = (l == KgDifficultyLevel::VeryHard);
    const int size = boardSize();
    m_scene->startNewGame(size, size, (Wrapping)isWrapped);
    m_clickCount = -m_scene->fieldItem()->minimumMoves();
    m_gameClock->restart();

    if(m_pauseAction->isChecked())
    {
        m_scene->setGamePaused(false);
        m_pauseAction->setChecked(false);
    }
    m_pauseAction->setEnabled(true);
    Kg::difficulty()->setGameRunning(false);

    updateStatusBar();
}

void MainWindow::gameOver()
{
    if(Settings::playSounds())
        m_soundWin->start();

    m_gameClock->pause();
    m_pauseAction->setEnabled(false);
    Kg::difficulty()->setGameRunning(false);

    //=== calculate the score ====//

    double penalty = m_gameClock->seconds() / 2.0 * (m_clickCount/2 + 1);

    // normalize the penalty
    penalty = std::sqrt(penalty/m_scene->fieldItem()->cellCount());

    int score = static_cast<int>(100.0 / penalty);

    // create the new scoreInfo
    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KScoreDialog::Score].setNum(score);
    scoreInfo[KScoreDialog::Custom1].setNum(m_clickCount/2);
    scoreInfo[KScoreDialog::Time] = m_gameClock->timeString();

    // show the new dialog and add the new score to it
    KScoreDialog scoreDialog(KScoreDialog::Name | KScoreDialog::Time, this);
    scoreDialog.addField(KScoreDialog::Custom1, i18n("Moves Penalty"), QLatin1String( "moves" ));
    scoreDialog.initFromDifficulty(Kg::difficulty());
    bool madeIt = scoreDialog.addScore(scoreInfo);
    if (!madeIt) {
        QString comment = i18np("Your score was %1, you did not make it to the high score list.",
                                "Your score was %1, you did not make it to the high score list.", score);
        scoreDialog.setComment(comment);
    }
    scoreDialog.exec();
}

void MainWindow::rotationPerformed()
{
    Kg::difficulty()->setGameRunning(true);
    m_clickCount++;
    updateStatusBar();
}

void MainWindow::pauseGame(bool paused)
{
    m_scene->setGamePaused(paused);
    if(paused) {
        m_gameClock->pause();
    } else {
        m_gameClock->resume();
    }
}

void MainWindow::updateStatusBar()
{
    QString moves = i18nc("Number of mouse clicks", "Moves: %1", m_clickCount);
    QString time = i18nc("Time elapsed", "Time: %1", m_gameClock->timeString());
    statusBar()->changeItem(moves, StatusBarIndexMoves);
    statusBar()->changeItem(time, StatusBarIndexTime);
}

int MainWindow::boardSize()
{
    switch (Kg::difficultyLevel()) {
    case KgDifficultyLevel::Easy: return NoviceBoardSize;
    case KgDifficultyLevel::Medium: return NormalBoardSize;
    case KgDifficultyLevel::Hard: return ExpertBoardSize;
    default: return MasterBoardSize;
    }
}

#include "mainwindow.moc"
