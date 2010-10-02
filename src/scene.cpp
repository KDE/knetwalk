/*
    Copyright 2010 Brian Croom <brian.s.croom@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "scene.h"
#include "fielditem.h"
#include "settings.h"

#include <QPainter>
#include <QGraphicsView>
#include <KGameRenderedItem>
#include <KGamePopupItem>
#include <KMessageBox>
#include <KLocale>

Renderer::Renderer()
    : KGameRenderer(Settings::defaultThemeValue())
{
    setTheme(Settings::theme());
}

Renderer* Renderer::self()
{
    static Renderer instance;
    return &instance;
}

KNetWalkScene::KNetWalkScene(QObject* parent)
    : QGraphicsScene(parent)
{
    m_fieldItem = new FieldItem();
    connect(m_fieldItem, SIGNAL(terminalsConnected()), SLOT(terminalsConnected()));
    addItem(m_fieldItem);

    m_gamePausedMessage = new KGamePopupItem;
    m_gamePausedMessage->setMessageOpacity(0.8);
    m_gamePausedMessage->setMessageTimeout(0);
    m_gamePausedMessage->setHideOnMouseClick(false);
    addItem(m_gamePausedMessage);

    resizeItems();
}

void KNetWalkScene::resizeItems()
{
    QPixmap bg = Renderer::self()->spritePixmap(QLatin1String( "background" ), sceneRect().size().toSize());

    qreal size = qMin(sceneRect().width(), sceneRect().height());
    QPainter p(&bg);
    p.drawPixmap(QPointF(sceneRect().width()/2  - size/2,
                         sceneRect().height()/2 - size/2),
                 Renderer::self()->spritePixmap(QLatin1String( "overlay" ), QSize(size, size)));
    setBackgroundBrush(bg);

    size *= (1.0 - 2*OverlayBorder);
    m_fieldItem->resize(QSizeF(size, size));
    m_fieldItem->setPos(sceneRect().width()/2  - size/2,
                        sceneRect().height()/2 - size/2);

    m_gamePausedMessage->setPos(sceneRect().width()/2  - m_gamePausedMessage->boundingRect().width()/2,
                                sceneRect().height()/2 - m_gamePausedMessage->boundingRect().height()/2);
};

void KNetWalkScene::resizeScene(const QSizeF& size)
{
    setSceneRect(QRectF(QPointF(0.0, 0.0), size));
    resizeItems();
}

void KNetWalkScene::startNewGame(uint width, uint height, Wrapping w)
{
    m_fieldItem->initField(width, height, w);
    resizeItems();
}

void KNetWalkScene::setGamePaused(bool paused)
{
    m_fieldItem->setGamePaused(paused);
    if(paused)
        m_gamePausedMessage->showMessage(i18n("Game is paused."), KGamePopupItem::Center);
    else
        m_gamePausedMessage->forceHide();
}

void KNetWalkScene::terminalsConnected()
{
    KMessageBox::information(views().first(),
                             i18n("Note: to win the game all terminals "
                                   "<strong>and all <em>cables</em></strong> "
                                   "need to be connected to the server!"),
                             i18n("The game is not won yet!"),
                             QLatin1String( "dontShowGameNotWonYet" ));
}
