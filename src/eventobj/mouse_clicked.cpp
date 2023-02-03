#include "mouse_clicked.h"

#include <chrono>

#include <QEvent>
#include <QMouseEvent>
#include <QTimer>

namespace EventObj {

MouseClicked::MouseClicked(QObject* parent, Qt::MouseButton button)
    : QObject(parent), timer_(new QTimer(this)), button_(button) {
    timer_->setSingleShot(true);
}

bool MouseClicked::eventFilter(QObject* obj, QEvent* e) {

    do {
        switch(e->type()) {
            case QEvent::MouseButtonPress:
                {
                    auto ev = static_cast<QMouseEvent*>(e);
                    if(ev->button() != button_) break;

                    timer_->start(std::chrono::milliseconds(300));
                    break;
                }
            case QEvent::MouseButtonRelease:
                {
                    auto ev = static_cast<QMouseEvent*>(e);
                    if(ev->button() != button_) break;

                    if(timer_->isActive()) {
                        timer_->stop();
                        emit signalClicked();
                    }
                    break;
                }
            default: break;
        }
    } while(0);

    return false;
}
MouseDblClick::MouseDblClick(QObject* parent, Qt::MouseButton button) : QObject(parent), button_(button) { }

bool MouseDblClick::eventFilter(QObject* obj, QEvent* e) {
    do {
        if(e->type() == QEvent::MouseButtonDblClick) {
            auto ev = static_cast<QMouseEvent*>(e);
            if(ev->button() == button_) {
                emit signalDblClicked();
            }
        }
    } while(0);

    return false;
}
}    // namespace EventObj
