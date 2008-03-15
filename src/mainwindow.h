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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <KSvgRenderer>

#include <QList>

#include "abstractgrid.h"
#include "cell.h"

class QAction;
class QSound;
class QLCDNumber;
class QCloseEvent;
class QResizeEvent;
class QPixmap;
class QGridLayout;
class KGameClock;
class KScoreDialog;

class MainWindow : public KXmlGuiWindow, public AbstractGrid
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=0);
    virtual ~MainWindow();
    
    static int boardSize();
    
protected:
    void setupActions();
    void createBoard();
    virtual void closeEvent(QCloseEvent*);
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);
    
    virtual Cell *newCell(int index) {return new Cell(this, index);}
    
protected slots:
    // must not be virtual
    void updateConnections();
    
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
    
    enum {
      NumHighscores   = 10,
      MinimumNumCells = 20
    };
    typedef QList<Cell*> CellList;
    
private slots:
    void startNewGame();
    void updateStatusBar();

    void lClicked(int index);
    void rClicked(int index);
    void mClicked(int index);

    void  showHighscores();
    void  configureNotifications();

private:
    Cell *cellAt(int index);
    void  checkIfGameEnded();
    void  rotate(int index, bool clockWise);
    
private:
    bool          gameEnded;
    int           clickCount;
    KGameClock   *gameClock;
    KScoreDialog *scoreDialog;
    
    QGridLayout* gridLayout;

    QSound*     clicksound;
    QSound*     connectsound;
    QSound*     startsound;
    QSound*     turnsound;
    QSound*     winsound;

    QString     username;
    QString     soundpath;
    QAction*    soundaction;

    QPixmap *pixmapCache;
    bool invalidCache;
};

#endif // MAINWINDOW_H
