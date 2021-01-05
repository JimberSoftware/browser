#ifndef JIMBERX264WSSERVER
#define JIMBERX264WSSERVER

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QVector>
#include <QTcpServer>
#include <QTcpSocket>
#include <QQueue>
#include <QWebSocket>
#include <QGuiApplication>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

typedef QSharedPointer<QStringList> QStringListPtr;

class JxWsServer : public QObject
{
  Q_OBJECT
public:
  static JxWsServer &instance()
  {
    static JxWsServer instance; // Guaranteed to be destroyed.
                                // Instantiated on first use.
    return instance;
  }

  ~JxWsServer();
  void sendToAllClients(const QString &message, int windowId);

  void sendToAllClients(const QStringList &message);

  void disconnectAll();

  int countConnectedClients();


public slots:
  // void sendReply(void *sender, const QString &message);
  void sendBinToAllClients(const QByteArray &message);
  void sendBinToFirstClient(const QByteArray &message);
  void sendVideo(const QByteArray &message, int windowId);
signals:
  void closed();
  void connected();
  void textMessageReceived(void *sender, QString message, WId winId);
  void binaryMessageReceived(void *sender, QByteArray message);

private slots:
  void onNewConnection();
  void processTextMessage(QString message);
  void processBinaryMessage(QByteArray message);
  void socketDisconnected();

  void processTcpMessage();
  void onNewTcpConnection();
  void socketTcpDisconnected();

  void processAudioMessage();
  void onNewAudioConnection();
  void checkForExit();

private:
  QTcpServer *_tcpServer;
  QTcpServer *_audioTcpServer;
  QList<QTcpSocket *> _tcpClients;

  QWebSocketServer *m_pWebSocketServer;
  // QVector<QWebSocket *> m_clients;
  QMap<QWebSocket *, int> m_clients;
  // QQueue<QString> queuedMessages;
  QMap<int, QStringListPtr> m_queuedMessages;

  bool m_debug;
  JxWsServer();
  void sendQueuedMessages(int winId);
};
#endif