#ifndef QT_TEST_MAINWINDOW_H_
#define QT_TEST_MAINWINDOW_H_

#include <QWidget>
#include <QThread>
#include <QObject>
#include <QImage>

#include "../camera_v4l2.h"

namespace Ui {
  class MainWindow;
}

class CameraThread : public QThread {
  Q_OBJECT
 signals:
  void ReadFrameSignal(QImage frame);
 public:
  explicit CameraThread(QObject *parent = nullptr);

  virtual ~CameraThread();

  virtual void run() override;
 private:
  camera_v4l2_camera_t *camera_;
};

class MainWindow : public QWidget {
 public:
  explicit MainWindow(QWidget *parent = nullptr);
  virtual ~MainWindow();
 private:
  Ui::MainWindow *ui_;
  CameraThread *camera_thread_;
};

#endif  // QT_TEST_MAINWINDOW_H_
