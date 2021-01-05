
#ifndef QJIMBERINTEGRATION_H
#define QJIMBERINTEGRATION_H

#include "jxwsserver.h"
#include "jxbackingstore.h"
#include <qpa/qplatformintegration.h>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtCore/qthread.h>

#include "jxwindow.h"
#include "jxcursor.h"
#include "jxscreen.h"
#include "jxinputcontext.h"
#include <qpa/qplatforminputcontextfactory_p.h>
#include <qpa/qplatforminputcontext.h>

QT_BEGIN_NAMESPACE



typedef QMap<QString, QFunctionPointer> FunctionMap;

#define BrowserFunction(TYPE,NAME, ...) TYPE (*NAME(bool checkNullPtr = false ))(__VA_ARGS__) { if (m_functions[#NAME] == nullptr && checkNullPtr){ qInfo() << "No browserfunction" << #NAME << "found"; return [] (__VA_ARGS__) { return TYPE(); }; }  return reinterpret_cast<TYPE (*) (__VA_ARGS__)>(m_functions[#NAME]);    }


class QAbstractEventDispatcher;

class JxIntegration : public QPlatformIntegration, public QPlatformNativeInterface
{

public:
  explicit JxIntegration();
  virtual ~JxIntegration() override
  {
    QWindowSystemInterface::unregisterTouchDevice(_touchDevice);
  };
  
  static JxIntegration *instance();
  void initialize() override;
  QPlatformWindow *createPlatformWindow(QWindow *window) const;
  QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const;
  QAbstractEventDispatcher *createEventDispatcher() const override;
  void sendToAllClients(const QString &message, int windowId);
  QPlatformClipboard *clipboard() const override;
  QPlatformNativeInterface *nativeInterface() const override;
  QPlatformFontDatabase *fontDatabase() const override;
  QPlatformServices *services() const override;

  bool hasCapability(Capability cap) const
  {
 return cap == NonFullScreenWindows || cap == NativeWidgets || cap == WindowManagement;   }
  QPlatformInputContext *inputContext() const;
  QFunctionPointer platformFunction(const QByteArray &function) const override;

  void browserFunctions(FunctionMap functions){

    m_functions = functions;
   
  }

  BrowserFunction(void, onMessage, QString, int);
  BrowserFunction(void, onBinaryMessage, QByteArray);
  BrowserFunction(int, createBrowser, QString);  
  BrowserFunction(void, closeWindow, int);

private:
  FunctionMap m_functions;
  QTouchDevice *_touchDevice;
  // JxInputContext *_inputContext;
  QPlatformInputContext *_inputContext;

};

QT_END_NAMESPACE

#endif
