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
#include <Kdelibs4ConfigMigrator>

#include "mainwindow.h"
#include "knetwalk_version.h"
#include <KDBusService>

#include <QCommandLineParser>

static const char description[] =
I18N_NOOP("KNetWalk, a game for system administrators.");

static const char copyleft[] =
I18N_NOOP("(C) 2004-2005 Andi Peredri, ported to KDE by Thomas Nagy\n"
          "(C) 2007-2008 Fela Winkelmolen\n"
          "(C) 2013 Ashwin Rajeev");

int main(int argc, char ** argv)
{
    // Fixes blurry icons with fractional scaling
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("knetwalk");

    Kdelibs4ConfigMigrator migrate(QStringLiteral("knetwalk"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("knetwalkrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("knetwalkui.rc"));
    migrate.migrate();

    KAboutData about(QStringLiteral("knetwalk"), i18n("KNetWalk"),
                         QStringLiteral(KNETWALK_VERSION_STRING), i18n(description),
                         KAboutLicense::GPL, i18n(copyleft), QString(),
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

    about.setOrganizationDomain(QByteArray("kde.org"));
    
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

