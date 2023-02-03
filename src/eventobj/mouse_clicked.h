#pragma once

#include <QObject>

class QTimer;
namespace EventObj {

class MouseClicked : public QObject {
    Q_OBJECT;

public:
    MouseClicked(QObject* parent = nullptr, Qt::MouseButton button = Qt::LeftButton);

    bool eventFilter(QObject* obj, QEvent* e);
signals:
    void clicked();

private:
    Qt::MouseButton button_;
    QTimer*         timer_;
};
}    // namespace EventObj
