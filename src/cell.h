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

#ifndef CELL_H
#define CELL_H

#include <QWidget>

#include <KSvgRenderer>

#include "abstractgrid.h"

class QPixmap;
class QTimeLine;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;

class Cell : public QWidget, public AbstractCell
{
Q_OBJECT
public:
    Cell(QWidget* parent, int index);
    ~Cell();
    
    virtual void makeEmpty();
    
    void rotate(int a);
    void animateRotation(bool toLeft);
    void setLocked(bool newlocked=true);
    virtual void setConnected(bool isConnected);
    bool isRotated() const;
    bool isLocked() const;
    
    // marks the cache as invalid
    void setInvalidCache();

    // keyboard mode
    void toggleKeyboardMode(bool useKeyboard);
    void activateForHover(bool activate);

private slots:
    // used by the animation
    // angle is relative to angleStart
    void rotateStep(int angle);

signals:
    void lClicked(int);
    void rClicked(int);
    void mClicked(int);
    void connectionsChanged();
    
protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void resizeEvent(QResizeEvent*);
    
private:
    void paintForground();
    // paints the forground, the cables and the background on the pixmap
    void paintOnCache();
    
    typedef QMap<int, QString> NamesMap;
    int     angle;
    int     light;
    bool    cableChanged;
    bool    forgroundChanged;
    bool    locked;
    QPixmap *pixmapCache;
    QPixmap *forgroundCache;
    
    // used by the animation of the rotation
    int rotationStart;
    int totalRotation;
    QTimeLine *timeLine;
    
    // used in keyboard mode
    bool m_useKeyboard;
    bool m_cellIsActivated;  // highlighted like hover effect
};

#endif
