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

#include <KgSound>
#include <KStandardDirs>
#include <QGraphicsObject>
#include <KMessageBox>
#include <KgThemeProvider>
#include <KLocale>

#include "gameview.h"
#include "globals.h"
#include "settings.h"

GameView::GameView(QWidget *parent) :
    KgDeclarativeView(parent),
    grid(new AbstractGrid),
    m_provider(new KgThemeProvider)
{
    m_provider->discoverThemes("appdata", QLatin1String("themes"));
    m_provider->setDeclarativeEngine("themeProvider", engine());
    m_soundTurn = new KgSound(KStandardDirs::locate("appdata", "sounds/turn.wav"), this);
    m_soundClick = new KgSound(KStandardDirs::locate("appdata", "sounds/click.wav"), this);
    m_soundConnect = new KgSound(KStandardDirs::locate("appdata", "sounds/connect.wav"), this);
    QString path = KStandardDirs::locate("appdata", "qml/main.qml");
    setSource(QUrl::fromLocalFile(path));

    connect(rootObject(), SIGNAL(clicked(int, QString)),
            this, SLOT(rotationStarted(int, QString)));
    connect(rootObject(), SIGNAL(rotated(int)), this, SLOT(rotated(int)));
    connect(rootObject(), SIGNAL(empty()), this, SLOT(playClick()));
    connect(this, SIGNAL(levelChanged(QVariant)),
            rootObject(), SLOT(setBoardSize(QVariant)));
    connect(this, SIGNAL(newCell(QVariant,QVariant,QVariant)),
            rootObject(), SLOT(addCell(QVariant,QVariant,QVariant)));
    connect(this, SIGNAL(setSprite(QVariant,QVariant,QVariant)),
            rootObject(), SLOT(setSprite(QVariant,QVariant,QVariant)));
    connect(this, SIGNAL(gameWon()), rootObject(), SLOT(won()));
}

void GameView::startNewGame(uint width, uint height, Wrapping w=NotWrapped)
{
    levelChanged(width);
    grid->initializeGrid(width, height, w);

    for(int i = 0; i < grid->cellCount(); i++)
    {
        QString code = getCableCode(grid->cellAt(i)->cables());
        QString type = "";
        if(grid->cellAt(i)->isConnected())
            code = QString("con") + code;

        if(grid->cellAt(i)->isServer()){
            type = "server";
        }
        else if(grid->cellAt(i)->isTerminal()) {
            type = (grid->cellAt(i)->isConnected())? "computer2": "computer1";
        }
        newCell(i, code, type);
    }
}

void GameView::rotationStarted(int index, QString dir)
{
    if(dir == "clockwise") {
        grid->cellAt(index)->rotateClockwise();
    }
    else if(dir == "counterclockwise"){
        grid->cellAt(index)->rotateCounterclockwise();
    }
    if(Settings::playSounds())
        m_soundTurn->start();
    emit rotationPerformed();
}

void GameView::rotated(int index)
{
    QList<int> changedCells = grid->updateConnections();
    bool newTerminalConnected = false;

    foreach (int i, changedCells) {
        QString type = "none";
        if(grid->cellAt(i)->isTerminal()){
            type = (grid->cellAt(i)->isConnected())? "computer2": "computer1";
            if(grid->cellAt(i)->isConnected())
                newTerminalConnected = true;
        }

        QString code = getCableCode(grid->cellAt(i)->cables());
        if(grid->cellAt(i)->isConnected())
            code = QString("con") + code;

        setSprite(i, code, type);
    }

    if(newTerminalConnected && Settings::playSounds())
        m_soundConnect->start();

    if(!changedCells.contains(index)) {
        QString code = getCableCode(grid->cellAt(index)->cables());
        if(grid->cellAt(index)->isConnected())
            code = QString("con") + code;
        setSprite(index, code, "none");
    }

    checkCompleted();
}

void GameView::checkCompleted()
{
    for(int i = 0; i < grid->cellCount(); i++) {
        if(grid->cellAt(i)->cables() != None && !grid->cellAt(i)->isConnected()) {
            if(grid->allTerminalsConnected()){
                KMessageBox::information(this,
                    i18n("Note: to win the game all terminals "
                    "<strong>and all <em>cables</em></strong> "
                    "need to be connected to the server!"),
                    i18n("The game is not won yet!"),
                    QLatin1String( "dontShowGameNotWonYet" ));
            }
            return;
        }
    }
    emit gameWon();
}

void GameView::playClick()
{
    if(Settings::playSounds())
        m_soundClick->start();
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
