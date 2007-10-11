/*
    Copyright 2007 Dmitry Suzdalev <dimsuz@gmail.com>
    Copyright 2007 Fela Winkelmolen <fela.kde@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "renderer.h"

#include <QPainter>

#include <KSvgRenderer>
#include <KGameTheme>
#include <kpixmapcache.h>

#include "settings.h"
#include "consts.h"

Renderer* Renderer::self()
{
    static Renderer instance;
    return &instance;
}

Renderer::Renderer()
{
    m_renderer = new KSvgRenderer();
    m_cache = new KPixmapCache("knetwalk-cache");
    m_cache->setCacheLimit(3*1024);

    if(!loadTheme( Settings::theme() ))
        kDebug() << "Failed to load any game theme!";
    
    directionNames[L]     = "0001";
    directionNames[D]     = "0010";
    directionNames[D|L]   = "0011";
    directionNames[R]     = "0100";
    directionNames[R|L]   = "0101";
    directionNames[R|D]   = "0110";
    directionNames[R|D|L] = "0111";
    directionNames[U]     = "1000";
    directionNames[U|L]   = "1001";
    directionNames[U|D]   = "1010";
    directionNames[U|D|L] = "1011";
    directionNames[U|R]   = "1100";
    directionNames[U|R|L] = "1101";
    directionNames[U|R|D] = "1110";
}

Renderer::~Renderer()
{
    delete m_renderer;
    delete m_cache;
}

bool Renderer::loadTheme( const QString& themeName )
{
    // variable saying whether to discard old cache upon successful new theme loading
    // we won't discard it if m_currentTheme is empty meaning that
    // this is the first time loadTheme() is called
    // (i.e. during startup) as we want to pick the cache from disc
    bool discardCache = !m_currentTheme.isEmpty();

    if( !m_currentTheme.isEmpty() && m_currentTheme == themeName )
    {
        kDebug() << "Notice: not loading the same theme";
        return true; // this is not an error
    }
    KGameTheme theme;
    if ( !theme.load( themeName ) )
    {
        kDebug() << "Failed to load theme" << Settings::theme();
        kDebug() << "Trying to load default";
        if(!theme.loadDefault())
            return false;
    }

    m_currentTheme = themeName;

    bool res = m_renderer->load( theme.graphics() );
    kDebug() << "loading" << theme.graphics();
    if ( !res )
        return false;

    if(discardCache)
    {
        kDebug() << "discarding cache";
        m_cache->discard();
    }
    return true;
}

QPixmap Renderer::backgroundPixmap( const QSize& size ) const
{
    QPixmap pixmap;
    QString cacheName = QString("background%1x%2").arg(size.width()).arg(size.height());
    if(!m_cache->find( cacheName, pixmap ))
    {
        
        // calculate the background bounding rect
        int const w = size.width();
        int const h = size.height();
        qreal ratio = 1.0 - BackgroundBorder*2;
        QRectF bgRect(BackgroundBorder * w, BackgroundBorder * h, 
                                ratio * w, ratio * h);
        
        kDebug() << "re-rendering pixmap";
        pixmap = QPixmap(size);
        pixmap.fill(Qt::red); // TODO: testing only make transparent
        QPainter p(&pixmap);
        m_renderer->render(&p, "background", bgRect);
        m_cache->insert(cacheName, pixmap);
        kDebug() << "cache size:" << m_cache->size() << "kb";
    }
    return pixmap;
}

QPixmap Renderer::backgroundOverlayPixmap(int size) const
{
    QPixmap pixmap;
    QString cacheName = QString("overlay%1").arg(size);
    if ( !m_cache->find(cacheName, pixmap) )
    {
        kDebug() << "re-rendering overlay";
        
        pixmap = QPixmap(size, size);
        pixmap.fill(Qt::transparent);
        QPainter p(&pixmap);
        m_renderer->render(&p, "overlay");
        
        m_cache->insert(cacheName, pixmap);
        kDebug() << "cache size:" << m_cache->size() << "kb";
    }
    return pixmap;
}

QPixmap Renderer::cablesPixmap(int size, int dirs, bool isConnected) const
{
    QPixmap pixmap;
    QString elementId = isConnected ? "cablecon" : "cable";
    elementId += directionNames[dirs];
    QString cacheName = QString("%1-%2").arg(elementId).arg(size);
    if (m_cache->find(cacheName, pixmap)) return pixmap;
    
    // else
    kDebug() << "re-rendering " << cacheName;
    
    pixmap = QPixmap(size, size);
    pixmap.fill(Qt::transparent);
    
    const qreal ratio = 1.0 - CellBorder*2;
    QRectF boundingRect(CellBorder * size, CellBorder * size, 
                        ratio * size, ratio * size);
    
    QPainter painter;
    painter.begin(&pixmap);
    m_renderer->render(&painter, elementId, boundingRect);
    painter.end();
    
    kDebug() << "cache size:" << m_cache->size() << "kb";
    
    m_cache->insert(cacheName, pixmap);
    return pixmap;
}

QPixmap Renderer::computerPixmap(int size, bool root, bool connected) const
{
    QPixmap pixmap;
    QString elementId;
    if (root) elementId = "server";
    if (!root && !connected) elementId = "computer1";
    if (!root && connected)  elementId = "computer2";
    QString cacheName = QString(elementId) + size;
    
    if (m_cache->find(cacheName, pixmap)) return pixmap;
    
    // else
    kDebug() << "re-rendering " << elementId;
    
    const qreal ratio = 1.0 - CellForgroundBorder*2;
    QRectF boundingRect(CellForgroundBorder * size, CellForgroundBorder * size, 
                            ratio * size, ratio * size);
    
    pixmap = QPixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    m_renderer->render(&p, elementId, boundingRect);
    
    m_cache->insert(cacheName, pixmap);
    kDebug() << "cache size:" << m_cache->size() << "kb";
    return pixmap;
}
