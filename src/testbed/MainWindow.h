#pragma once

#include "OpenLL/Framebuffer.h"
#include "OpenLL/DrawAPI.h"
#include <QMainWindow>
#include <QTimer>
#include "ui_main.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
public:
    MainWindow();

private:
    ll::Framebuffer fb;
    ll::DrawAPI drawAPi;
    QTimer timer;
    QImage img;
};
