
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>
#include <QGuiApplication>

#include <QFile>
#include <QThread>
#include <QProcess>
#include <QUrlQuery>
#include <fcntl.h> /* For O_RDWR */
#include <unistd.h>
#include <QTimer>

#include "jxlog.h"
#include "audiostreaming/Encoder.h"
#include "jxwsserver.h"
#include "jxintegration.h"

#define QPA_PORT 9001
#define BROWSER_PORT 6662
#define AUDIO_PORT 6663

JxWsServer::JxWsServer() : QObject(Q_NULLPTR), _tcpServer(new QTcpServer()), _audioTcpServer(new QTcpServer()),
                           m_pWebSocketServer(new QWebSocketServer(QStringLiteral("JimberX264Wsserver"),
                                                                   QWebSocketServer::NonSecureMode, this))
{
    qInfo() << " IN JXWS";
    if (m_pWebSocketServer->listen(QHostAddress::Any, QPA_PORT))
    {
        JxLog() << "JxWsServer listening on port" << QPA_PORT;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &JxWsServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &JxWsServer::closed);
    }

    if (_tcpServer->listen(QHostAddress::LocalHost, BROWSER_PORT))
    {
        qInfo() << "Drawing hub tcpsock listening on port" << BROWSER_PORT;
        connect(_tcpServer, &QTcpServer::newConnection,
                this, &JxWsServer::onNewTcpConnection);
        //  connect(_tcpServer, &QTcpServer::closed, this, &glHub::closed);
    }

    if (_audioTcpServer->listen(QHostAddress::LocalHost, AUDIO_PORT))
    {
        qInfo() << "Drawing audio tcpsock listening on port" << AUDIO_PORT;
        connect(_audioTcpServer, &QTcpServer::newConnection,
                this, &JxWsServer::onNewAudioConnection);
        //  connect(_tcpServer, &QTcpServer::closed, this, &glHub::closed);
    }
}

JxWsServer::~JxWsServer()
{
    m_pWebSocketServer->close();
    // qDeleteAll(m_clients.keys().begin(), m_clients.keys().end()); segfaults?
}

void JxWsServer::onNewConnection()
{
    // qInfo() << "[JxWsServer::onNewConnection]";
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
    QString userId = qgetenv("USER_ID");
    QUrlQuery query(pSocket->requestUrl());
    if (!userId.isEmpty())
    {
        QString dirtyUserID = pSocket->requestUrl().path().split("/")[2];
        if (dirtyUserID != userId)
        {
            pSocket->sendTextMessage(QString("close;"));
            pSocket->close();
            qInfo() << "Wrong user connected";
            return;
        }
    }

    QNetworkRequest nreq = pSocket->request();
    QString userLang = QString(nreq.rawHeader(QByteArray("accept-language")));
    auto queryWinId = query.queryItemValue("winId");
    int winId = 1;
    if (queryWinId.isEmpty())
    {
        winId = JxIntegration::instance()->createBrowser(true)(userLang);
    }
    else
    {
        winId = queryWinId.toInt();
    }

    JxLog() << "Client connected";
    connect(pSocket, &QWebSocket::textMessageReceived, this, &JxWsServer::processTextMessage, Qt::QueuedConnection);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &JxWsServer::processBinaryMessage, Qt::QueuedConnection);
    connect(pSocket, &QWebSocket::disconnected, this, &JxWsServer::socketDisconnected);

    m_clients.insert(pSocket, winId);

    pSocket->sendTextMessage(QString("socketready;"));

    sendQueuedMessages(winId);
}

void JxWsServer::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    int winId = m_clients.value(pClient);
    JxIntegration::instance()->onMessage(true)(message, winId); //send message to browser
    emit textMessageReceived(sender(), message, winId);         // send message to qpa
}

void JxWsServer::processBinaryMessage(QByteArray message)
{
    //sendBinaryMessageToBrowser(message);
    JxIntegration::instance()->onBinaryMessage(true)(message); //send message to browser
    emit binaryMessageReceived(sender(), message);
}

void JxWsServer::socketDisconnected()
{
    qInfo() << "[JxWsServer::socketDisconnected]";
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qInfo() << " A window was closed with this windid: " << m_clients.value(pClient);
    JxIntegration::instance()->closeWindow(false)(m_clients.value(pClient));
    pClient->deleteLater();
    m_clients.remove(pClient);

    QTimer::singleShot(10000, this, &JxWsServer::checkForExit);
}

void JxWsServer::checkForExit()
{

    if (m_clients.size() <= 0)
    {
        qInfo() << "have to exit!??";
        QGuiApplication::quit();
    }
    else
    {
        qInfo() << "I dont have to exit!";
    }
}

void JxWsServer::sendQueuedMessages(int winId)
{
    if (m_clients.size() == 0)
    {
        return;
    }

    QStringListPtr messageList = m_queuedMessages.value(winId); //@todo no winid makes this crash, check nullptr

    if (messageList == nullptr || messageList->isEmpty())
    {
        return;
    }

    auto client = m_clients.key(winId);

    for (int i = 0; i < messageList->size(); ++i)
    {
        client->sendTextMessage(messageList->value(i));
    }
}

void JxWsServer::sendToAllClients(const QString &message, int windowId)
{

    if (windowId == -1)
    {
        for (auto client : m_clients.keys())
        {
            client->sendTextMessage(message);
        }

        return;
    }

    if (!m_clients.key(windowId))
    {
        if (!m_queuedMessages.contains(windowId))
        {
            m_queuedMessages.insert(windowId, QStringListPtr::create());
        }
        m_queuedMessages.value(windowId)->append(message);
        return;
    }

    m_clients.key(windowId)->sendTextMessage(message);
}

void JxWsServer::sendBinToAllClients(const QByteArray &message)
{
    for (auto client : m_clients.keys())
    {
        client->sendBinaryMessage(message);
    }
}
void JxWsServer::sendBinToFirstClient(const QByteArray &message)
{
    if (m_clients.isEmpty())
    {
        return;
    }
    m_clients.firstKey()->sendBinaryMessage(message);
}

void JxWsServer::sendToAllClients(const QStringList &message)
{
    for (auto client : m_clients.keys())
    {
        client->sendTextMessage(message.join(";"));
    }
}
void JxWsServer::disconnectAll()
{
    for (auto client : m_clients.keys())
    {
        client->close();
    }
}
void JxWsServer::onNewTcpConnection()
{
    QTcpSocket *socket = _tcpServer->nextPendingConnection();
    qInfo() << "Browser connected to QPA";
    connect(socket, &QTcpSocket::readyRead, this, &JxWsServer::processTcpMessage);
    connect(socket, &QTcpSocket::disconnected, this, &JxWsServer::socketTcpDisconnected);

    _tcpClients << socket;
}

void JxWsServer::onNewAudioConnection()
{
    QTcpSocket *socket = _audioTcpServer->nextPendingConnection();
    qInfo() << "Audio connected to QPA";
    connect(socket, &QTcpSocket::readyRead, this, &JxWsServer::processAudioMessage);
}

void JxWsServer::socketTcpDisconnected()
{
    QTcpSocket *pClient = qobject_cast<QTcpSocket *>(sender());
    if (pClient)
    {

        _tcpClients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void JxWsServer::processTcpMessage()
{

    QTcpSocket *socket = static_cast<QTcpSocket *>(sender());

    QStringList list;
    while (socket->canReadLine())
    {
        QString data = QString(socket->readLine());

        //   sendToAllClients(data);
    }
}

bool isEmpty(char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (data[i] != 0)
        {
            return false;
        }
    }
    return true;
}

void JxWsServer::processAudioMessage()
{

    QTcpSocket *socket = static_cast<QTcpSocket *>(sender());
    if (socket->bytesAvailable() < 1152 * 10)
        return;

    auto bytesAv = static_cast<size_t>(socket->bytesAvailable());
    QByteArray rawAudioData(bytesAv, 0);
    socket->read(rawAudioData.data(), bytesAv);

    bool empty = isEmpty(rawAudioData.data(), bytesAv);
    if (empty)
        return;

    wavify(rawAudioData, 1, 44100);
    // wavify(rawAudioData, 1, 22050);

    rawAudioData.insert(0, QString("audio").toUtf8());
    // sendBinToAllClients(rawAudioData);
    sendBinToFirstClient(rawAudioData);
}

void JxWsServer::sendVideo(const QByteArray &message, int windowId)
{
    for (auto client : m_clients.keys())
    {
        if (windowId != -1 && m_clients.value(client) == windowId)
        {
            client->sendBinaryMessage(message);
        }
    }
}

int JxWsServer::countConnectedClients()
{
    return m_clients.size();
}