#include <QJsonObject>
#include <QGuiApplication>
class TouchObject
{

    int _width;
    int _height;

    QString _eventType;

    int _identifier;

    double _force;
    double _pageX;
    double _pageY;
    double _radiusX;
    double _radiusY;
    double _clientX;
    double _clientY;

    double _screenX;
    double _screenY;

public:
    TouchObject(int width, int height, const QString &eventType, const QJsonObject &object)
        : _width(width), _height(height),
          _eventType(eventType),
          _identifier(object.value("identifier").toInt(0)),
          _force(object.value("force").toDouble(1.)),
          _pageX(object.value("pageX").toDouble()), _pageY(object.value("pageY").toDouble()),
          _radiusX(object.value("radiusX").toDouble()), _radiusY(object.value("radiusY").toDouble()),
          _clientX(object.value("clientX").toDouble()), _clientY(object.value("clientY").toDouble()),
          _screenX(object.value("screenX").toDouble()),
          _screenY(object.value("screenY").toDouble())
    {
    }

    QWindowSystemInterface::TouchPoint point()
    {
        double ratio = QGuiApplication::primaryScreen()->devicePixelRatio();
        // qInfo() << ratio;
        QWindowSystemInterface::TouchPoint point;
        point.id = _identifier;
        point.pressure = _force;
        point.area.setX(_pageX* ratio - _radiusX);
        point.area.setY(_pageY* ratio - _radiusY);
        point.area.setWidth(_radiusX * 2);
        point.area.setHeight(_radiusY * 2);
        point.normalPosition.setX(_screenX * ratio / _width);
        point.normalPosition.setY(_screenY * ratio / _height);
        point.rawPositions = {{_clientX, _clientY}};

        if (_eventType == QStringLiteral("touchstart"))
        {
            point.state = Qt::TouchPointPressed;
        }
        else if (_eventType == QStringLiteral("touchend"))
        {
            //qCDebug(lcWebGL, ) << "end" << object;
            point.state = Qt::TouchPointReleased;
        }
        else if (_eventType == QStringLiteral("touchmove"))
        {
            point.state = Qt::TouchPointMoved;
        }
        else if (_eventType == QStringLiteral("stationary"))
        {
            point.state = Qt::TouchPointStationary;
        }

        return point;
    }
};