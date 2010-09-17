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

#ifndef CELL_H
#define CELL_H

#include <QGraphicsRectItem>


#include "abstractgrid.h"

class QTimeLine;
class QMouseEvent;
class QResizeEvent;
class KGameRenderedItem;

class Cell : public QObject, public QGraphicsRectItem, public AbstractCell
{
Q_OBJECT
public:
    Cell(QGraphicsItem* parent, int index);
    
    virtual void makeEmpty();

    // called after the AbstractCell properties have been set
    void setupSprites();

    void animateRotation(bool clockWise);
    virtual void setConnected(bool isConnected);

    void setCablesHidden(bool hidden);
    void setLocked(bool locked=true);
    bool isLocked() const {return m_locked;}
    void setActivated(bool activate);

    void setSize(const QSizeF& size);
private slots:
    // used by the animation
    // angle is relative to angleStart
    void rotateStep(int angle);

signals:
    void connectionsChanged();
    
private:
    void updateColor();
    void updateSprites();

    static const QHash<int, QByteArray> s_directionNames;
    static QHash<int, QByteArray> fillNameHash();

    KGameRenderedItem* m_cablesItem;
    KGameRenderedItem* m_hostItem;

    bool m_hidden;
    bool m_locked;
    bool m_activated;  // highlighted like hover effect
    
    // used by the animation of the rotation
    QTimeLine *timeLine;
};

#endif
