#ifndef CONSTS_H
#define CONSTS_H

#include <QColor>

int const MINIMUM_WIDTH = 200;
int const MINIMUM_HEIGHT = 150;

// used for the rotation of cables
int const ANIMATION_UPDATE_INTERVAL = 50;

// relative numbers
qreal const BOARD_BORDER = 0.04;
qreal const OVERLAY_BORDER = 0.02;
// negative borders to get around the wrong bounding rect
// calculation of the svg renderer
qreal const CELL_BORDER = -0.015;
qreal const BACKGROUND_BORDER = -0.01;
// border of the computer and server sprites inside the cells
qreal const CELL_FORGROUND_BORDER = 0.1;

const QColor HOVERED_CELL_COLOR(255, 255, 255, 30);
const QColor LOCKED_CELL_COLOR(0, 0, 0, 100);

#endif // CONSTS_H
