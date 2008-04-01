/*
    Copyright 2004-2005 Andi Peredri <andi@ukr.net> 
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

Cell::Cell(QWidget* parent, int index) 
    : QWidget(parent), AbstractCell(index)
{
    pixmapCache = new QPixmap(width(), height());
    forgroundCache = new QPixmap(width(), height());
    
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

void Cell::setInvalidCache()
{
    forgroundChanged = true;
}

void Cell::makeEmpty()
{
    AbstractCell::makeEmpty();
    angle = 0;
    light = 0;
    locked = false;
    forgroundChanged = true;
}

bool Cell::isRotated() const
{
    return angle;
}

bool Cell::isLocked() const
{
    return locked;
}

void Cell::setLocked(bool newlocked)
{
    if ( locked == newlocked ) return;
    locked = newlocked;
    forgroundChanged = true;
    update();
}

void Cell::setConnected(bool isConnected)
{
    AbstractCell::setConnected(isConnected);
    forgroundChanged = true;
}

void Cell::paintEvent(QPaintEvent*)
{
    if (width() == 0 || height() == 0) {
        kDebug() << "Painting empty area";
        return;
    }
    
    if (forgroundChanged) {
        // paint the terminals or server on the forgroundCache
        paintForground();
    }
    
    if (cables() == None /*|| cables() == Free*/) {
        *pixmapCache = *forgroundCache;
    } else if (forgroundChanged || cableChanged) {
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
    if (isServer() || isTerminal()) {
        *forgroundCache = Renderer::self()->computerPixmap(width(), isServer(), 
                                                           isConnected());
    } else { 
        forgroundCache->fill(Qt::transparent);
    }
}

void Cell::paintOnCache()
{   
    if (locked) {
        pixmapCache->fill(LockedCellColor);
    } else {
        pixmapCache->fill(Qt::transparent);
    }
    
    
    QPixmap cable(Renderer::self()->cablesPixmap(width(),
                  cables(), isConnected()));
    QPainter painter(pixmapCache);
    
    if (angle != 0) {
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
    if (e->button() == Qt::LeftButton) {
        emit lClicked(index());
    } else if (e->button() == Qt::RightButton) {
        emit rClicked(index());
    } else if (e->button() == Qt::MidButton) {
        emit mClicked(index());
    }
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
    if (timeLine->state() == QTimeLine::Running) {
        totalRotation += clockWise ? 90 : -90;
        
        timeLine->setFrameRange(timeLine->currentFrame(), totalRotation);
        timeLine->stop();
        timeLine->setCurrentTime(0);
        
        timeLine->start();
    } else {
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
        repaint();
        emit connectionsChanged();
    }
}

void Cell::rotate(int a)
{
    angle += a;
    
    while (angle > 45) {
        angle -= 90;
        rotationStart -= 90;
        rotateClockwise();
    }
    
    while (angle < -45) {
        angle += 90;
        rotationStart += 90;
        rotateCounterclockwise();
    }
    
    cableChanged = true;
    update();
}

#include "cell.moc"
