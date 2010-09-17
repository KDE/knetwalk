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
 
#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <KGameRenderer>

#include "globals.h"

class KGameRenderedItem;
class FieldItem;
class KGamePopupItem;

class Renderer : public KGameRenderer
{
public:
    Renderer();
    static Renderer* self();
};

class KNetWalkScene : public QGraphicsScene
{
    Q_OBJECT
public:
    KNetWalkScene(QObject* parent=0);

    FieldItem* fieldItem() const {return m_fieldItem;}

    void resizeScene(const QSizeF& size);
    void startNewGame(uint width, uint height, Wrapping w=NotWrapped);
    void setGamePaused(bool paused);
private slots:
    void terminalsConnected();
private:
    void resizeItems();

    FieldItem* m_fieldItem;
    KGamePopupItem* m_gamePausedMessage;
};

#endif
