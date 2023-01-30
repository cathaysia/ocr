#include "menulineedit.h"

#include <QAction>
#include <QCompleter>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QStringListModel>

#include "delimitedcompleter.h"

MenuLineEdit::MenuLineEdit(QWidget *parent)
    : QLineEdit(parent), comp_(new DelimitedCompleter(this, ' ')), model_(new QStringListModel) {
    comp_->setModel(model_);
    this->setCompleter(comp_);

    connect(this, &QLineEdit::textChanged, [this](QString const &text) {
        SetModel(cmpList_);
        comp_->complete();
    });
}

void MenuLineEdit::SetCmpList(QStringList const &list) {
    cmpList_ = list;
    SetModel(list);
};

void MenuLineEdit::SetModel(QStringList const &list) {
    auto existsData = this->text().split(",");
    auto mapData    = QMap<QString, bool>();

    for(auto const &item: existsData) {
        auto v = item.trimmed();
        if(v.isEmpty()) continue;

        mapData[v] = true;
    }

    QStringList res;
    for(auto const &item: list) {
        if(mapData[item] == true) {
            continue;
        }

        res.append(item);
    }

    model_->setStringList(res);
};
