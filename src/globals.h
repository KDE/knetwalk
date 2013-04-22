/*
    Copyright 2005 Thomas Nagy  <tnagyemail-mail@yahoo.fr>
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
