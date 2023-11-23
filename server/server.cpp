#include "server.h"

Server::Server() {
  if (this->listen(QHostAddress::Any, 2323)) {
    qDebug() << "start";
    return;
  }

  qDebug() << "error";
}

void Server::incomingConnection(qintptr descriptor) {
  socket = new QTcpSocket;
  socket->setSocketDescriptor(descriptor);
  sockets.push_back(socket);

  connect(socket, &QTcpSocket::readyRead, this, &Server::isReadyToRead);
  connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

  qDebug() << "new connection - " << descriptor;
  readenData.clear();
}

void Server::isReadyToRead() {
  socket = static_cast<QTcpSocket *>(sender());
  data.clear();

  readenData = socket->readAll();

  qDebug() << readenData.size() << "readed server";
  qDebug() << readenData;
  sendClient(readenData);
}

void Server::sendClient(QByteArray sendingData) {
  for (QTcpSocket *sock : sockets) {
    sock->write(sendingData);
  }
}
