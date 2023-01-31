/// COPY FROM https://stackoverflow.com/questions/21773348/how-to-force-qcompleter-to-check-second-word-in-qlineedit
#pragma once

#include <QCompleter>

class QStringListModel;
class QLineEdit;

class DelimitedCompleter : public QCompleter {
    Q_OBJECT

public:
    DelimitedCompleter(QLineEdit *parent, char delimiter);
    DelimitedCompleter(QLineEdit *parent, char delimiter, QAbstractItemModel *model);
    DelimitedCompleter(QLineEdit *parent, char delimiter, const QStringList &list);
    QString     pathFromIndex(const QModelIndex &index) const;
    QStringList splitPath(const QString &path) const;

private:
    char        delimiter;
    mutable int cursor_pos = -1;

    void connectSignals();

private slots:
    void onActivated(const QString &text);
    void onCursorPositionChanged(int old_pos, int new_pos);
};
