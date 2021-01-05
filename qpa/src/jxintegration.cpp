#include "jxintegration.h"

#include "jxbackingstore.h"

#include <qpa/qplatformintegration.h>
#include <QtGui/qpa/qplatformwindow.h>
#include <QtGui/qpa/qplatformbackingstore.h>
#include <QtGui/private/qguiapplication_p.h>

#include <QtFontDatabaseSupport/private/qgenericunixfontdatabase_p.h>
#include <QtEventDispatcherSupport/private/qgenericunixeventdispatcher_p.h>

#include "jxclipboard.h"
#include "jxservices.h"
// #include "jFontDatabase.h"

QT_BEGIN_NAMESPACE

JxIntegration::JxIntegration()
{
}
QPlatformNativeInterface *JxIntegration::nativeInterface() const
{
    return const_cast<JxIntegration *>(this);
}

void JxIntegration::sendToAllClients(const QString &message, int windowId)
{
    JxWsServer::instance().sendToAllClients(message, windowId);
}
void JxIntegration::initialize()
{
    JxWsServer::instance(); // initialize wss
    JxScreen *mPrimaryScreen = new JxScreen(5000, 5000);

    // JxStreamer::instance().resize(QSize(800, 600));
    QWindowSystemInterface::handleScreenAdded(mPrimaryScreen);
    //mPrimaryScreen->_depth = 32;
    mPrimaryScreen->_format = QImage::Format_RGB888;

    // _inputContext = QPlatformInputContextFactory::create();
    _inputContext = new JxInputContext();

    // screenAdded(mPrimaryScreen);*/
}

QPlatformWindow *JxIntegration::createPlatformWindow(QWindow *window) const
{
    Q_UNUSED(window);
    auto touchDevice = new QTouchDevice();
    touchDevice->setName("JxTouchDevice");
    touchDevice->setType(QTouchDevice::TouchScreen);

    touchDevice->setCapabilities(QTouchDevice::Position | QTouchDevice::Pressure |
                                 QTouchDevice::MouseEmulation);

    touchDevice->setMaximumTouchPoints(6);

    QWindowSystemInterface::registerTouchDevice(touchDevice);

    QPlatformWindow *w = new JxWindow(window, touchDevice);
    // w->requestActivateWindow(); //If this line is active, popup windows will close the first time you try to open them.

    return w;
}

QPlatformBackingStore *JxIntegration::createPlatformBackingStore(QWindow *window) const
{
    return new JxBackingStore(window);
}
QAbstractEventDispatcher *JxIntegration::createEventDispatcher() const
{

    return createUnixEventDispatcher();
}
QPlatformFontDatabase *JxIntegration::fontDatabase() const
{

    return new QGenericUnixFontDatabase(); //NEVER EVER DISABLE THIS AGAIN, WE NEED THIS FOR TOOLTIPS AND STUFF
}

JxIntegration *JxIntegration::instance()
{
    return static_cast<JxIntegration *>(QGuiApplicationPrivate::platformIntegration());
}

QPlatformClipboard *JxIntegration::clipboard() const
{
    static QPlatformClipboard *clipboard = 0;
    if (!clipboard)
    {
        clipboard = new JxClipboard();
    }
    return clipboard;
}

QPlatformInputContext *JxIntegration::inputContext() const
{
    return _inputContext;
}

void sendToAll(QString message, int windowId)
{
    JxWsServer::instance().sendToAllClients(message, windowId);
}


void setFunctionsCallback(FunctionMap functions)
{
   qInfo() << "Setting functions!!!";
   JxIntegration::instance()->browserFunctions(functions);
}


QFunctionPointer JxIntegration::platformFunction(const QByteArray &function) const
{

    QString functionToGet = QString::fromLatin1(function);

    if (functionToGet == QString("sendToAllClients"))
    {
        return (QFunctionPointer)&sendToAll; //@todo reint
    }
    else if (functionToGet == QString("setAllFunctions"))
    {
        return (QFunctionPointer)&setFunctionsCallback;
    }

    else
    {
        return nullptr;
    }
}

QPlatformServices *JxIntegration::services() const
{

    return new JxServices();
}

QT_END_NAMESPACE