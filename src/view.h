#ifndef VIEW_H
#define VIEW_H

#include <QFrame>

#include "mainwindow.h"

class View : public QFrame 
{
public:
    View(QWidget *parent=0) : QFrame(parent) {}

protected:
    void resizeEvent(QResizeEvent *) {
        int size = qMin(width(), height());
        size = qRound(size * (1.0 - 2*BoardBorder)); // add a border
        
        // assure all the squares being of the same size
        // by making the size a multiple of the boardSize
        int bsize = MainWindow::boardSize();
        size = size / bsize * bsize;
        
        int borderLeft = (width() - size)/2;
        int borderTop = (height() - size)/2;
        setFrameRect(QRect(borderLeft, borderTop, size, size));
    }
};

#endif //VIEW_H
