#pragma once

#include <qpa/qplatformfontdatabase.h>

QT_BEGIN_NAMESPACE

class JFontDatabase : public QPlatformFontDatabase
{
public:
    JFontDatabase()
    {
        qDebug() << "[JFontDatabase::ctor]";
    }
    virtual ~JFontDatabase(){};
    QStringList addApplicationFont(const QByteArray &fontData, const QString &fileName) override
    {
        Q_UNUSED(fontData);
        Q_UNUSED(fileName);
        // qDebug() << "[QPlatformFontDatabase::addApplicationFont]";
        return QStringList();
    }
    QFont defaultFont() const override
    {
        // qDebug() << "[QPlatformFontDatabase::defaultFont]";
        return QFont(QLatin1String("Helvetica"));
    }
    QString fontDir() const override
    {
        // qDebug() << "Fontdir asked!";
        QString fontpath = QString::fromLocal8Bit(qgetenv("QT_QPA_FONTDIR"));
        if (fontpath.isEmpty())
            fontpath = QLatin1String("/tmp/fonts");
        return QLatin1String("/tmp/fonts");
    }
};
    // static void QPlatformFontDatabase::registerFont(const QString &familyname, const QString &stylename,
    //                          const QString &foundryname, QFont::Weight weight,
    //                          QFont::Style style, QFont::Stretch stretch, bool antialiased,
    //                          bool scalable, int pixelSize, bool fixedPitch,
    //                          const QSupportedWritingSystems &writingSystems, void *handle)
    //                          {
    //                              qDebug() << "[JFontDatabase::registerFont]";
    //                          };

QT_END_NAMESPACE

// #endif