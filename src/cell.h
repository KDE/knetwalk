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

class Cell : public QWidget
{
Q_OBJECT
public:
    // Free or None means it's not used, 
    // if it has only one direction it's a terminal or a server
    Cell(QWidget* parent, int i);
    ~Cell();
    int  index() const;
    void rotate(int a);
    void animateRotation(bool toLeft);
    void setDirs(Directions d);
    void setRoot(bool b);
    void setLight(int l);
    void setConnected(bool b);
    void setLocked( bool newlocked = true );
    bool isConnected() const;
    bool isRotated() const;
    bool isLocked() const;
    Directions dirs() const;
    static void initPixmaps();

private slots:
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
    Directions    ddirs;
    QPixmap *pixmapCache;
    QPixmap *forgroundCache;
    
    // used by the animation of the rotation
    int rotationStart;
    int totalRotation;
    QTimeLine *timeLine;
};

#endif
