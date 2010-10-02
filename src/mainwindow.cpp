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
#include <KNotification>
#include <KNotifyConfigWidget>
#include <KMessageBox>
#include <KStatusBar>

#include <KGameDifficulty>
#include <KGameThemeSelector>
#include <KScoreDialog>
#include <KGameClock>

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
    KGameDifficulty::init(this, this, SLOT(startNewGame()));
    KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::VeryHard);

    setupActions();

    setupGUI();

    setAutoSaveSettings();

    srand(time(0));

    m_gameClock = new KGameClock(this, KGameClock::MinSecOnly);
    connect(m_gameClock, SIGNAL(timeChanged(const QString&)), SLOT(updateStatusBar()));

    // default values of KConfig XT don't seem to work
    // this works around it. TODO: see why (and whether it still is true)
    if (Settings::skill() == 0) {
        KGameDifficulty::setLevel(KGameDifficulty::Easy);
    } else {
        KGameDifficulty::setLevel(
                (KGameDifficulty::standardLevel) (Settings::skill()) );
    }
    kDebug() << KGameDifficulty::levelString() << Settings::skill();

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
    KStandardAction::configureNotifications(this,
                SLOT(configureNotifications()), actionCollection());

    KStandardAction::preferences(this, SLOT(configureSettings()),
                                 actionCollection());

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

void MainWindow::configureSettings()
{
    if (KConfigDialog::showDialog( QLatin1String(  "settings" ))) {
        return;
    }

    KConfigDialog *dialog = new KConfigDialog(this, QLatin1String( "settings" ),
                                              Settings::self());

    dialog->addPage(new KGameThemeSelector( dialog, Settings::self(),
                    KGameThemeSelector::NewStuffDisableDownload ),
                    i18n("Theme"), QLatin1String( "games-config-theme" ));

    connect(dialog, SIGNAL( settingsChanged(const QString&)), this,
            SLOT(loadSettings()));
    dialog->setHelp(QString(), QLatin1String( "knetwalk" ));
    dialog->show();
}

void MainWindow::loadSettings()
{
    Renderer::self()->setTheme(Settings::theme());
    m_view->resetCachedContent();
    m_scene->resizeScene(m_scene->sceneRect().size());
}

void MainWindow::showHighscores()
{
    KScoreDialog scoreDialog(KScoreDialog::Name | KScoreDialog::Time, this);
    scoreDialog.addField(KScoreDialog::Custom1, i18n("Moves Penalty"), QLatin1String( "moves" ));
    scoreDialog.addLocalizedConfigGroupNames(KGameDifficulty::localizedLevelStrings()); //Add all the translations of the group names
    scoreDialog.setConfigGroupWeights(KGameDifficulty::levelWeights());
    scoreDialog.setConfigGroup(KGameDifficulty::localizedLevelString());
    scoreDialog.exec();
}

void MainWindow::startNewGame()
{
    KNotification::event( QLatin1String( "startsound" ), i18n("New Game") );

    KGameDifficulty::standardLevel l = KGameDifficulty::level();
    Settings::setSkill((int) l);

    Settings::self()->writeConfig();

    const bool isWrapped = (l == KGameDifficulty::VeryHard);
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
    KGameDifficulty::setRunning(false);

    updateStatusBar();
}

void MainWindow::gameOver()
{
    KNotification::event(QLatin1String( "winsound" ));
    m_gameClock->pause();
    m_pauseAction->setEnabled(false);
    KGameDifficulty::setRunning(false);

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
    scoreDialog.addLocalizedConfigGroupNames(KGameDifficulty::localizedLevelStrings()); //Add all the translations of the group names
    scoreDialog.setConfigGroupWeights(KGameDifficulty::levelWeights());
    scoreDialog.setConfigGroup(KGameDifficulty::localizedLevelString());
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
    KGameDifficulty::setRunning(true);
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

void MainWindow::configureNotifications()
{
    KNotifyConfigWidget::configure(this);
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

#include "mainwindow.moc"
