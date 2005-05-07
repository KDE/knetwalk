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

#include <qpainter.h>
#include <qimage.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

#include "cell.h"

Cell::PixmapMap Cell::connectedpixmap;
Cell::PixmapMap Cell::disconnectedpixmap;

void Cell::initPixmaps()
{
	typedef QMap<int, QString> NamesMap;
	NamesMap names;
	names[L]     = "0001";
	names[D]     = "0010";
	names[D|L]   = "0011";
	names[R]     = "0100";
	names[R|L]   = "0101";
	names[R|D]   = "0110";
	names[R|D|L] = "0111";
	names[U]     = "1000";
	names[U|L]   = "1001";
	names[U|D]   = "1010";
	names[U|D|L] = "1011";
	names[U|R]   = "1100";
	names[U|R|L] = "1101";
	names[U|R|D] = "1110";

	NamesMap::ConstIterator it;
	for(it = names.constBegin(); it != names.constEnd(); ++it)
	{
		connectedpixmap[it.key()]=new QPixmap(KGlobal::iconLoader()->loadIcon(
					locate("data","knetwalk/cable"+it.data()+".png"), KIcon::NoGroup, 32) );

		QImage image = connectedpixmap[it.key()]->convertToImage();
		for(int y = 0; y < image.height(); y++)
		{
			QRgb* line = (QRgb*)image.scanLine(y);
			for(int x = 0; x < image.width(); x++)
			{
				QRgb pix = line[x];
				if(qAlpha(pix) == 255)
				{
					int g = (255 + 4 * qGreen(pix)) / 5;
					int b = (255 + 4 * qBlue(pix)) / 5;
					int r = (255 + 4 * qRed(pix)) / 5;
					line[x] = qRgb(r, g, b);
				}
			}
		}
		disconnectedpixmap[it.key()] = new QPixmap(image);
	}
}

Cell::Cell(QWidget* parent, int i) : QWidget(parent, 0, WNoAutoErase)
{
	angle     = 0;
	light     = 0;
	iindex    = i;
	ddirs     = Free;
	changed   = true;
	connected = false;
	root      = false;
	locked    = false;
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
	if(changed)
	{
		changed = false;
		if ( locked ) {
	    pixmap = KGlobal::iconLoader()->loadIcon(locate("data", "knetwalk/background_locked.png"), KIcon::NoGroup, 32);
		} else {
			pixmap = KGlobal::iconLoader()->loadIcon(locate("data", "knetwalk/background.png"), KIcon::NoGroup, 32);
		}

		QPainter paint;
		paint.begin(&pixmap);

		if(light)
		{
			paint.setPen(QPen(white, 5));
			paint.drawLine(0, width() - light, width(), 2 * width() - light);
		}

		if((ddirs != Free) && (ddirs != None))
		{
			double offset = 0;
			if(angle)
			{
				offset = width() / 2;
				paint.translate(offset, offset);
				paint.rotate(angle);
			}

			if(connected)
				paint.drawPixmap(int(-offset), int(-offset), *connectedpixmap[ddirs]);
			else paint.drawPixmap(int(-offset), int(-offset), *disconnectedpixmap[ddirs]);
			paint.resetXForm();

			QPixmap pix;
			
			if(root)
			{
				pix=KGlobal::iconLoader()->loadIcon(locate("data", "knetwalk/server.png"), KIcon::NoGroup, 32);
			}
			else if(ddirs == U || ddirs == L || ddirs == D || ddirs == R)
			{
				if(connected) 
					pix=KGlobal::iconLoader()->loadIcon(locate("data","knetwalk/computer2.png"),KIcon::NoGroup,32);
				else 
					pix=KGlobal::iconLoader()->loadIcon(locate("data","knetwalk/computer1.png"),KIcon::NoGroup,32);
			}
			paint.drawPixmap(0, 0, pix);
		}
		paint.end();
	}
	bitBlt(this, 0, 0, &pixmap);
}

void Cell::mousePressEvent(QMouseEvent* e)
{
	if(e->button() == LeftButton)
		emit lClicked(iindex);
	else if(e->button() == RightButton)
		emit rClicked(iindex);
	else if(e->button() == MidButton)
		emit mClicked(iindex);
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
