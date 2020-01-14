/***************************************************************************
 *   Copyright (C) 2013 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QTest>
#include <QDir>

#include "../src/abstractgrid.h"
#include "../src/mainwindow.h"

class AbstractGridTester : public AbstractGrid
{
    public:
        void initializeGrid(uint width, uint height, Wrapping w=NotWrapped)
        {
            AbstractGrid::initializeGrid(width, height, w);
        }
};

class AbstractGridTest : public QObject
{
    Q_OBJECT

    private slots:
        void testCreateGrid();
};

void AbstractGridTest::testCreateGrid()
{
    for (int i = 0; i < 25; ++i) {
        qDebug() << "Iteration" << i;
        AbstractGridTester t;
        t.initializeGrid(5, 5, NotWrapped);
        AbstractGridTester t2;
        t2.initializeGrid(5, 5, Wrapped);
        AbstractGridTester t3;
        t3.initializeGrid(7, 7, NotWrapped);
        AbstractGridTester t4;
        t4.initializeGrid(7, 7, Wrapped);
        AbstractGridTester t5;
        t5.initializeGrid(9, 9, NotWrapped);
        AbstractGridTester t6;
        t6.initializeGrid(9, 9, Wrapped);
    }
}

QTEST_GUILESS_MAIN( AbstractGridTest )

#include "abstractgridtest.moc"
