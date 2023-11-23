#include "messager.h"
#include "ui_messager.h"
#include <QFileDialog>
#include <QLabel>
#include <cassert>

Messager::Messager(QWidget *parent)
    : QWidget(parent), ui(new Ui::Messager), data() {
  ui->setupUi(this);
  socket = new QTcpSocket(this);
  socket->connectToHost("127.0.0.1", 2323);
  connect(socket, &QTcpSocket::readyRead, this, &Messager::isReadyToRead);
  connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

Messager::~Messager() { delete ui; }

void Messager::isReadyToRead() {
  QDataStream in(socket);
  qint64 estimatedSize;

  QByteArray temp = socket->readAll();

  if (temp[0] == identificators::sizeInfo) {
    data = temp.sliced(1);
    estimatedSize = deatachSizeFromData(data);
  }

  if (data.size() < estimatedSize) {
    for (int i = 0; i < temp.size() && !(temp[i] == identificators::endMessage);
         ++i) {
      data += temp;
    }
    return;
  }

  const char identifier = data[0];

  data = data.sliced(1);
  qDebug() << data.size() << "data length on client";

  if (identifier == identificators::imageMessage) {
    QLabel *lbl = new QLabel();
    lbl->setAlignment(Qt::AlignCenter);

    QPixmap p;

    if (p.loadFromData(data, "JPG")) {
      lbl->setPixmap(p);
      lbl->show();
    }

  } else if (identifier == identificators::textMessage) {
    QString str = QString::fromUtf8(data);
    ui->textBrowser->append(str);
  } else {
    qDebug() << "client: message is not resolved";
  }
  data.clear();
}

void Messager::sendToServer(QByteArray sendingData) {
  addSizeInfoToData(sendingData);
  data = sendingData + identificators::endMessage;
  qDebug() << data;
  socket->write(data);
  data.clear();
}

void Messager::on_pushButton_clicked() {
  data = identificators::textMessage + ui->textEdit->toPlainText().toUtf8();
  sendToServer(data);
}

void Messager::on_pushButton_2_clicked() {
  QFileDialog dialog(this);
  QStringList fileNames;
  QPixmap img;
  QByteArray temp;
  QBuffer buffer(&temp);

  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
  dialog.setViewMode(QFileDialog::Detail);
  dialog.setStyleSheet("color: white;");

  if (dialog.exec())
    fileNames = dialog.selectedFiles();

  img.load(fileNames[0], "JPG");
  img = img.scaledToHeight(720, Qt::SmoothTransformation);
  img.save(&buffer, "JPG");
  data = identificators::imageMessage + temp;

  qDebug() << data.size() << " bytes img sent from client";
  sendToServer(data);
}

void Messager::addSizeInfoToData(QByteArray &data) {
  qint64 sz = data.size();

  QByteArray arrsize =
      QByteArray::fromRawData(reinterpret_cast<const char *>(&sz), 8);

  data = identificators::sizeInfo + arrsize +
         data; // sizeinfo - спецсимвол, что это начало qbytearray
}

qint64 Messager::deatachSizeFromData(QByteArray &data) {
  qint64 sdata;
  std::memcpy(&sdata, data, 8);

  data = data.sliced(8);
  qDebug() << sdata << "bytes waiting";
  return sdata;
}
