#include "jxintegration.h"

#include <QtGui/qpa/qplatformintegrationplugin.h>

#include <cstring>
#define MODULENAME "QPA"

QT_BEGIN_NAMESPACE

class JxIntegrationPlugin : public QPlatformIntegrationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformIntegrationFactoryInterface_iid FILE "jimberx264.json")
public:
    QPlatformIntegration *create(const QString &, const QStringList &) override;
};

QPlatformIntegration *JxIntegrationPlugin::create(const QString &system,
                                                  const QStringList &paramList)
{
    Q_UNUSED(paramList)
    if (!system.compare(QLatin1String("jimber"), Qt::CaseInsensitive))
        return new JxIntegration();


    return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
