/*
    SPDX-FileCopyrightText: 2013 Ashwin Rajeev <ashwin_rajeev@hotmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
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
    explicit GameView(QWidget *parent=nullptr);
    ~GameView() override;
    void startNewGame(uint width, uint height, Wrapping w);
    int minimumMoves() {return grid->minimumMoves();}
    int cellCount() {return grid->cellCount();}
    KgThemeProvider* getProvider() {return m_provider;}
    


public Q_SLOTS:
    void updateSettings();

Q_SIGNALS:
    void newCell(const QVariant &cable, const QVariant &type);
    void setSprite(const QVariant &pos, const QVariant &cable, const QVariant &type);
    void setSize(const QVariant &width, const QVariant &height);
    void rotationStarted();
    void lock(const QVariant&);
    void gameOver(const QVariant &msg);

private Q_SLOTS:
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
