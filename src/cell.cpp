/*
    Copyright 2004-2005 Andi Peredri <andi@ukr.net> 
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

#include "cell.h"

#include <QTimeLine>
#include <QPen>
#include <KGameRenderedItem>
#include <KColorUtils>

#include "globals.h"
#include "scene.h"

const QHash<int, QByteArray> Cell::s_directionNames = Cell::fillNameHash();

Cell::Cell(QGraphicsItem* parent, int index) 
    : QGraphicsRectItem(parent), AbstractCell(index), m_cablesItem(NULL), m_hostItem(NULL),
      m_hidden(false), m_locked(false), m_activated(false)
{
    setPen(QPen(Qt::transparent));

    timeLine = new QTimeLine(AnimationTime, this);
    timeLine->setCurveShape(QTimeLine::EaseOutCurve);
    timeLine->setUpdateInterval(30);
    connect(timeLine, SIGNAL(frameChanged(int)), SLOT(rotateStep(int)));
}

void Cell::makeEmpty()
{
    AbstractCell::makeEmpty();
    if (m_cablesItem != NULL) {
        delete m_cablesItem;
        m_cablesItem = NULL;
    }
    if (m_hostItem != NULL) {
        delete m_hostItem;
        m_hostItem = NULL;
    }
    setLocked(false);
}

void Cell::setupSprites()
{
    if (cables() != None) {
        m_cablesItem = new KGameRenderedItem(Renderer::self(), "", this);
        m_cablesItem->setTransformationMode(Qt::SmoothTransformation);
    }

    if (isServer() || isTerminal()) {
        m_hostItem = new KGameRenderedItem(Renderer::self(), "", this);
    }

    updateSprites();
}

void Cell::updateSprites()
{
    if (m_cablesItem != NULL) {
        QByteArray spriteKey = isConnected() ? "cablecon" : "cable";
        spriteKey += s_directionNames[cables()];
        m_cablesItem->setSpriteKey(spriteKey);
    }

    if (m_hostItem != NULL) {
        QByteArray spriteKey;
        if (isServer()) {
            spriteKey = "server";
        } else if (isTerminal()) {
            spriteKey = isConnected() ? "computer2" : "computer1";
        }
        m_hostItem->setSpriteKey(spriteKey);
    }
}

void Cell::updateColor()
{
    static const QColor combined = KColorUtils::mix(LockedCellColor, HoveredCellColor, 0.25);
    QColor color = Qt::transparent;

    if(!m_hidden) {
        if(m_locked && m_activated) {
            color = combined;
        } else if(m_locked) {
            color = LockedCellColor;
        } else if(m_activated) {
            color = HoveredCellColor;
        }
    }
    setBrush(color);
}

void Cell::setConnected(bool isConnected)
{
    AbstractCell::setConnected(isConnected);
    updateSprites();
}

void Cell::setCablesHidden(bool hidden)
{
    if(m_hidden == hidden) return;
    m_hidden = hidden;
    updateColor();
    if(m_cablesItem != NULL) {
        m_cablesItem->setVisible(!hidden);
    }
}

void Cell::setLocked(bool locked)
{
    if ( m_locked == locked ) return;
    m_locked = locked;
    updateColor();
}

void Cell::setActivated(bool activate)
{
    if (m_activated == activate) return;
    m_activated = activate;
    updateColor();
}

void Cell::animateRotation(bool clockWise) 
{
    if(clockWise) {
        rotateClockwise();
    } else {
        rotateCounterclockwise();
    }
    updateSprites();

    // if there is already an animation running make a new animition
    // taking into account also the new click
    if (timeLine->state() == QTimeLine::Running) {
        timeLine->setFrameRange(timeLine->currentFrame() + (clockWise ? -90 : 90), 0);
    } else {
        timeLine->setFrameRange(clockWise ? -90 : 90, 0);
    }

    timeLine->stop();
    timeLine->setCurrentTime(0);
    rotateStep(timeLine->currentFrame());
    timeLine->start();
}

void Cell::rotateStep(int a)
{
    if (m_cablesItem != NULL) {
        m_cablesItem->setRotation(a);
    }

    if(a == 0) {
        emit connectionsChanged();
    }
}


void Cell::setSize(const QSizeF& size)
{
    setRect(0, 0, size.width(), size.height());
    if (m_cablesItem != NULL) {
        m_cablesItem->setRenderSize(size.toSize());
        m_cablesItem->setTransformOriginPoint(size.width()/2.0, size.height()/2.0);
    }
    if (m_hostItem != NULL) {
        QSizeF foregroundSize = size*(1.0 - CellForegroundBorder*2);
        m_hostItem->setRenderSize(foregroundSize.toSize());
        m_hostItem->setPos(size.width()*CellForegroundBorder, size.height()*CellForegroundBorder);
    }
}

QHash<int, QByteArray> Cell::fillNameHash()
{
    QHash<int, QByteArray> directionNames;
    directionNames[Left]            = "0001";
    directionNames[Down]            = "0010";
    directionNames[Down|Left]       = "0011";
    directionNames[Right]           = "0100";
    directionNames[Right|Left]      = "0101";
    directionNames[Right|Down]      = "0110";
    directionNames[Right|Down|Left] = "0111";
    directionNames[Up]              = "1000";
    directionNames[Up|Left]         = "1001";
    directionNames[Up|Down]         = "1010";
    directionNames[Up|Down|Left]    = "1011";
    directionNames[Up|Right]        = "1100";
    directionNames[Up|Right|Left]   = "1101";
    directionNames[Up|Right|Down]   = "1110";
    return directionNames;
}

#include "cell.moc"
