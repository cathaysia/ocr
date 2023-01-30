#pragma once

#include <QDialog>
#include <QPixmap>

class QMouseEvent;

class ScreenCapture : public QDialog {
    Q_OBJECT;

public:
    ScreenCapture(QWidget* parent = nullptr);

    void capture();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

signals:
    void signalScreenReady(QPixmap const& pix);

private:
    QPixmap pix_;

    QPoint startPoint_;
    QPoint endPoint_;
};
