/*
    Copyright 2004-2005 Andi Peredri <andi@ukr.net>
    Copyright 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch>
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com>
    Copyright 2013 Ashwin Rajeev<ashwin_rajeev@hotmail.com>

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

#include "ui_customgame.h"

#include "globals.h"
#include "gameview.h"
#include "settings.h"
#include "abstractgrid.h"

class CustomGameConfig : public QWidget
{
public:
    CustomGameConfig(QWidget *parent)
        : QWidget(parent)
    {
        ui.setupUi(this);
    }
private:
    Ui::CustomGameConfig ui;
};

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent), m_clickCount(0),
      m_view(new GameView(this))
{
    connect(m_view, SIGNAL(gameOver(QVariant)), this, SLOT(gameOver(QVariant)));
    connect(m_view, SIGNAL(rotationPerformed()), this, SLOT(rotationPerformed()));
    connect(this, SIGNAL(pause(QVariant)), m_view->rootObject(), SLOT(pause(QVariant)));

    statusBar()->insertItem(QLatin1String( "" ), StatusBarIndexMoves, 1);
    statusBar()->insertItem(QLatin1String( "" ), StatusBarIndexTime, 1);

    // Difficulty
    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::Easy, KgDifficultyLevel::VeryHard
    );
    Kg::difficulty()->addLevel(
                new KgDifficultyLevel(100, QByteArray("Custom"), i18n("Custom"))
    );
    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), SLOT(startNewGame()));


    setCentralWidget(m_view);

    setupActions();

    setupGUI();

    setAutoSaveSettings();

    srand(time(0));

    m_gameClock = new KGameClock(this, KGameClock::MinSecOnly);
    connect(m_gameClock, SIGNAL(timeChanged(QString)), SLOT(updateStatusBar()));

    m_soundStart = new KgSound(KStandardDirs::locate("appdata", "sounds/start.wav"), this);
    m_soundWin = new KgSound(KStandardDirs::locate("appdata", "sounds/win.wav"), this);

    startNewGame();
}

void MainWindow::setupActions()
{
    // Game
    KStandardGameAction::gameNew(this, SLOT(startNewGame()),
                                 actionCollection());

    m_pauseAction = KStandardGameAction::pause(this, SLOT(pauseGame(bool)),
                                               actionCollection());

    m_solveAction = KStandardGameAction::solve(m_view, SLOT(solve()),
                                               actionCollection());

    KStandardGameAction::highscores(this, SLOT(showHighscores()),
                                    actionCollection());

    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    // Settings
    KStandardAction::preferences(this, SLOT(configureSettings()), actionCollection());

    KToggleAction *soundAction = new KToggleAction(i18n("&Play Sounds"), this);
    connect(soundAction, SIGNAL(triggered(bool)), this, SLOT(setSounds(bool)));
    actionCollection()->addAction( QLatin1String( "toggle_sound" ), soundAction);
    soundAction->setChecked(Settings::playSounds());

    KAction *action = new KAction(i18n("&Unlock All"), this);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(unlockAll()));
    actionCollection()->addAction( QLatin1String( "unlock_all" ), action);

    action = new KAction(i18n("Keyboard: Field right"), this);
    action->setShortcut(Qt::Key_Right);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(kbGoRight()));
    actionCollection()->addAction( QLatin1String( "kb_go_right" ), action);

    action = new KAction(i18n("Keyboard: Field left"),this);
    action->setShortcut(Qt::Key_Left);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(kbGoLeft()));
    actionCollection()->addAction( QLatin1String( "kb_go_left" ), action);

    action = new KAction(i18n("Keyboard: Field up"),this);
    action->setShortcut(Qt::Key_Up);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(kbGoUp()));
    actionCollection()->addAction( QLatin1String( "kb_go_up" ), action);

    action = new KAction(i18n("Keyboard: Field down"),this);
    action->setShortcut(Qt::Key_Down);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(kbGoDown()));
    actionCollection()->addAction( QLatin1String( "kb_go_down" ), action);

    action = new KAction(i18n("Keyboard: Turn clockwise"),this);
    action->setShortcut(Qt::Key_Return);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(rotateClockwise()));
    actionCollection()->addAction( QLatin1String( "kb_turn_clockwise" ), action);

    action = new KAction(i18n("Keyboard: Turn counterclockwise"),this);
    action->setShortcut(Qt::CTRL + Qt::Key_Return);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(rotateCounterclockwise()));
    actionCollection()->addAction( QLatin1String( "kb_turn_counterclockwise" ), action);

    action = new KAction(i18n("Keyboard: Toggle lock"),this);
    action->setShortcut(Qt::Key_Space);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(toggleLock()));
    actionCollection()->addAction( QLatin1String( "kb_lock" ), action);
}

void MainWindow::configureSettings()
{
    if (KConfigDialog::showDialog(QLatin1String("settings")))
        return;
    KConfigDialog *dialog = new KConfigDialog(this, QLatin1String("settings"), Settings::self());
    dialog->addPage(new KgThemeSelector(m_view->getProvider()), i18n("Theme"), QLatin1String("games-config-theme"));
    dialog->addPage(new CustomGameConfig(dialog), i18n("Custom Game"), QLatin1String("games-config-custom"));
    dialog->show();
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

    bool isWrapped = (l == KgDifficultyLevel::VeryHard);
    if (Kg::difficultyLevel() == KgDifficultyLevel::Custom)
        isWrapped = Settings::wrapping();
    const QSize size = boardSize();
    m_view->startNewGame(size.width(), size.height(), (Wrapping)isWrapped);
    m_clickCount = -m_view->minimumMoves();
    m_gameClock->restart();

    if(m_pauseAction->isChecked())
    {
        m_pauseAction->setChecked(false);
    }
    m_pauseAction->setEnabled(true);
    m_solveAction->setEnabled(true);
    Kg::difficulty()->setGameRunning(false);

    updateStatusBar();
}

void MainWindow::gameOver(QVariant msg)
{
    m_gameClock->pause();
    m_pauseAction->setEnabled(false);
    m_solveAction->setEnabled(false);
    Kg::difficulty()->setGameRunning(false);

    if (msg.toString() != "won")
        return;

    if(Settings::playSounds())
        m_soundWin->start();

    //=== calculate the score ====//

    double penalty = m_gameClock->seconds() / 2.0 * (m_clickCount/2 + 1);

    // normalize the penalty
    penalty = std::sqrt(penalty/m_view->cellCount());

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
    pause(paused);
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

QSize MainWindow::boardSize()
{
    switch (Kg::difficultyLevel()) {
    case KgDifficultyLevel::Easy: return QSize(5, 5);
    case KgDifficultyLevel::Medium: return QSize(7, 7);
    case KgDifficultyLevel::Hard: return QSize(9, 9);
    case KgDifficultyLevel::Custom: return QSize(Settings::width(), Settings::height());
    default: return QSize(9, 9);
    }
}

#include "mainwindow.moc"
