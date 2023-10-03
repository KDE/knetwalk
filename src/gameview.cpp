/*
    SPDX-FileCopyrightText: 2013 Ashwin Rajeev <ashwin_rajeev@hotmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gameview.h"

// game
#include "globals.h"
#include "settings.h"
// KDEGames
#include <KgSound>
// KF
#include <KMessageBox>
#include <KLocalizedContext>
#include <KLocalizedString>
// Qt
#include <QGraphicsObject>
#include <QQmlContext>


GameView::GameView(QWidget *parent) :
    QQuickWidget(parent),
    grid(new AbstractGrid),
    m_provider(new KGameThemeProvider)
{
    QQmlEngine *engine = this->engine();

    auto *localizedContextObject = new KLocalizedContext(engine);
    engine->rootContext()->setContextObject(localizedContextObject);

    setResizeMode(SizeRootObjectToView);

    m_provider->discoverThemes(QStringLiteral("themes"));
    m_provider->setDeclarativeEngine(QStringLiteral("themeProvider"), engine);
    m_soundTurn = new KgSound(QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("sounds/turn.wav")), this);
    m_soundClick = new KgSound(QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("sounds/click.wav")), this);
    m_soundConnect = new KgSound(QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("sounds/connect.wav")), this);
    QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("qml/main.qml"));

    setSource(QUrl::fromLocalFile(path));
    updateSettings();

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
        QString type = QStringLiteral("none");
        if(grid->cellAt(i)->isConnected()){
            code = QLatin1String("con") + code;
        }
        if(grid->cellAt(i)->isServer()){
            type = QStringLiteral("server");
        }
        else if(grid->cellAt(i)->isTerminal()) {
            type = (grid->cellAt(i)->isConnected())? QStringLiteral("computer2"): QStringLiteral("computer1");
        }
        newCell(code, type);
    }
}

void GameView::clicked(int index)
{
    if (index >= 0) {
        rotatingCells.insert(index);
        Q_EMIT rotationStarted();
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
    const QList<int> changedCells = grid->updateConnections();
    bool newTerminalConnected = false;
    rotatingCells.remove(index);

    for (int i : changedCells) {
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
        Q_EMIT lock(index);
    }
    if (rotatingCells.count() == 0) {
        checkCompleted();
    }
}

void GameView::updateSprite(int index)
{
    QString type = QStringLiteral("none");
    if(grid->cellAt(index)->isTerminal()){
        type = (grid->cellAt(index)->isConnected())? QStringLiteral("computer2"): QStringLiteral("computer1");
    }

    QString code = getCableCode(grid->cellAt(index)->cables());
    if(grid->cellAt(index)->isConnected()) {
        code.insert(0, QLatin1String("con"));
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
    for(int i = 0; i < grid->cellCount(); ++i) {
        if (grid->cellAt(i)->cables() != None && !grid->cellAt(i)->isConnected()) {
            KMessageBox::information(this,
                i18n("Note: to win the game all terminals "
                "<strong>and all <em>cables</em></strong> "
                "need to be connected to the server!"),
                i18n("The game is not won yet!"),
                QStringLiteral( "dontShowGameNotWonYet" ));
            return;
        }
    }
    Q_EMIT gameOver(QLatin1String("won"));
}

void GameView::solve()
{
    if (!rotatingCells.isEmpty()) {
        return;
    }
    for(int i = 0; i < grid->cellCount(); i++) {
        grid->cellAt(i)->reset();
        QString code = QLatin1String("con") + getCableCode(grid->cellAt(i)->cables());
        if (grid->cellAt(i)->isTerminal()){
            setSprite(i, code, QLatin1String("computer2"));
        }
        else {
            setSprite(i, code, QLatin1String("none"));
        }
    }
    Q_EMIT gameOver(QLatin1String("solved"));
}

void GameView::updateSettings()
{
    rootObject()->setProperty("rotateDuration", Settings::rotateDuration());
    rootObject()->setProperty("reverseButtons", Settings::reverseButtons());
}

QString GameView::getCableCode(int cables)
{
    QHash<int, QString> directionNames;
    directionNames[Left]            = QStringLiteral("0001");
    directionNames[Down]            = QStringLiteral("0010");
    directionNames[Down|Left]       = QStringLiteral("0011");
    directionNames[Right]           = QStringLiteral("0100");
    directionNames[Right|Left]      = QStringLiteral("0101");
    directionNames[Right|Down]      = QStringLiteral("0110");
    directionNames[Right|Down|Left] = QStringLiteral("0111");
    directionNames[Up]              = QStringLiteral("1000");
    directionNames[Up|Left]         = QStringLiteral("1001");
    directionNames[Up|Down]         = QStringLiteral("1010");
    directionNames[Up|Down|Left]    = QStringLiteral("1011");
    directionNames[Up|Right]        = QStringLiteral("1100");
    directionNames[Up|Right|Left]   = QStringLiteral("1101");
    directionNames[Up|Right|Down]   = QStringLiteral("1110");
    return directionNames[cables];
}

#include "moc_gameview.cpp"
