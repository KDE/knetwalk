/***************************************************************************
 *   Copyright (C) 2007 Fela Winkelmolen <fela.kde@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef CONSTS_H
#define CONSTS_H

#include <QColor>

int const MinimumWidth = 200;
int const MinimumHeight = 200;

// used for the rotation of cables
int const AnimationTime = 400;

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
