/*
    SPDX-FileCopyrightText: 2004-2005 Andi Peredri <andi@ukr.net>
    SPDX-FileCopyrightText: 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch>
    SPDX-FileCopyrightText: 2007-2008 Fela Winkelmolen <fela.kde@gmail.com>
    SPDX-FileCopyrightText: 2013 Ashwin Rajeev <ashwin_rajeev@hotmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"

#include <KConfigDialog>
#include <KLocalizedString>
#include <KStandardAction>
#include <QAction>
#include <KToggleAction>
#include <KActionCollection>
#include <KGameStandardAction>
#include <QStatusBar>

#include <KGameDifficulty>
#include <KGameHighScoreDialog>
#include <KGameThemeSelector>
#include <KGameRenderer>
#include <KGameClock>
#include <KGameSound>

#include <ctime>
#include <cmath>

#include "ui_general.h"
#include "ui_customgame.h"

#include "globals.h"
#include "gameview.h"
#include "settings.h"
#include "abstractgrid.h"

class GeneralConfig : public QWidget
{
public:
    GeneralConfig(QWidget *parent)
        : QWidget(parent)
    {
        ui.setupUi(this);
    }
private:
    Ui::GeneralConfig ui;
};

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
    connect(m_view, &GameView::gameOver, this, &MainWindow::gameOver);
    connect(m_view, &GameView::rotationStarted, this, &MainWindow::rotationStarted);
    connect(this, SIGNAL(pause(QVariant)), m_view->rootObject(), SLOT(pause(QVariant)));

    m_movesLabel = new QLabel(this);
    m_movesLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel = new QLabel(this);
    m_timeLabel->setAlignment(Qt::AlignCenter);

    statusBar()->insertPermanentWidget(0, m_movesLabel, 1);
    statusBar()->insertPermanentWidget(1, m_timeLabel, 1);

    // Difficulty
    KGameDifficulty::global()->addStandardLevelRange(
        KGameDifficultyLevel::Easy, KGameDifficultyLevel::VeryHard
    );
    KGameDifficulty::global()->addLevel(
                new KGameDifficultyLevel(100, QByteArray("Custom"), i18nc("@item difficulty", "Custom"))
    );
    KGameDifficultyGUI::init(this);
    connect(KGameDifficulty::global(), &KGameDifficulty::currentLevelChanged, this, &MainWindow::startNewGame);


    setCentralWidget(m_view);

    setupActions();

    setupGUI();

    setAutoSaveSettings();

    m_gameClock = new KGameClock(this, KGameClock::FlexibleHourMinSec);
    connect(m_gameClock, &KGameClock::timeChanged, this, &MainWindow::updateStatusBar);

    m_soundStart = new KGameSound(QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("sounds/start.wav")), this);
    m_soundWin = new KGameSound(QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("sounds/win.wav")), this);


    startNewGame();
}

void MainWindow::setupActions()
{
    // Game
    KGameStandardAction::gameNew(this, &MainWindow::startNewGame,
                                 actionCollection());

    m_pauseAction = KGameStandardAction::pause(this, &MainWindow::pauseGame,
                                               actionCollection());
    connect(KGameDifficulty::global(), &KGameDifficulty::gameRunningChanged, m_pauseAction,
            &QAction::setEnabled);

    QAction *action = KGameStandardAction::solve(m_view, &GameView::solve, actionCollection());
    connect(KGameDifficulty::global(), &KGameDifficulty::gameRunningChanged, action, &QAction::setEnabled);

    KGameStandardAction::highscores(this, &MainWindow::showHighscores,
                                    actionCollection());

    KGameStandardAction::quit(this, &MainWindow::close, actionCollection());

    // Settings
    KStandardAction::preferences(this, &MainWindow::configureSettings, actionCollection());

    action = new QAction(i18nc("@action", "&Unlock All"), this);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(unlockAll()));
    connect(KGameDifficulty::global(), &KGameDifficulty::gameRunningChanged, action, &QAction::setEnabled);
    actionCollection()->addAction( QStringLiteral( "unlock_all" ), action);

    action = new QAction(i18nc("@action", "Move Right"), this);
    KActionCollection::setDefaultShortcut(action, Qt::Key_Right);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(kbGoRight()));
    actionCollection()->addAction( QStringLiteral( "kb_go_right" ), action);

    action = new QAction(i18nc("@action", "Move Left"),this);
    KActionCollection::setDefaultShortcut(action, Qt::Key_Left);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(kbGoLeft()));
    actionCollection()->addAction( QStringLiteral( "kb_go_left" ), action);

    action = new QAction(i18nc("@action", "Move Up"),this);
    KActionCollection::setDefaultShortcut(action, Qt::Key_Up);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(kbGoUp()));
    actionCollection()->addAction( QStringLiteral( "kb_go_up" ), action);

    action = new QAction(i18nc("@action", "Move Down"),this);
    KActionCollection::setDefaultShortcut(action, Qt::Key_Down);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(kbGoDown()));
    actionCollection()->addAction( QStringLiteral( "kb_go_down" ), action);

    action = new QAction(i18nc("@action", "Turn Clockwise"),this);
    KActionCollection::setDefaultShortcut(action, Qt::Key_Return);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(rotateClockwise()));
    actionCollection()->addAction( QStringLiteral( "kb_turn_clockwise" ), action);

    action = new QAction(i18nc("@action", "Turn Counterclockwise"),this);
    KActionCollection::setDefaultShortcut(action, Qt::CTRL | Qt::Key_Return);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(rotateCounterclockwise()));
    actionCollection()->addAction( QStringLiteral( "kb_turn_counterclockwise" ), action);

    action = new QAction(i18nc("@action", "Toggle Lock"),this);
    KActionCollection::setDefaultShortcut(action, Qt::Key_Space);
    connect(action, SIGNAL(triggered()), m_view->rootObject(), SLOT(toggleLock()));
    actionCollection()->addAction( QStringLiteral( "kb_lock" ), action);
}

void MainWindow::configureSettings()
{
    if (KConfigDialog::showDialog(QStringLiteral("settings")))
        return;
    KConfigDialog *dialog = new KConfigDialog(this, QStringLiteral("settings"), Settings::self());
    dialog->addPage(new GeneralConfig(dialog), i18nc("@title:tab", "General"), QStringLiteral("games-config-options"));
    dialog->addPage(new KGameThemeSelector(m_view->getProvider()), i18nc("@title:tab", "Theme"), QStringLiteral("games-config-theme"));
    dialog->addPage(new CustomGameConfig(dialog), i18nc("@title:tab", "Custom Game"), QStringLiteral("games-config-custom"));
    connect(dialog, &KConfigDialog::settingsChanged, m_view, &GameView::updateSettings);
//    dialog->setHelp(QString(),QLatin1String("knetwalk"));
    dialog->show();
}

void MainWindow::showHighscores()
{
    KGameHighScoreDialog scoreDialog(KGameHighScoreDialog::Name | KGameHighScoreDialog::Time, this);
    scoreDialog.addField(KGameHighScoreDialog::Custom1, i18n("Moves Penalty"), QStringLiteral( "moves" ));
    scoreDialog.initFromDifficulty(KGameDifficulty::global());
    scoreDialog.exec();
}

void MainWindow::startNewGame()
{
    if(Settings::playSounds())
        m_soundStart->start();

    const KGameDifficultyLevel::StandardLevel l = KGameDifficulty::globalLevel();

    bool isWrapped = (l == KGameDifficultyLevel::VeryHard);
    if (KGameDifficulty::globalLevel() == KGameDifficultyLevel::Custom)
        isWrapped = Settings::wrapping();
    const QSize size = boardSize();
    m_view->startNewGame(size.width(), size.height(), (Wrapping)isWrapped);
    m_clickCount = -m_view->minimumMoves();
    m_gameClock->restart();

    if(m_pauseAction->isChecked())
    {
        m_pauseAction->setChecked(false);
    }
    KGameDifficulty::global()->setGameRunning(true);

    updateStatusBar();
}

void MainWindow::gameOver(const QVariant &msg)
{
    m_gameClock->pause();
    KGameDifficulty::global()->setGameRunning(false);

    if (msg.toString() != QLatin1String("won"))
        return;

    if(Settings::playSounds())
        m_soundWin->start();

    //=== calculate the score ====//

    double penalty = m_gameClock->seconds() / 2.0 * (m_clickCount/2 + 1);

    // normalize the penalty
    penalty = std::sqrt(penalty/m_view->cellCount());

    int score = static_cast<int>(100.0 / penalty);

    // create the new scoreInfo
    KGameHighScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KGameHighScoreDialog::Score].setNum(score);
    scoreInfo[KGameHighScoreDialog::Custom1].setNum(m_clickCount/2);
    scoreInfo[KGameHighScoreDialog::Time] = m_gameClock->timeString();

    // show the new dialog and add the new score to it
    KGameHighScoreDialog scoreDialog(KGameHighScoreDialog::Name | KGameHighScoreDialog::Time, this);
    scoreDialog.addField(KGameHighScoreDialog::Custom1, i18n("Moves Penalty"), QStringLiteral( "moves" ));
    scoreDialog.initFromDifficulty(KGameDifficulty::global());
    bool madeIt = scoreDialog.addScore(scoreInfo);
    if (!madeIt) {
        QString comment = i18np("Your score was %1, you did not make it to the high score list.",
                                "Your score was %1, you did not make it to the high score list.", score);
        scoreDialog.setComment(comment);
    }
    scoreDialog.exec();
}

void MainWindow::rotationStarted()
{
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
    m_movesLabel->setText(moves);
    m_timeLabel->setText(time);
}

QSize MainWindow::boardSize()
{
    switch (KGameDifficulty::globalLevel()) {
    case KGameDifficultyLevel::Easy: return QSize(5, 5);
    case KGameDifficultyLevel::Medium: return QSize(7, 7);
    case KGameDifficultyLevel::Hard: return QSize(9, 9);
    case KGameDifficultyLevel::Custom: return QSize(Settings::width(), Settings::height());
    default: return QSize(9, 9);
    }
}

#include "moc_mainwindow.cpp"
