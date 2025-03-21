/*
    SPDX-FileCopyrightText: 2005 Thomas Nagy <tnagyemail-mail@yahoo.fr>
    SPDX-FileCopyrightText: 2007-2008 Fela Winkelmolen <fela.kde@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "config-knetwalk.h"

#include "mainwindow.h"
#include "knetwalk_version.h"

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#if HAVE_KDBUSADDONS
#include <KDBusService>
#endif
#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char ** argv)
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif
    QApplication app(argc, argv);
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER
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
#if HAVE_KDBUSADDONS
    KDBusService service;
#endif
 
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("knetwalk")));

    MainWindow* window = new MainWindow;
    window->show();

    return app.exec();
}

