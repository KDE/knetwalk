/***************************************************************************
 *   Copyright (C) 2004, 2005 Andi Peredri                                 *
 *   andi@ukr.net                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation (see COPYING)            *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/

#include "cell.h"

#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QPaintEvent>

#include <KGlobal>
#include <KIconLoader>
#include <KStandardDirs>
#include <KSvgRenderer>

Cell::NamesMap Cell::directionNames;
KSvgRenderer Cell::allSvg;

void Cell::initPixmaps()
{
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
	allSvg.load( KStandardDirs::locate( "data","knetwalk/all.svgz" ) );
}

Cell::Cell(QWidget* parent, int i) : QWidget(parent)
{
	angle     = 0;
	light     = 0;
	iindex    = i;
	ddirs     = Free;
	changed   = true;
	connected = false;
	root      = false;
	locked    = false;
	pixmapCache = new QPixmap(width(), height());
}

Cell::~Cell()
{
    delete pixmapCache;
}

int Cell::index() const
{
	return iindex;
}

Cell::Dirs Cell::dirs() const
{
	return ddirs;
}

bool Cell::isConnected() const
{
	return connected;
}

bool Cell::isRotated() const
{
	return angle;
}

bool Cell::isLocked() const
{
    return locked;
}

void Cell::setLocked( bool newlocked )
{
	if ( locked == newlocked ) return;
	locked = newlocked;
	changed = true;
	update();
}


void Cell::setDirs(Dirs d)
{
	if(ddirs == d) return;
	ddirs = d;
	changed = true;
	update();
}

void Cell::setConnected(bool b)
{
	if(connected == b) return;
	connected = b;
	changed = true;
	update();
}

void Cell::setRoot(bool b)
{
	if(root == b) return;
	root = b;
	changed = true;
	update();
}

void Cell::setLight(int l)
{
	light = l;
	changed = true;
	update();
}

void Cell::paintEvent(QPaintEvent*)
{
	QPainter painter;
	if(changed)
	{
		pixmapCache->fill(QColor(0, 0, 0, 0));
		painter.begin(pixmapCache);

		changed = false;
		/*if ( locked ) {
			allSvg.render(&painter, "background-locked");
		} else {
			allSvg.render(&painter, "background");
		}*/

/*
		if(light)
		{
			painter.setPen(QPen(Qt::white, 5));
			painter.drawLine(0, width() - light, width(), 2 * width() - light);
		}
*/
		if((ddirs != Free) && (ddirs != None))
		{
			if(angle)
			{
				double woffset, hoffset;
				woffset = width() / 2;
				hoffset = height() / 2;
				painter.translate(woffset, hoffset);
				painter.rotate(angle);
				painter.translate(-woffset, -hoffset);
			}

			if(connected)
				allSvg.render(&painter, "cable" + directionNames[ddirs]);
			else
				allSvg.render(&painter, "cable" + directionNames[ddirs]);
			
			int w = pixmapCache->width();
			int h = pixmapCache->height();
			qreal ratio = 0.6;
                        QRectF boundingRect((1.0-ratio)/2 * w, (1.0-ratio)/2 * h, 0.6 * w, 0.6 * h);
			painter.resetMatrix();
			if(root)
			{
				allSvg.render(&painter, "server", boundingRect);
			}
			else if(ddirs == U || ddirs == L || ddirs == D || ddirs == R)
			{
				if(connected)
					allSvg.render(&painter, "computer2", boundingRect);
				else
					allSvg.render(&painter, "computer1", boundingRect);
			}
		}
		painter.end();
	}
	painter.begin(this);
	painter.drawPixmap(0, 0, *pixmapCache);
	painter.end();
}

void Cell::mousePressEvent(QMouseEvent* e)
{
	if(e->button() == Qt::LeftButton)
		emit lClicked(iindex);
	else if(e->button() == Qt::RightButton)
		emit rClicked(iindex);
	else if(e->button() == Qt::MidButton)
		emit mClicked(iindex);
}

void Cell::resizeEvent(QResizeEvent* e)
{
	changed = true;
	delete pixmapCache;
	pixmapCache = new QPixmap(e->size());
}

void Cell::rotate(int a)
{
	angle += a;
	changed = true;
	while(angle >= 45)
	{
		angle -= 90;
		int newdirs = Free;
		if(ddirs & U) newdirs |= R;
		if(ddirs & R) newdirs |= D;
		if(ddirs & D) newdirs |= L;
		if(ddirs & L) newdirs |= U;
		setDirs(Dirs(newdirs));
	}
	while(angle < -45)
	{
		angle += 90;
		int newdirs = Free;
		if(ddirs & U) newdirs |= L;
		if(ddirs & R) newdirs |= U;
		if(ddirs & D) newdirs |= R;
		if(ddirs & L) newdirs |= D;
		setDirs(Dirs(newdirs));
	}
	update();
}

#include "cell.moc"
