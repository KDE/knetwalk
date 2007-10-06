#ifndef CONSTS_H
#define CONSTS_H

#include <QColor>

int const MinimumWidth = 200;
int const MinimumHeight = 200;

// used for the rotation of cables
int const AnimationUpdateInterval = 50;

// relative numbers
const qreal BoardBorder = 0.04;
const qreal OverlayBorder = 0.02;
// negative borders to get around the wrong bounding rect
// calculation of the svg renderer
const qreal CellBorder = -0.015;
const qreal BackgroundBorder = -0.01;
// border of the computer and server sprites inside the cells
const qreal CellForgroundBorder = 0.1;

const QColor HoveredCellColor(255, 255, 255, 30);
const QColor LockedCellColor(0, 0, 0, 100);

#endif // CONSTS_H
