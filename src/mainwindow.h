/*
    Copyright 2004-2005 Andi Peredri <andi@ukr.net>   
    Copyright 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch> 
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com> 
    Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
  
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

#include "abstractgrid.h"
#include "cell.h"

class KgSound;
class KGameClock;
class KgThemeSelector;
class KNetWalkScene;
class KNetWalkView;
class KToggleAction;

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

    enum BoardSize {
      NoviceBoardSize = 5,
      NormalBoardSize = 7,
      ExpertBoardSize = 9,
      MasterBoardSize = 9
    };
    static int boardSize();

private slots:
    void startNewGame();
    void gameOver();
    void rotationPerformed();
    void pauseGame(bool paused);
    void updateStatusBar();

    void showHighscores();
    void loadSettings();
    void setSounds(bool val);
private:
    void setupActions();

    int           m_clickCount;
    KGameClock   *m_gameClock;

    KToggleAction* m_pauseAction;
    KToggleAction* m_soundAction;

    KNetWalkScene* m_scene;
    KNetWalkView* m_view;
    KgThemeSelector* m_selector;
    KgSound *m_soundStart;
    KgSound *m_soundWin;
};

#endif // MAINWINDOW_H
