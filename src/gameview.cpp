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

#include "gameview.h"

#include <KgSound>
#include <QGraphicsObject>
#include <KMessageBox>
#include <KgThemeProvider>
#include <QQuickItem>
#include <KLocalizedString>

#include "globals.h"
#include "settings.h"

GameView::GameView(QWidget *parent) :
    KgDeclarativeView(parent),
    grid(new AbstractGrid),
    m_provider(new KgThemeProvider)
{
    m_provider->discoverThemes("appdata", QLatin1String("themes"));
    m_provider->setDeclarativeEngine("themeProvider", engine());
    m_soundTurn = new KgSound(QStandardPaths::locate(QStandardPaths::DataLocation, "sounds/turn.wav"), this);
    m_soundClick = new KgSound(QStandardPaths::locate(QStandardPaths::DataLocation, "sounds/click.wav"), this);
    m_soundConnect = new KgSound(QStandardPaths::locate(QStandardPaths::DataLocation, "sounds/connect.wav"), this);
    QString path = QStandardPaths::locate(QStandardPaths::DataLocation, "qml/main.qml");

    setSource(QUrl::fromLocalFile(path));
    setRotateDuration();

    connect(rootObject(), SIGNAL(clicked(int)),this, SLOT(clicked(int)));
    connect(rootObject(), SIGNAL(rotated(int,int)), this, SLOT(rotated(int,int)));
    connect(this, SIGNAL(setSize(QVariant,QVariant)),
            rootObject(), SLOT(setBoardSize(QVariant,QVariant)));
    connect(this, SIGNAL(newCell(QVariant,QVariant)),
            rootObject(), SLOT(addCell(QVariant,QVariant)));
    connect(this, SIGNAL(setSprite(QVariant,QVariant,QVariant)),
            rootObject(), SLOT(setSprite(QVariant,QVariant,QVariant)));
    connect(this, SIGNAL(lock(QVariant)), rootObject(), SLOT(lock(QVariant)));
    connect(this, SIGNAL(gameOver(QVariant)), rootObject(), SLOT(gameOver(QVariant)));
}

GameView::~GameView()
{
    delete m_provider;
    delete grid;
}

void GameView::startNewGame(uint width, uint height, Wrapping w=NotWrapped)
{
    setSize(width, height);
    grid->initializeGrid(width, height, w);

    for(int i = 0; i < grid->cellCount(); i++)
    {
        QString code = getCableCode(grid->cellAt(i)->cables());
        QString type = "none";
        if(grid->cellAt(i)->isConnected()){
            code = QString("con") + code;
        }
        if(grid->cellAt(i)->isServer()){
            type = "server";
        }
        else if(grid->cellAt(i)->isTerminal()) {
            type = (grid->cellAt(i)->isConnected())? "computer2": "computer1";
        }
        newCell(code, type);
    }
}

void GameView::clicked(int index)
{
    if (index >= 0) {
        rotatingCells.insert(index);
        emit rotationStarted();
        if (Settings::playSounds()) {
            m_soundTurn->start();
        }
    }
    else if (Settings::playSounds()) { //invalid click
        m_soundClick->start();
    }
}

void GameView::rotated(int index, int angle)
{
    switch (angle) {
    case 90: case -270:
        grid->cellAt(index)->rotateClockwise();
        break;
    case -90: case 270:
        grid->cellAt(index)->rotateCounterclockwise();
        break;
    case 180: case -180:
        grid->cellAt(index)->invert();
    }
    QList<int> changedCells = grid->updateConnections();
    bool newTerminalConnected = false;
    rotatingCells.remove(index);

    foreach (int i, changedCells) {
        if(grid->cellAt(i)->isTerminal()){
            newTerminalConnected = true;
        }
        updateSprite(i);
    }

    if(newTerminalConnected && Settings::playSounds())
        m_soundConnect->start();

    if(!changedCells.contains(index)) {
        updateSprite(index);
    }

    if (Settings::autolock()) {
        emit lock(index);
    }
    if (rotatingCells.count() == 0) {
        checkCompleted();
    }
}

void GameView::updateSprite(int index)
{
    QString type = "none";
    if(grid->cellAt(index)->isTerminal()){
        type = (grid->cellAt(index)->isConnected())? "computer2": "computer1";
    }

    QString code = getCableCode(grid->cellAt(index)->cables());
    if(grid->cellAt(index)->isConnected()) {
        code.insert(0, "con");
    }

    if (!rotatingCells.contains(index)) {
        setSprite(index, code, type);
    }
}

void GameView::checkCompleted()
{
    if (!grid->allTerminalsConnected()) {
        return;
    }
    for(int i = 0; i < grid->cellCount(); i++) {
        if (grid->cellAt(i)->cables() != None && !grid->cellAt(i)->isConnected()) {
            KMessageBox::information(this,
                i18n("Note: to win the game all terminals "
                "<strong>and all <em>cables</em></strong> "
                "need to be connected to the server!"),
                i18n("The game is not won yet!"),
                QLatin1String( "dontShowGameNotWonYet" ));
            return;
        }
    }
    emit gameOver("won");
}

void GameView::solve()
{
    if (rotatingCells.count() > 0) {
        return;
    }
    for(int i = 0; i < grid->cellCount(); i++) {
        grid->cellAt(i)->reset();
        QString code = "con" + getCableCode(grid->cellAt(i)->cables());
        if (grid->cellAt(i)->isTerminal()){
            setSprite(i, code, "computer2");
        }
        else {
            setSprite(i, code, "none");
        }
    }
    emit gameOver("solved");
}

void GameView::setRotateDuration()
{
    rootObject()->setProperty("rotateDuration", Settings::rotateDuration());
}

QString GameView::getCableCode(int cables)
{
    QHash<int, QString> directionNames;
    directionNames[Left]            = "0001";
    directionNames[Down]            = "0010";
    directionNames[Down|Left]       = "0011";
    directionNames[Right]           = "0100";
    directionNames[Right|Left]      = "0101";
    directionNames[Right|Down]      = "0110";
    directionNames[Right|Down|Left] = "0111";
    directionNames[Up]              = "1000";
    directionNames[Up|Left]         = "1001";
    directionNames[Up|Down]         = "1010";
    directionNames[Up|Down|Left]    = "1011";
    directionNames[Up|Right]        = "1100";
    directionNames[Up|Right|Left]   = "1101";
    directionNames[Up|Right|Down]   = "1110";
    return directionNames[cables];
}
