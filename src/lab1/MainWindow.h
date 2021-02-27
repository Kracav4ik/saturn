#pragma once

#include "OpenLL/Framebuffer.h"
#include "OpenLL/DrawAPI.h"
#include <QMainWindow>
#include <QTimer>
#include "ui_main.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
public:
    MainWindow();

protected:
    void wheelEvent(QWheelEvent* event) override;

private:
    void drawScene(
            ll::Framebuffer& fb,
            QGraphicsView* drawArea,
            ll::Matrix4x4 dynamicRot,
            ll::Matrix4x4 staticRot,
            bool isPerspective,
            QGraphicsScene& scene
    );

    void drawCenter(float width, ll::Color colorLeft, ll::Color colorRight);

    ll::Framebuffer fbs[4];
    ll::DrawAPI drawAPi;
    QTimer timer;
};
