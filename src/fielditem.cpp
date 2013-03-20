/*
    Copyright 2010 Brian Croom <brian.s.croom@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "fielditem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include <KgSound>
#include <KStandardDirs>

#include "settings.h"

FieldItem::FieldItem()
    : m_terminalsConnectedEmitted(false), m_activeCell(-1)
{
    setAcceptHoverEvents(true);

    m_soundTurn = new KgSound(KStandardDirs::locate("appdata", "sounds/turn.wav"));
    m_soundClick = new KgSound(KStandardDirs::locate("appdata", "sounds/click.wav"));
    m_soundConnect = new KgSound(KStandardDirs::locate("appdata", "sounds/connect.wav"));
}

void FieldItem::resize(const QSizeF& size)
{
    prepareGeometryChange();
    m_cellSize = size.width() / qMax(width(), height());
    repositionCells();
}

void FieldItem::initField(uint width, uint height, Wrapping w)
{
    m_gameRunning = true;
    m_gameEnded = false;
    initializeGrid(width, height, w);
    for(int i=0; i<cellCount(); i++) {
        Cell* cell = cellAt(i);
        cell->setupSprites();

        cell->disconnect();
        // called when a rotation ends
        connect(cellAt(i), SIGNAL(connectionsChanged()),
                            SLOT(updateConnections()));
    }
    // highlight current cell in keyboard mode
    activateCell(0);
    repositionCells();
}

void FieldItem::repositionCells()
{
    if(width() == 0 || height() == 0) {
        return;
    }

    for(int row=0; row<height(); row++) {
        for(int col=0; col<width(); col++) {
            Cell* cell = cellAt(col + row*width());
            cell->setPos(col*m_cellSize, row*m_cellSize);
            cell->setSize(QSize(m_cellSize+1, m_cellSize+1));
        }
    }
}

void FieldItem::setGamePaused(bool paused)
{
    m_gameRunning = (!paused && !m_gameEnded);

    for(int i=0; i<cellCount(); i++) {
        cellAt(i)->setCablesHidden(paused);
    }
}

QRectF FieldItem::boundingRect() const
{
    return QRectF(0, 0, m_cellSize*width(), m_cellSize*height());
}

void FieldItem::kbGoRight()
{
    activateCell(qMin(m_activeCell+1, cellCount()-1));
}

void FieldItem::kbGoLeft()
{
    activateCell(qMax(m_activeCell-1, 0));
}

void FieldItem::kbGoUp()
{
    activateCell(m_activeCell < width() ? m_activeCell : m_activeCell-width());
}

void FieldItem::kbGoDown()
{
    activateCell(m_activeCell+width() >= cellCount() ? m_activeCell : m_activeCell+width());
}

void FieldItem::kbTurnClockwise()
{
    rClicked(m_activeCell);
}

void FieldItem::kbTurnCounterclockwise()
{
    lClicked(m_activeCell);
}

void FieldItem::kbLock()
{
    mClicked(m_activeCell);
}

void FieldItem::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
    int index = indexFromPos(ev->pos());

    if(index < 0) {
        return;
    }

    if(ev->button() == Qt::LeftButton) {
        lClicked(index);
    } else if(ev->button() == Qt::RightButton) {
        rClicked(index);
    } else if(ev->button() == Qt::MidButton) {
        mClicked(index);
    }
}

void FieldItem::hoverEnterEvent(QGraphicsSceneHoverEvent* ev)
{
    activateCell(indexFromPos(ev->pos()));
}
void FieldItem::hoverMoveEvent(QGraphicsSceneHoverEvent* ev)
{
    activateCell(indexFromPos(ev->pos()));
}

void FieldItem::lClicked(int index)
{
    if(m_gameRunning) {
        rotate(index, false);
    }
}

void FieldItem::rClicked(int index)
{
    if(m_gameRunning) {
        rotate(index, true);
    }
}

void FieldItem::mClicked(int index)
{
    if(m_gameRunning && index >= 0) {
        cellAt(index)->setLocked(!cellAt(index)->isLocked());
    }
}

void FieldItem::rotate(int index, bool clockWise)
{
    if(index < 0) {
        return;
    }

    const Directions d = cellAt(index)->cables();

    if ((d == None) || m_gameEnded || cellAt(index)->isLocked()) {
        if(Settings::playSounds())
            m_soundClick->start();
    } else {
        if(Settings::playSounds())
            m_soundTurn->start();

        cellAt(index)->animateRotation(clockWise);

        // FIXME: won't work!!!
        //if (updateConnections())
        //    m_soundConnect->start();

        emit rotationPerformed();
    }
}


void FieldItem::updateConnections()
{
    QList<int> changedCells = AbstractGrid::updateConnections();
    checkIfGameEnded();
}

void FieldItem::checkIfGameEnded()
{
	if (m_gameEnded)
		return;

    foreach (AbstractCell* cell, cells()) {
        if (cell->cables() != None && !cell->isConnected()) {
            if (!m_terminalsConnectedEmitted && allTerminalsConnected()) {
                emit terminalsConnected();
                m_terminalsConnectedEmitted = true;
            }
            return;
        }
    }

    activateCell(-1);
    m_gameEnded = true;
    m_gameRunning = false;
    emit gameWon();
}

void FieldItem::activateCell(int index)
{
    if(index == m_activeCell || !m_gameRunning) {
        return;
    }

    if(m_activeCell >= 0 && m_activeCell < cellCount()) {
        cellAt(m_activeCell)->setActivated(false);
    }
    m_activeCell = index;
    if(m_activeCell >= 0 && m_activeCell < cellCount()) {
        cellAt(m_activeCell)->setActivated(true);
    }
}

int FieldItem::indexFromPos(const QPointF& pos)
{
    int row = pos.y()/m_cellSize;
    int col = pos.x()/m_cellSize;

    if(row < 0 || row >= height() || col < 0 || col >= width()) {
        return -1;
    }

    return col + row*width();
}

Cell *FieldItem::cellAt(int index)
{
    return static_cast<Cell *>(cells()[index]);
}

#include "fielditem.moc"
