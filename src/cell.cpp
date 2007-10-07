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
#include <QTimeLine>

#include <KGlobal>
#include <KIconLoader>
#include <KStandardDirs>
#include <KSvgRenderer>
#include <KDebug>

#include "consts.h"

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
    
    timeLine = new QTimeLine(AnimationTime, this);
    timeLine->setCurveShape(QTimeLine::EaseOutCurve);
    //timeLine->setUpdateInterval(80);
    connect(timeLine, SIGNAL(frameChanged(int)), SLOT(rotateStep(int)));
}

Cell::~Cell()
{
    delete pixmapCache;
    delete forgroundCache;
    delete timeLine;
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
    // animating cables are never connected
    //if (timeLine->state() == QTimeLine::Running) return false;
    
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
    
    if (forgroundChanged)
    {
        // paint the terminals or server on the forgroundCache
        paintForground();
    }
    if (ddirs == None || ddirs == Free) {
        *pixmapCache = *forgroundCache;
    }
    else if (forgroundChanged || cableChanged) {
        // paints everything on the cache
        paintOnCache();
    }
    
    
    QPainter painter;
    painter.begin(this);
    
    // light on hover
    if (underMouse() && !locked) {
        painter.setBrush(HoveredCellColor);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, pixmapCache->width(), pixmapCache->height());
    }
    
    painter.drawPixmap(0, 0, *pixmapCache);
    painter.end();
    
    forgroundChanged = false;
    cableChanged = false;
}

void Cell::paintForground()
{
    QPainter painter;
    forgroundCache->fill(QColor(0, 0, 0, 0));
    painter.begin(forgroundCache);

    /*if ( locked ) {
        allSvg.render(&painter, "background-locked");
    } else {
        allSvg.render(&painter, "background");
    }

    if (light)
    {
        painter.setPen(QPen(Qt::white, 5));
        painter.drawLine(0, width() - light, width(), 2 * width() - light);
    }*/

    
    int w = pixmapCache->width();
    int h = pixmapCache->height();
    const qreal ratio = 1.0 - CellForgroundBorder*2;
    QRectF boundingRect(CellForgroundBorder * w, CellForgroundBorder * h, 
                            ratio * w, ratio * h);
    if(root)
    {
        allSvg.render(&painter, "server", boundingRect);
    }
    // if the cell has only one direction and isn't a server
    else if(ddirs == U || ddirs == L || ddirs == D || ddirs == R)
    {
        if(isConnected())
            allSvg.render(&painter, "computer2", boundingRect);
        else
            allSvg.render(&painter, "computer1", boundingRect);
    }
    painter.end();
}

void Cell::paintOnCache()
{
    QPainter painter;
    if (locked) pixmapCache->fill(LockedCellColor);
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
    const qreal ratio = 1.0 - CellBorder*2;
    QRectF boundingRect(CellBorder * w, CellBorder * h, 
                        ratio * w, ratio * h);
    
    if(isConnected())
        allSvg.render(&painter, "cablecon" + directionNames[ddirs], boundingRect);
    else
        allSvg.render(&painter, "cable" + directionNames[ddirs], boundingRect);
    
    painter.resetMatrix();
    
    painter.drawPixmap(0, 0, *forgroundCache);
    painter.end();
}

void Cell::mousePressEvent(QMouseEvent* e)
{
    // do nothing if there is an animation running
    //if (timeLine->state() == QTimeLine::Running) return;
    
    if (e->button() == Qt::LeftButton)
        emit lClicked(iindex);
    else if (e->button() == Qt::RightButton)
        emit rClicked(iindex);
    else if (e->button() == Qt::MidButton)
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

void Cell::animateRotation(bool clockWise) 
{
    // if there is already an animation running make a new animition
    // taking into account also the new click
    if (timeLine->state() == QTimeLine::Running) 
    {
        totalRotation += clockWise ? 90 : -90;
        
        timeLine->setFrameRange(timeLine->currentFrame(), totalRotation);
        timeLine->stop();
        timeLine->setCurrentTime(0);
        
        timeLine->start();
    }
    else 
    {
        rotationStart = angle;
        totalRotation = clockWise ? 90 : -90;
        
        timeLine->setFrameRange(0, totalRotation);
        
        timeLine->start();
    }
}

void Cell::rotateStep(int a)
{
    kDebug() << a << endl;
    
    int newAngle = rotationStart + a;
    rotate(newAngle - angle);
    
    if (a == totalRotation) {
        emit connectionsChanged();
    }
}

void Cell::rotate(int a)
{
    angle += a;
    
    while (angle > 45)
    {
        angle -= 90;
        rotationStart -= 90;
        int newdirs = Free;
        if (ddirs & U) newdirs |= R;
        if (ddirs & R) newdirs |= D;
        if (ddirs & D) newdirs |= L;
        if (ddirs & L) newdirs |= U;
        setDirs(Dirs(newdirs));
    }
    while (angle < -45)
    {
        angle += 90;
        rotationStart += 90;
        int newdirs = Free;
        if (ddirs & U) newdirs |= L;
        if (ddirs & R) newdirs |= U;
        if (ddirs & D) newdirs |= R;
        if (ddirs & L) newdirs |= D;
        setDirs(Dirs(newdirs));
    }
    
    cableChanged = true;
    update();
}

#include "cell.moc"
