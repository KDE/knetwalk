/*
    Copyright 2004-2005 Andi Peredri <andi@ukr.net>   
    Copyright 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch> 
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com> 
    Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
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
    explicit MainWindow(QWidget *parent=0);
private:
    enum StatusBarIndex {
        StatusBarIndexMoves = 0,
        StatusBarIndexTime = 1
    };
    QSize boardSize();
signals:
    void pause(const QVariant &paused);
private slots:
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
