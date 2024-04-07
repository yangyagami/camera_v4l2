#include "mainwindow.h"

#include <QImage>
#include <QPixmap>

#include "ui_mainwindow.h"

#define CAMERA_V4L2_IMPLEMENTATION
#include "../camera_v4l2.h"

CameraThread::CameraThread(QObject *parent) : QThread(parent) {
  camera_ = nullptr;
  camera_ = camera_v4l2_create();

  if (camera_ != nullptr) {
    camera_v4l2_param_t param;
    param.frame_width = 640;
    param.frame_height = 480;
    param.fmt = MJPEG;
    camera_v4l2_open(camera_, 2, &param);
  }
}

CameraThread::~CameraThread() {
  camera_v4l2_destroy(camera_);
}

void CameraThread::run() {
  while (true) {
    camera_v4l2_buffer_t buf;
    int ret = camera_v4l2_read(camera_, &buf);
    if (ret) {
      QImage image = QImage::fromData((const unsigned char*)buf.start, buf.length);
      emit ReadFrameSignal(image.copy());
    }
    QThread::msleep(10);
  } 
}

MainWindow::MainWindow(QWidget *parent) 
    : QWidget(parent),
      ui_(new Ui::MainWindow) {
  ui_->setupUi(this);

  camera_thread_ = new CameraThread(this);

  connect(camera_thread_, &CameraThread::ReadFrameSignal, this, [=](QImage frame){
    ui_->video_label->setPixmap(QPixmap::fromImage(frame));
  });

  camera_thread_->start();
}

MainWindow::~MainWindow() {
  delete ui_;
}
