#pragma once
#include <QDataStream>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <string>

class Server : public QTcpServer {
  Q_OBJECT

public:
  Server();

private:
  QTcpSocket *socket;
  QVector<QTcpSocket *> sockets;
  QByteArray data;
  QByteArray readenData;
  void sendClient(QByteArray sendingData);

public slots:
  void incomingConnection(qintptr descriptor); // SPROSIT'
  void isReadyToRead();
};
