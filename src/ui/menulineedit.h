#pragma once

#include <QLineEdit>

class QStringListModel;

class MenuLineEdit : public QLineEdit {
public:
    MenuLineEdit(QWidget* parent = nullptr);
    void SetCmpList(QStringList const& list);
    void SetModel(QStringList const& list);

private:
    QStringList       cmpList_;
    QCompleter*       comp_;
    QStringListModel* model_;
};
