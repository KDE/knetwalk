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
#include "renderer.h"

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
        kDebug() << "Painting empty area";
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
    if (root)
        *forgroundCache = Renderer::self()->computerPixmap(width(), root, isConnected());
    else if(ddirs == U || ddirs == L || ddirs == D || ddirs == R)
        // if the cell has only one direction and isn't a server
        *forgroundCache = Renderer::self()->computerPixmap(width(), root, isConnected());
    else 
        forgroundCache->fill(Qt::transparent);
}

void Cell::paintOnCache()
{   
    if (locked) pixmapCache->fill(LockedCellColor);
    else pixmapCache->fill(Qt::transparent);
    
    QPixmap cable( Renderer::self()->cablesPixmap(width(), ddirs, isConnected()) );
    QPainter painter(pixmapCache);
    
    if(angle)
    {
        qreal offset = width() / 2.0;
        painter.translate(offset, offset);
        painter.rotate(angle);
        painter.translate(-offset, -offset);
    }
    
    painter.drawPixmap(0, 0, cable);
    painter.resetMatrix();
    
    painter.drawPixmap(0, 0, *forgroundCache);
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
