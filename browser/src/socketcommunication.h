#ifndef SOCKETCOM_H
#define SOCKETCOM_H
#include <functional>
#include <QString>
#include <QDebug>
#include <QObject>
#include <QGuiApplication>

typedef void (*SendToAllClientsPtr)(QString, WId);


typedef QMap<QString, QFunctionPointer> FunctionMap;
typedef void (*setFunctionsCallbackPtr)(FunctionMap);


void onWsMessage(QString message, int winId);
void onBinaryWsMessage(QByteArray message);



class QPACommunication : public QObject
{
    Q_OBJECT;

public:
    static QPACommunication &instance()
    {
        static QPACommunication instance; // Guaranteed to be destroyed.
        return instance;
    }

private:
    SendToAllClientsPtr _sendToAllClients;

    explicit QPACommunication(QObject *parent = nullptr);
    virtual ~QPACommunication(){};

public:
    void sendToAllClients(QString send, WId windowId)
    {
        if (_sendToAllClients)
        {
            _sendToAllClients(send, windowId);
        }
        else
        {
        }
    }
    QPACommunication(QPACommunication const &) = delete;
    void operator=(QPACommunication const &) = delete;

public slots:
    void onTextMessage(QString message, int winId)
    {
        emit textMessageReceived(message, winId);
    }
    void onBinaryMessage(QByteArray message)
    {
        emit binaryMessageReceived(message);
    }

signals:
    void textMessageReceived(QString, WId);
    void binaryMessageReceived(QByteArray);
};

inline void onWsMessage(QString message, int winId)
{
    QPACommunication::instance().onTextMessage(message, winId);
}
inline void onBinaryWsMessage(QByteArray message)
{
    QPACommunication::instance().onBinaryMessage(message);
}


#endif
