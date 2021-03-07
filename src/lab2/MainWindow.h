#pragma once

#include <QMainWindow>
#include <QTimer>
#include "ui_main.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
public:
    MainWindow();

protected:
    void wheelEvent(QWheelEvent* event) override;

private:
    void drawCenter(ll::DrawAPI& drawAPi, float width, ll::Color colorLeft, ll::Color colorRight);

    QTimer timer;

    ll::CullMode cullMode;
};
