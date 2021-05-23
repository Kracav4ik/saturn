#pragma once

#include "OpenLL/Camera.h"
#include <QMainWindow>
#include <QTimer>
#include "ui_main.h"

namespace ll { class DrawAPI; }

class MainWindow : public QMainWindow, private Ui::MainWindow {
public:
    MainWindow();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    ll::Camera cam;
    QMap<quint32, bool> keys;

    QTimer timer;
};
