/***************************************************************************
 *   Copyright (C) 2004, 2005 Andi Peredri                                 *
 *   andi@ukr.net                                                          *
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

#ifndef CELL_H
#define CELL_H

#include <QWidget>

#include <KSvgRenderer>

class QPixmap;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;

class Cell : public QWidget
{
    Q_OBJECT
    public:
            // Free or None means it's not used, if it has only one direction
            // it's a computer or a server
        enum Dirs { Free = 0, U = 1, R = 2, D = 4, L = 8, None = 16 };
        Cell(QWidget* parent, int i);
        ~Cell();
        int  index() const;
        void rotate(int a);
        void setDirs(Dirs d);
        void setRoot(bool b);
        void setLight(int l);
        void setConnected(bool b);
        void setLocked( bool newlocked = true );
        bool isConnected() const;
        bool isRotated() const;
        bool isLocked() const;
        Dirs dirs() const;
        static void initPixmaps();
    signals:
        void lClicked(int);
        void rClicked(int);
        void mClicked(int);
    protected:
        virtual void paintEvent(QPaintEvent*);
        virtual void mousePressEvent(QMouseEvent*);
        virtual void resizeEvent(QResizeEvent*);
    private:
        typedef QMap<int, QString> NamesMap;
        static NamesMap directionNames;
        static KSvgRenderer allSvg;
        int     angle;
        int     light;
        int     iindex;
        bool    connected;
        bool    cableChanged;
        bool    forgroundChanged;
        bool    root;
        bool    locked;
        Dirs    ddirs;
        // TODO: any reason for the following being pointers
        QPixmap *pixmapCache;
        QPixmap *forgroundCache;
};

#endif
