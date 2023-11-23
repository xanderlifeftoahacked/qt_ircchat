#ifndef MESSAGER_H
#define MESSAGER_H

#include "constants.h"
#include <QBuffer>
#include <QDebug>
#include <QTcpSocket>
#include <QWidget>

namespace Ui {
class Messager;
}

class Messager : public QWidget {
  Q_OBJECT

public:
  explicit Messager(QWidget *parent = nullptr);
  ~Messager();

private:
  Ui::Messager *ui;
  QTcpSocket *socket;
  QByteArray data;
  void sendToServer(QByteArray sendingData);
  void addSizeInfoToData(QByteArray &data);
  qint64 deatachSizeFromData(QByteArray &data);

public slots:
  void isReadyToRead();

private slots:
  void on_pushButton_clicked();
  void on_pushButton_2_clicked();
};

#endif // MESSAGER_H
