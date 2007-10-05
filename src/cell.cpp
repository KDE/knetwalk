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

#include "cell.h"

#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QPaintEvent>

#include <KGlobal>
#include <KIconLoader>
#include <KStandardDirs>
#include <KSvgRenderer>
#include <KDebug>

Cell::NamesMap Cell::directionNames;
KSvgRenderer Cell::allSvg;

void Cell::initPixmaps()
{
    directionNames[L]     = "0001";
    directionNames[D]     = "0010";
    directionNames[D|L]   = "0011";
    directionNames[R]     = "0100";
    directionNames[R|L]   = "0101";
    directionNames[R|D]   = "0110";
    directionNames[R|D|L] = "0111";
    directionNames[U]     = "1000";
    directionNames[U|L]   = "1001";
    directionNames[U|D]   = "1010";
    directionNames[U|D|L] = "1011";
    directionNames[U|R]   = "1100";
    directionNames[U|R|L] = "1101";
    directionNames[U|R|D] = "1110";
    allSvg.load( KStandardDirs::locate( "data","knetwalk/all.svgz" ) );
}

Cell::Cell(QWidget* parent, int i) : QWidget(parent)
{
    angle     = 0;
    light     = 0;
    iindex    = i;
    ddirs     = Free;
    connected = false;
    root      = false;
    locked    = false;
    pixmapCache = new QPixmap(width(), height());
    forgroundCache = new QPixmap(width(), height());
    
    forgroundChanged = true;
    setAttribute(Qt::WA_Hover, true);
}

Cell::~Cell()
{
    delete pixmapCache;
    delete forgroundCache;
}

int Cell::index() const
{
    return iindex;
}

Cell::Dirs Cell::dirs() const
{
    return ddirs;
}

bool Cell::isConnected() const
{
    return connected;
}

bool Cell::isRotated() const
{
    return angle;
}

bool Cell::isLocked() const
{
    return locked;
}

void Cell::setLocked( bool newlocked )
{
    if ( locked == newlocked ) return;
    locked = newlocked;
    forgroundChanged = true;
    update();
}


void Cell::setDirs(Dirs d)
{
    if(ddirs == d) return;
    ddirs = d;
    forgroundChanged = true;
    update();
}

void Cell::setConnected(bool b)
{
    if(connected == b) return;
    connected = b;
    forgroundChanged = true;
    update();
}

void Cell::setRoot(bool b)
{
    if(root == b) return;
    root = b;
    cableChanged = true;
    if (!(ddirs & None)) 
       forgroundChanged = true;
    update();
}

void Cell::setLight(int l)
{
    light = l;
    forgroundChanged = true;
    update();
}

void Cell::paintEvent(QPaintEvent*)
{
    if (width() == 0 || height() == 0) {
        kDebug() << "Painting empty area" << endl;
        return;
    }
        
    QPainter painter;
    if (forgroundChanged)
    {
        forgroundCache->fill(QColor(0, 0, 0, 0));
        painter.begin(forgroundCache);

        /*if ( locked ) {
            allSvg.render(&painter, "background-locked");
        } else {
            allSvg.render(&painter, "background");
        }

        if(light)
        {
            painter.setPen(QPen(Qt::white, 5));
            painter.drawLine(0, width() - light, width(), 2 * width() - light);
        }*/

        
        int w = pixmapCache->width();
        int h = pixmapCache->height();
        const qreal ratio = 0.8;
        QRectF boundingRect((1.0-ratio)/2 * w, (1.0-ratio)/2 * h, 
                             ratio * w, ratio * h);
        if(root)
        {
            allSvg.render(&painter, "server", boundingRect);
        }
        // if the cell has only one direction and isn't a server
        else if(ddirs == U || ddirs == L || ddirs == D || ddirs == R)
        {
            if(connected)
                allSvg.render(&painter, "computer2", boundingRect);
            else
                allSvg.render(&painter, "computer1", boundingRect);
        }
        painter.end();
    }
    if (ddirs & None) {
        *pixmapCache = *forgroundCache;
    }
    else if (forgroundChanged || cableChanged) {
        if (locked) pixmapCache->fill(QColor(0, 0, 0, 100));
        else pixmapCache->fill(QColor(0, 0, 0, 0));
        
        painter.begin(pixmapCache);
        
        if(angle)
        {
            double woffset, hoffset;
            woffset = width() / 2;
            hoffset = height() / 2;
            painter.translate(woffset, hoffset);
            painter.rotate(angle);
            painter.translate(-woffset, -hoffset);
        }
        
        int w = pixmapCache->width();
        int h = pixmapCache->height();
        const qreal ratio = 1.03;
                QRectF boundingRect((1.0-ratio)/2 * w, (1.0-ratio)/2 * h, 
                                     ratio * w, ratio * h);
        
        if(connected)
            allSvg.render(&painter, "cable" + directionNames[ddirs], boundingRect);
        else
            allSvg.render(&painter, "cable" + directionNames[ddirs], boundingRect);
        
        painter.resetMatrix();
        
        painter.drawPixmap(0, 0, *forgroundCache);
        painter.end();
    }
    painter.begin(this);
    if (underMouse() && !locked) {
       painter.setBrush(QColor(255, 255, 255, 30));
       painter.setPen(Qt::NoPen);
       painter.drawRect(0, 0, pixmapCache->width(), pixmapCache->height());
    }
    painter.drawPixmap(0, 0, *pixmapCache);
    painter.end();
    
    forgroundChanged = false;
    cableChanged = false;
}

void Cell::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton)
        emit lClicked(iindex);
    else if(e->button() == Qt::RightButton)
        emit rClicked(iindex);
    else if(e->button() == Qt::MidButton)
        emit mClicked(iindex);
}

void Cell::resizeEvent(QResizeEvent* e)
{
    forgroundChanged = true;
    delete pixmapCache;
    delete forgroundCache;
    pixmapCache = new QPixmap(e->size());
    forgroundCache = new QPixmap(e->size());
}

void Cell::rotate(int a)
{
    angle += a;
    cableChanged = true;
    while(angle >= 45)
    {
        angle -= 90;
        int newdirs = Free;
        if(ddirs & U) newdirs |= R;
        if(ddirs & R) newdirs |= D;
        if(ddirs & D) newdirs |= L;
        if(ddirs & L) newdirs |= U;
        setDirs(Dirs(newdirs));
    }
    while(angle < -45)
    {
        angle += 90;
        int newdirs = Free;
        if(ddirs & U) newdirs |= L;
        if(ddirs & R) newdirs |= U;
        if(ddirs & D) newdirs |= R;
        if(ddirs & L) newdirs |= D;
        setDirs(Dirs(newdirs));
    }
    update();
}

#include "cell.moc"
