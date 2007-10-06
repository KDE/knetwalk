#ifndef VIEW_H
#define VIEW_H

#include <QFrame>

class View : public QFrame 
{
public:
    View(QWidget *parent=0) : QFrame(parent) {}

protected:
    void resizeEvent(QResizeEvent *) {
        int size = qMin(width(), height());
        size = static_cast<int>(size * (1.0 - 2*BoardBorder)); // add a border
        int borderLeft = (width() - size)/2;
        int borderTop = (height() - size)/2;
        setFrameRect(QRect(borderLeft, borderTop, size, size));
    }
};

#endif //VIEW_H
