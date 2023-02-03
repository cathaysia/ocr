#include "focus.h"

#include <QEvent>

namespace EventObj {

Focus::Focus(QObject* parent) { }

bool Focus::eventFilter(QObject* obj, QEvent* e) {
    switch(e->type()) {
        case QEvent::FocusIn:
            {
                emit signalFocusIn();
                break;
            }
        case QEvent::FocusOut:
            {
                emit signalFocusOut();
                break;
            }
        default: break;
    }

    return false;
}

}    // namespace EventObj
