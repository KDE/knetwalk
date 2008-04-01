/*
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
 
#ifndef VIEW_H
#define VIEW_H

#include <QFrame>

#include "mainwindow.h"

class View : public QFrame 
{
public:
    View(QWidget *parent=0) : QFrame(parent) {
        setFrameStyle(QFrame::NoFrame);
        setMinimumSize(MinimumWidth, MinimumHeight);
    }

protected:
    void paintEvent(QPaintEvent *e) {
    //void resizeEvent(QResizeEvent *) {
        int size = qMin(width(), height());
        size = qRound(size * (1.0 - 2*BoardBorder)); // add a border
        
        // assure all the squares being of the same size
        // by making the size a multiple of the boardSize
        int boardSize = MainWindow::boardSize();
        size = (size / boardSize) * boardSize;
        
        int borderLeft = (width() - size)/2;
        int borderTop = (height() - size)/2;
        
        setFrameRect(QRect(borderLeft, borderTop, size, size));
        
        QFrame::paintEvent(e);
    }
};

#endif //VIEW_H
