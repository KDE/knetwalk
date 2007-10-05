/***************************************************************************
 *   Copyright (C) 2004, 2005 Andi Peredri <andi@ukr.net>                  *
 *   Copyright (C) 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation (see COPYING)            *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <KSvgRenderer>

#include <QList>

class Cell;
class QAction;
class QSound;
class QLCDNumber;
class QCloseEvent;
class QResizeEvent;
class QPixmap;
class QGridLayout;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent=0);
        virtual ~MainWindow();
    protected:
        void setupActions();
        void createBoard();
        virtual void closeEvent(QCloseEvent*);
        virtual void paintEvent(QPaintEvent*);
        virtual void resizeEvent(QResizeEvent*);
    private:
        enum BoardSize
        {
            NoviceBoardSize = 5,
            NormalBoardSize = 7,
            ExpertBoardSize = 9,
            MasterBoardSize = 9
        };
        enum
        {
            NumHighscores   = 10,
            MinimumNumCells = 20
        };
        typedef QList<Cell*> CellList;
    public slots:
        void  slotNewGame();

        void  lClicked(int index);
        void  rClicked(int index);
        void  mClicked(int index);

        void  showHighscores();
        void  configureNotifications();

    private:
        Cell* uCell(Cell* cell) const;
        Cell* dCell(Cell* cell) const;
        Cell* lCell(Cell* cell) const;
        Cell* rCell(Cell* cell) const;
        bool  isGameOver();
        bool  startBrowser(const QString& url);
        bool  updateConnections();
        void  blink(int index);
        void  rotate(int index, bool toleft);
        void  addRandomDir(CellList& list);
        void  dialog(const QString& caption, const QString& text);
        int boardSize();
        void setBoardSize(int size);
    private:
        bool        wrapped;
        Cell*       root;
        Cell*       board[MasterBoardSize * MasterBoardSize];
        QGridLayout* gridLayout;

        QSound*     clicksound;
        QSound*     connectsound;
        QSound*     startsound;
        QSound*     turnsound;
        QSound*     winsound;

        QString     username;
        QString     soundpath;
        QAction*    soundaction;
        QStringList highscores;
        QLCDNumber* lcd;

        int m_clickcount;
        KSvgRenderer m_background;

        QPixmap *pixmapCache;
        bool m_invalidCache;
};

#endif // MAINWINDOW_H
