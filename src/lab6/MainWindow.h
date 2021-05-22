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
    void drawCenter(ll::DrawAPI& drawAPi, float width, ll::Color colorLeft, ll::Color colorRight);

    QTimer timer;

    std::shared_ptr<Model> model;
};
