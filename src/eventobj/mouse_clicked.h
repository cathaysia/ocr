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
    void signalClicked();

private:
    Qt::MouseButton button_;
    QTimer*         timer_;
};

class MouseDblClick : public QObject {
    Q_OBJECT;

public:
    MouseDblClick(QObject* parent = nullptr, Qt::MouseButton button = Qt::LeftButton);

    bool eventFilter(QObject* obj, QEvent* e);
signals:
    void signalDblClicked();

private:
    Qt::MouseButton button_;
};
}    // namespace EventObj
