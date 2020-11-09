/*
    Copyright (C) 2013 by Ashwin Rajeev<ashwin_rajeev@hotmail.com>

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

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <KgDeclarativeView>
#include <QVariant>
#include <KgThemeProvider>
#include <QQuickItem>

#include "abstractgrid.h"

class KgSound;

class GameView : public KgDeclarativeView
{
    Q_OBJECT
public:
    explicit GameView(QWidget *parent=0);
    virtual ~GameView();
    void startNewGame(uint width, uint height, Wrapping w);
    int minimumMoves() {return grid->minimumMoves();}
    int cellCount() {return grid->cellCount();}
    KgThemeProvider* getProvider() {return m_provider;}
    


public slots:
    void updateSettings();

signals:
    void newCell(const QVariant &cable, const QVariant &type);
    void setSprite(const QVariant &pos, const QVariant &cable, const QVariant &type);
    void setSize(const QVariant &width, const QVariant &height);
    void rotationStarted();
    void lock(const QVariant&);
    void gameOver(const QVariant &msg);

private slots:
    void solve();
    void clicked(int index);
    void rotated(int index, int angle);

private:
    QString getCableCode(int cables);
    void updateSprite(int index);
    void checkCompleted();

    AbstractGrid *grid;
    KgThemeProvider* m_provider;
    KgSound *m_soundTurn;
    KgSound *m_soundClick;
    KgSound *m_soundConnect;
    QSet<int> rotatingCells;
};

#endif //GAMEVIEW_H
