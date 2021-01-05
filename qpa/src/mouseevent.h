#include <QJsonObject>
#include <QGuiApplication>
#include <iostream>
#include <string>
#include <QScreen>

class MouseEvent
{
    int _identifier;
    double _clientX;
    double _clientY;
    double _windowOffsetX;
    double _windowOffsetY;
    QString _eventType;
    int _which;
    Qt::KeyboardModifiers _modifiers;
  public:
    MouseEvent(const QJsonObject &object, const QPoint &windowOffset)
    {
        _which = object.value("which").toInt();
        _eventType = object.value("type").toString();
        _clientX = object.value("clientX").toDouble();
        _clientY = object.value("clientY").toDouble();
        _modifiers = Qt::KeyboardModifiers(object.value("modifiers").toInt());
        _windowOffsetX = windowOffset.x();
        _windowOffsetY = windowOffset.y();
    };

    // double LayerX(){ return (_clientX - (_windowOffsetX * QGuiApplication::primaryScreen()->devicePixelRatio())); }
    // double LayerY(){return (_clientY - (_windowOffsetY * QGuiApplication::primaryScreen()->devicePixelRatio())); }

    Qt::KeyboardModifiers modifiers() { return _modifiers; }
    double LayerX(){ return (_clientX ); }
    double LayerY(){return (_clientY ); }
    QString EventType(){return _eventType;}
    int which(){ return _which; }
};