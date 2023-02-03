#pragma once

#include <QObject>

namespace EventObj {
class Focus : public QObject {
    Q_OBJECT;

public:
    Focus(QObject* parent = nullptr);

    bool eventFilter(QObject* obj, QEvent* e);
signals:
    void signalFocusIn();
    void signalFocusOut();
};
}    // namespace EventObj
