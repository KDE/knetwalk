/*
    Copyright 2010 Brian Croom <brian.s.croom@gmail.com> 
  
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

#include "view.h"
#include "scene.h"

#include <QResizeEvent>
#include <KLocale>

KNetWalkView::KNetWalkView(KNetWalkScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent), m_scene(scene)
{
    setWhatsThis(i18n("<h3>Rules of Game</h3><p>You are the " 
          "system administrator and your goal is to connect each terminal and "
          "each cable to the central server.</p><p>Click the right mouse "
          "button to turn the cable in a clockwise direction, and the left "
          "mouse button to turn the cable in a counterclockwise "
          "direction.</p><p>Start the LAN with as few turns as possible!</p>"));
}

void KNetWalkView::resizeEvent(QResizeEvent* event)
{
    m_scene->resizeScene(event->size());
}

#include "view.moc"
