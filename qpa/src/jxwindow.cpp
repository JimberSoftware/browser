#include "jxwindow.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "touchobject.h"
#include "jxlog.h"
#include "JCommandHandler.h"
#include "jxclipboard.h"
#include <private/qguiapplication_p.h>
#include <private/qhighdpiscaling_p.h>
#include "keymap.h"
typedef IncomingCommand IC;

int JxWindow::_wincount = 0;

JxWindow::JxWindow(QWindow *window, QTouchDevice *touchDevice) : QPlatformWindow(window), window(window), _touchDevice(touchDevice), _closeEventFilter(window)
{
  _wincount++;
  _winId = _wincount;
  connect(&JxWsServer::instance(), &JxWsServer::textMessageReceived, this, &JxWindow::onMessage, Qt::QueuedConnection);
  connect(&JxWsServer::instance(), &JxWsServer::binaryMessageReceived, this, &JxWindow::onBinaryMessage, Qt::QueuedConnection);

  if (QString(qgetenv("WEBCAM_ENABLED")) == "1")
  {
    m_webcam_enabled = true;

  }

  window->installEventFilter(&_closeEventFilter);
}

WId JxWindow::winId() const
{
  // Return anything but 0. Returning 0 would cause havoc with QWidgets on
  // very basic platform plugins that do not reimplement this function,
  // because the top-level widget's internalWinId() would always be 0 which
  // would mean top-levels are never treated as native.
  return WId(JxWindow::_winId);
}

void JxWindow::onMessage(void *pClient, QString message, WId winId)
{
  Q_UNUSED(pClient)
  if (winId != this->winId())
  {
    return;
  }

  auto jxCommand = JIncomingCommand(message);

  // if (!jxCommand.toHandleCommand(this->winId()))
  //   return;

  switch (jxCommand.getCommand())
  {
  case IC::WINDOWCLOSED:
    handleWindowClosed();
    break;
  case IC::KEYBOARDEVENT:
    handleKeyMessage(jxCommand);
    break;
  case IC::PASTEEVENT:
    handlePasteMessage(jxCommand);
    break;
  case IC::MOUSEEVENT:
    handleMouseEvent(jxCommand);
    break;
  case IC::MOUSEWHEELEVENT:
    handleMouseWheel(jxCommand);
    break;
  case IC::FOCUSEVENT:
    handleFocusEvent(jxCommand);
    break;
  case IC::TOUCHEVENT:
    handleTouchEvent(jxCommand);
    break;
  case IC::QUALITY:
    handleQualityEvent(jxCommand);
    break;
  case IC::DEVICEPIXELRATIO:
    handlePixelRatioEvent(jxCommand);
    break;
  case IC::RESIZE:
    handleResize(jxCommand);
    break;
  case IC::POPUPCLOSED:
    handlePopupClosed(jxCommand);
    break;
  case IC::PING:
    handlePing();
    break;
  default:
    //qDebug() << "Some unknown command ... " << message;
    break;
  }

  // commando toevoegen en maken in jxcommands dat verwijst naar mijn functie changecrf
}

void JxWindow::handlePopupClosed(JIncomingCommand command)
{
  WId popupId = static_cast<WId>(command.arg(1).toInt());
  auto windows = QGuiApplication::allWindows();
  auto topLevelWindows = QGuiApplication::topLevelWindows();

  for (int i = 0; i < windows.length(); i++)
  {
    // if (windows[i]->objectName() == "JPopupWindowClassWindow" && popupId == windows[i]->winId())
    if (popupId == windows[i]->winId())
    {
      qInfo() << "closing window" << windows[i]->winId();
      windows[i]->close();
      break;
    }
  }

  topLevelWindows[0]->requestActivate(); //activate the main window again
}

void JxWindow::onBinaryMessage(void *pClient, QByteArray message)
{
  Q_UNUSED(pClient);

  QByteArray data = message;
  if (message[0] == 'c' && message[1] == 'a' && message[2] == 'm')
  {
    if (!m_webcam_enabled)
      return;

    data.remove(0, 3);
    m_webcam->write(data);
  }
  else if (message[0] == 'm' && message[1] == 'i' && message[2] == 'c')
  {
    data.remove(0, 3);
    m_mic->write(data);
  }
  else
  {
    // qInfo() << message;
  }
}

int eventKeyToInt(QString eventKey)
{
  int key = chars[eventKey];
  return key;
}
// int JxWindow::codeToKey(QString code)
// {
//   if (code.startsWith("Key"))
//     return code.at(3).unicode();
//   if (code.startsWith("Digit"))
//     return code.at(5).unicode();
//   return eventKeyToInt(code);
// }
void JxWindow::handleKeyMessage(JIncomingCommand command)
{
  QString code = command.arg(1);
  QString text = command.arg(2);
  QString releaseOrPress = command.arg(3);
  int key;
  if (code.startsWith("Key"))
  {
    // key = code.at(3).unicode();
    key = text.at(0).unicode()-32;
  }
  else if (code.startsWith("Digit"))
  {
    key = code.at(5).unicode();
  }
  else
  {
    key = eventKeyToInt(text);
    // if (code.startsWith("Numpad")){
    //   key = eventKeyToInt(text);
    // } else {
    //   key = eventKeyToInt(code);
    // }
    if (key > 0)
    { //etje
      text = "";
    }
  }
  if (text == "semicolonneke")
  {
    text = ";";
  }

  Qt::KeyboardModifiers modifiers = Qt::KeyboardModifiers(command.arg(4).toInt());

  if (releaseOrPress == "0")
  {
    this->onKeyEvent(QEvent::KeyPress, key, modifiers, text); // Browsers are weird in their implementation, only mobile comes here (on purpose)
  }
  else if (releaseOrPress == "1")
  {
    this->onKeyEvent(QEvent::KeyRelease, key, modifiers, text);
  }
  else if (releaseOrPress == "2")
  {
    this->onKeyEvent(QEvent::KeyPress, key, modifiers, text);
  }
}

void JxWindow::raise()
{
}

void JxWindow::handlePasteMessage(JIncomingCommand command)
{
  //This puts the text in the clipboard of the user in the clipboard of the browser.
  //The actual pasting is handled by the CTRL + V that is sent from the frontend.
  QString type = QString(command.arg(1));
  QByteArray bytes = QByteArray::fromBase64(command.arg(2).toUtf8());

  if (type == "text")
  {
    QGuiApplication::clipboard()->setText(QString(bytes));
  }
  if (type == "image")
  {
    QImage img;
    img.loadFromData(bytes);
    QGuiApplication::clipboard()->setImage(img);
  }

  // for (int i = 0; i < value.length(); i++)
  // {
  //   this->onKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier, QString(value.at(i)));
  // }
}

void JxWindow::handleResize(JIncomingCommand command)
{
  int width = command.arg(1).toInt();
  int height = command.arg(2).toInt();
  _cleanGeometry = QRect(0, 0, width, height);
  QWindowSystemInterface::handleScreenGeometryChange(QGuiApplication::primaryScreen(), QRect(0, 0, width, height), QRect(0, 0, 5000, 5000));
  float ratio = QGuiApplication::primaryScreen()->devicePixelRatio();
  this->window->resize(width / ratio, height / ratio);
}

void JxWindow::handleMouseEvent(JIncomingCommand command)
{
  QString str = QString(QByteArray::fromBase64(command.arg(1).toStdString().c_str()));
  QJsonDocument jsonResponse = QJsonDocument::fromJson(str.toUtf8());
  QJsonObject object = jsonResponse.object();
  MouseEvent mouse(object, this->window->framePosition());
  this->onMouseEvent(mouse);
}

void JxWindow::handleFocusEvent(JIncomingCommand command)
{
  //  qInfo() << "[JxWindow::handleFocusEvent]";
  if (command.arg(1) == "lost")
  {
    // JxStreamer::instance().pause();
  }
  else if (command.arg(1) == "focus")
  {
    // JxStreamer::instance().resume();
  }
}
void JxWindow::onKeyEvent(QEvent::Type type, int code, int modifiers, const QString &text)
{
  QKeyEvent event(type, code, Qt::KeyboardModifiers(modifiers), text);
  QWindowSystemInterface::handleKeyEvent(this->window, type, code, Qt::KeyboardModifiers(modifiers), text); // This forces the event on the window and lets it bubble
}

void JxWindow::setGeometry(const QRect &rect)
{
  _geometry = rect;

  if (this->window->objectName() == "QMainWindowClassWindow" || this->window->objectName() == "JPopupWindowClassWindow")
  {
    qInfo() << "Finding streamer in jxwindow for objectname " << this->window->objectName() << " and the winid is " << this->window->winId();
    auto streamer = JxBackingStore::streamers().value(this->window->winId());
    if(streamer == nullptr){
      qInfo() << "No streamer found! Something went really wrong ";
      return;
    }
    JxBackingStore::streamers().value(this->window->winId())->resize(QSize(rect.width(), rect.height()));
  }

  QWindowSystemInterface::handleGeometryChange(this->window, rect);
}

QRect JxWindow::geometry() const
{
  return _geometry;
}

void JxWindow::onMouseEvent(MouseEvent &mouse)
{

  Qt::MouseButton button;
  switch (mouse.which())
  {
  case 1:
    button = Qt::LeftButton;
    break;
  case 2:
    button = Qt::MidButton;
    break;
  case 3:
    button = Qt::RightButton;
    break;
  }

  if (mouse.EventType() == "mousedown")
  {
    QWindowSystemInterface::handleMouseEvent(this->window, QPoint(mouse.LayerX(), mouse.LayerY()), QPoint(mouse.LayerX(), mouse.LayerY()),
                                             Qt::MouseButtons(button), button, QEvent::MouseButtonPress,
                                             mouse.modifiers(),
                                             Qt::MouseEventNotSynthesized);

    _leftMouseButton = "down";
  }
  else if (mouse.EventType() == "mouseup")
  {
    QWindowSystemInterface::handleMouseEvent(this->window, QPoint(mouse.LayerX(), mouse.LayerY()), QPoint(mouse.LayerX(), mouse.LayerY()),
                                             Qt::MouseButtons(button), button, QEvent::MouseButtonRelease,
                                             mouse.modifiers(),
                                             Qt::MouseEventNotSynthesized);

    _leftMouseButton = "up";
  }
  else if (mouse.EventType() == "contextmenu")
  {
    //   qInfo() << "[JxWindow::onMouseEvent] contextmenu";

    QWindowSystemInterface::handleMouseEvent(this->window, QPoint(mouse.LayerX(), mouse.LayerY()), QPoint(mouse.LayerX(), mouse.LayerY()),
                                             Qt::MouseButtons(Qt::RightButton), Qt::RightButton, QEvent::MouseButtonPress,
                                             mouse.modifiers(),
                                             Qt::MouseEventNotSynthesized);
  }
  else if (mouse.EventType() == "mousemove")
  {
    //   qInfo() << "[JxWindow::onMouseEvent] mousemove";

    if (_leftMouseButton == "down")
    {
      QWindowSystemInterface::handleMouseEvent(this->window, QPoint(mouse.LayerX(), mouse.LayerY()), QPoint(mouse.LayerX(), mouse.LayerY()),
                                               Qt::MouseButtons(Qt::LeftButton), Qt::LeftButton, QEvent::MouseMove,
                                               mouse.modifiers(),
                                               Qt::MouseEventNotSynthesized);
    }
    else
    {
      QWindowSystemInterface::handleMouseEvent(this->window, QPoint(mouse.LayerX(), mouse.LayerY()), QPoint(mouse.LayerX(), mouse.LayerY()),
                                               Qt::MouseButtons(Qt::NoButton), Qt::NoButton, QEvent::MouseMove,
                                               mouse.modifiers(),
                                               Qt::MouseEventNotSynthesized);
    }
  }
  else
  {
    JxLog() << "Unknown mouse event";
  }
}

void JxWindow::handleMouseWheel(JIncomingCommand command)
{
  double layerX = command.arg(1).toDouble();
  double layerY = command.arg(2).toDouble();
  double clientX = command.arg(3).toDouble();
  double clientY = command.arg(4).toDouble();
  int deltaX = static_cast<int>(command.arg(6).toDouble());
  int deltaY = static_cast<int>(command.arg(6).toDouble());
  Qt::KeyboardModifiers modifiers = Qt::KeyboardModifiers(command.arg(7).toInt());

  auto orientation = deltaY != 0 ? Qt::Vertical : Qt::Horizontal;
  QPointF localPos(layerX,
                   layerY);
  QPointF globalPos(clientX,
                    clientY);

  QPoint point = (orientation == Qt::Vertical) ? QPoint(0, -deltaY * 2) : QPoint(-deltaX * 2, 0);
  QWindowSystemInterface::handleWheelEvent(this->window,

                                           localPos,
                                           globalPos,
                                           QPoint(),
                                           point,
                                           modifiers);
}

void JxWindow::handleTouchEvent(JIncomingCommand command)
{
  QString str = QString(QByteArray::fromBase64(command.arg(1).toStdString().c_str()));

  QJsonDocument jsonResponse = QJsonDocument::fromJson(str.toUtf8());

  QJsonObject object = jsonResponse.object();

  const auto time = object.value("time").toDouble();

  if (object.value("event").toString() == QStringLiteral("touchcancel"))
  {
    QWindowSystemInterface::handleTouchCancelEvent(this->window,
                                                   time,
                                                   _touchDevice,
                                                   Qt::NoModifier);
  }
  else
  {
    QList<QWindowSystemInterface::TouchPoint> points;

    auto changedTouches = object.value("changedTouches").toArray();
    for (const auto &touch : changedTouches)
    {
      auto point = TouchObject(window->width(), window->height(), object.value("event").toString(), touch.toObject()).point();
      points.append(point);
    }

    auto stationaryTouches = object.value("stationaryTouches").toArray();
    for (const auto &touch : stationaryTouches)
    {
      auto point = TouchObject(window->width(), window->height(), QString("stationary"), touch.toObject()).point();
      points.append(point);
    }

    QWindowSystemInterface::handleTouchEvent(this->window,
                                             time,
                                             _touchDevice,
                                             points,
                                             Qt::NoModifier);
  }
}

void JxWindow::handleQualityEvent(JIncomingCommand command)
{
  JxBackingStore::streamers().value(this->window->winId())->changeQuality(command.arg(1).toInt());
}

void JxWindow::handlePixelRatioEvent(JIncomingCommand command)
{
  float ratio = command.arg(1).toFloat();
  QHighDpiScaling::setScreenFactor(QGuiApplication::primaryScreen(), ratio);
  JxBackingStore::streamers().value(this->window->winId())->setPixelRatio(ratio);
  // JxStreamer::instance().setPixelRatio(ratio); todo
  this->window->resize(_cleanGeometry.width() / ratio, _cleanGeometry.height() / ratio + 1);
}

void JxWindow::handlePing()
{
  JCommand(OC::PONG, QGuiApplication::focusWindow()->winId());
}

void JxWindow::handleWindowClosed()
{

  // if (JxWsServer::instance().countConnectedClients() <= 1)
  // {
  //   qInfo() << "handleWindowClosed exit";
  //   exit(0);
  //   QGuiApplication::quit();
  // } else {
  //       qInfo() << "handleWindowClosed";

  // }
}