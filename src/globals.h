/*
    SPDX-FileCopyrightText: 2005 Thomas Nagy <tnagyemail-mail@yahoo.fr>
    SPDX-FileCopyrightText: 2007-2008 Fela Winkelmolen <fela.kde@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QColor>

int const MinimumWidth = 400;
int const MinimumHeight = 400;

// used for the rotation of cables
int const AnimationTime = 400;

// relative numbers
const qreal BoardBorder = 0.04;
const qreal OverlayBorder = 0.02;

// ratio of minimum filled cells to total cells
const qreal minCellRatio = 0.8;

// border of the computer and server sprites inside the cells
const qreal CellForegroundBorder = 0.1;

const QColor HoveredCellColor(255, 255, 255, 30);
const QColor LockedCellColor(0, 0, 0, 100);

enum Directions { // bitwise ORed
  Up    = 1,
  Right = 2,
  Down  = 4,
  Left  = 8,
  
  None  = 0
};

// TODO: use settings??
// easy to convert to bool ("isWrapped = wrapping;")
enum Wrapping {NotWrapped = 0, Wrapped = 1};

#endif // GLOBALS_H
