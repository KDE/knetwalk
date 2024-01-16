/*
    SPDX-FileCopyrightText: 2005 Thomas Nagy <tnagyemail-mail@yahoo.fr>
    SPDX-FileCopyrightText: 2007-2008 Fela Winkelmolen <fela.kde@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"
#include "knetwalk_version.h"

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include <KDBusService>

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("knetwalk"));

    KAboutData about(QStringLiteral("knetwalk"), i18n("KNetWalk"),
                         QStringLiteral(KNETWALK_VERSION_STRING), i18n("KNetWalk, a game for system administrators."),
                         KAboutLicense::GPL, i18n("(C) 2004-2005 Andi Peredri, ported to KDE by Thomas Nagy\n"
                                                  "(C) 2007-2008 Fela Winkelmolen\n"
                                                  "(C) 2013 Ashwin Rajeev"), QString(),
                         QStringLiteral("https://apps.kde.org/knetwalk"));
    
    about.addAuthor(i18n("Andi Peredri"),
                    i18n("original author"),
                    QStringLiteral("andi@ukr.net"));
    
    about.addAuthor(i18n("Thomas Nagy"),
                    i18n("KDE port"),
                    QStringLiteral("tnagy2^8@yahoo.fr"));

    about.addAuthor(i18n("Ashwin Rajeev"),
                    i18n("Port to QtQuick"),
                    QStringLiteral("ashwin_rajeev@hotmail.com"));
                    
    about.addCredit(i18n("Eugene Trounev"),
                    i18n("icon design"),
                    QStringLiteral("eugene.trounev@gmail.com"));

    about.addCredit(i18n("Brian Croom"),
                    i18n("Port to use the QGraphicsView framework"),
                    QStringLiteral("brian.s.croom@gmail.com"));

    about.setProductName(QByteArray("knetwalk"));
    
    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    KCrash::initialize();
    KDBusService service;
 
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("knetwalk")));

    MainWindow* window = new MainWindow;
    window->show();

    return app.exec();
}

