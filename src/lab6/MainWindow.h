#pragma once

#include <QMainWindow>
#include <QTimer>
#include "ui_main.h"

namespace ll { class DrawAPI; }

class MainWindow : public QMainWindow, private Ui::MainWindow {
public:
    MainWindow();

protected:
    void wheelEvent(QWheelEvent* event) override;

private:
    QTimer timer;
};
