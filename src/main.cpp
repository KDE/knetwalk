/*
    Copyright 2005 Thomas Nagy <tnagyemail-mail@yahoo.fr> 
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com> 
  
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

#include <KAboutData>
#include <KCrash>
#include <QApplication>
#include <KLocalizedString>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <Kdelibs4ConfigMigrator>
#endif
#include "mainwindow.h"
#include "knetwalk_version.h"
#include <KDBusService>

#include <QCommandLineParser>

int main(int argc, char ** argv)
{
    // Fixes blurry icons with fractional scaling
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("knetwalk");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Kdelibs4ConfigMigrator migrate(QStringLiteral("knetwalk"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("knetwalkrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("knetwalkui.rc"));
    migrate.migrate();
#endif

    KAboutData about(QStringLiteral("knetwalk"), i18n("KNetWalk"),
                         QStringLiteral(KNETWALK_VERSION_STRING), i18n("KNetWalk, a game for system administrators."),
                         KAboutLicense::GPL, i18n("(C) 2004-2005 Andi Peredri, ported to KDE by Thomas Nagy\n"
                                                  "(C) 2007-2008 Fela Winkelmolen\n"
                                                  "(C) 2013 Ashwin Rajeev"), QString(),
                         QStringLiteral("https://kde.org/applications/games/org.kde.knetwalk"));
    
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
 
    app.setApplicationDisplayName(about.displayName());
    app.setOrganizationDomain(about.organizationDomain());
    app.setApplicationVersion(about.version());
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("knetwalk")));

    MainWindow* window = new MainWindow;
    window->show();

    return app.exec();
}

