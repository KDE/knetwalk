/*
    SPDX-FileCopyrightText: 2004-2005 Andi Peredri <andi@ukr.net>
    SPDX-FileCopyrightText: 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch>
    SPDX-FileCopyrightText: 2007-2008 Fela Winkelmolen <fela.kde@gmail.com>
    SPDX-FileCopyrightText: 2010 Brian Croom <brian.s.croom@gmail.com>
    SPDX-FileCopyrightText: 2013 Ashwin Rajeev <ashwin_rajeev@hotmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>

#include <QList>

class KgSound;
class KGameClock;
class KToggleAction;
class GameView;
class QLabel;

class MainWindow : public KXmlGuiWindow
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
private:
    enum StatusBarIndex {
        StatusBarIndexMoves = 0,
        StatusBarIndexTime = 1
    };
    QSize boardSize();
Q_SIGNALS:
    void pause(const QVariant &paused);
private Q_SLOTS:
    void startNewGame();
    void gameOver(const QVariant &msg);
    void rotationStarted();
    void pauseGame(bool paused);
    void updateStatusBar();

    void showHighscores();
    void configureSettings();
private:
    void setupActions();

    int           m_clickCount;
    KGameClock   *m_gameClock;

    KToggleAction* m_pauseAction;

    GameView *m_view;
    KgSound *m_soundStart;
    KgSound *m_soundWin;
    QLabel *m_timeLabel;
    QLabel *m_movesLabel;
};

#endif // MAINWINDOW_H
