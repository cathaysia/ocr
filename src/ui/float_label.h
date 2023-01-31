#pragma once

#include <QDialog>

class QLabel;
class QMouseEvent;
class QPoint;
class QMenu;
class QTimer;

class FloatLabel : public QDialog {
    Q_OBJECT;

public:
    FloatLabel(QWidget* parent = nullptr);
    ~FloatLabel();

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;

    void SetPixmap(QPixmap const& pixmap);

signals:
    void signalRequestContextMenu(QPoint const& pos);

private:
    QMenu*  menu_;
    QLabel* lbl_img_;
    QPoint* startPoint_;
    QTimer* timer_;

    bool escPressed_;
};
