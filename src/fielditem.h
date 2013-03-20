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
#ifndef FIELDITEM_H
#define FIELDITEM_H

#include "cell.h"

class KgSound;

class FieldItem : public QGraphicsObject, public AbstractGrid
{
    Q_OBJECT
public:
    FieldItem();

    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = 0) {};

    QRectF boundingRect() const;

    void resize(const QSizeF& size);

    void initField(uint width, uint height, Wrapping w=NotWrapped);

    void setGamePaused(bool paused);

    bool gameEnded() const {return m_gameEnded;}
signals:
    void rotationPerformed();
    void terminalsConnected(); // terminals connected but disconnected cables
    void gameWon();
public slots:
    // slots for keyboard mode
    void kbGoRight();
    void kbGoLeft();
    void kbGoUp();
    void kbGoDown();
    void kbTurnClockwise();
    void kbTurnCounterclockwise();
    void kbLock();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    void hoverMoveEvent(QGraphicsSceneHoverEvent*);

    virtual Cell *newCell(int index) {return new Cell(this, index);}
private slots:
    // called respectively when the 
    // left, right or middle mouse buttons are pressed
    void lClicked(int index);
    void rClicked(int index);
    void mClicked(int index);

    // must not be virtual
    void updateConnections();
private:
    int indexFromPos(const QPointF& pos);
    Cell *cellAt(int index);

    void activateCell(int index);

    void checkIfGameEnded();
    void rotate(int index, bool clockWise);

    void repositionCells();

    bool m_gameRunning; // false when paused or game over
    bool m_gameEnded;
    bool m_terminalsConnectedEmitted;

    qreal m_cellSize;

    // highlighted cell, from mouse or keyboard input
    int m_activeCell;

    KgSound *m_soundTurn;
    KgSound *m_soundClick;
    KgSound *m_soundConnect;
};

#endif
