#include "delimitedcompleter.h"

#include <QDebug>
#include <QLineEdit>
#include <QStringList>

DelimitedCompleter::DelimitedCompleter(QLineEdit *parent, char delimiter) : QCompleter(parent), delimiter(delimiter) {
    parent->setCompleter(this);
    connectSignals();
}

DelimitedCompleter::DelimitedCompleter(QLineEdit *parent, char delimiter, QAbstractItemModel *model)
    : QCompleter(model, parent), delimiter(delimiter) {
    parent->setCompleter(this);
    connectSignals();
}

DelimitedCompleter::DelimitedCompleter(QLineEdit *parent, char delimiter, const QStringList &list)
    : QCompleter(list, parent), delimiter(delimiter) {
    parent->setCompleter(this);
    connectSignals();
}

QString DelimitedCompleter::pathFromIndex(const QModelIndex &index) const {
    QString    auto_string = index.data(Qt::EditRole).toString();
    QLineEdit *line_edit   = qobject_cast<QLineEdit *>(parent());
    QString    str         = line_edit->text();

    // If cursor position was saved, restore it, else save it
    if(cursor_pos != -1) line_edit->setCursorPosition(cursor_pos);
    else
        cursor_pos = line_edit->cursorPosition();

    // Get current prosition
    int cur_index = line_edit->cursorPosition();

    /**
     * NOTE
     *
     * prev_delimiter_index should actually point at final white space AFTER the delimiter.
     */

    // Get index of last delimiter before current position
    int prev_delimiter_index = str.mid(0, cur_index).lastIndexOf(delimiter);
    while(prev_delimiter_index < str.size() - 1 && str.at(prev_delimiter_index + 1).isSpace()) prev_delimiter_index++;

    // Get index of first delimiter after current position (or EOL if no delimiter after cursor)
    int next_delimiter_index = str.indexOf(delimiter, cur_index);
    if(next_delimiter_index == -1) {
        next_delimiter_index = str.size();
    }

    // Get part of string that occurs before cursor
    QString part1 = str.mid(0, prev_delimiter_index + 1);

    // Get string value from before auto finished string is selected
    QString pre = str.mid(prev_delimiter_index + 1, cur_index - prev_delimiter_index - 1);

    // Get part of string that occurs AFTER cursor
    QString part2 = str.mid(next_delimiter_index);

    return part1 + auto_string + part2;
}

QStringList DelimitedCompleter::splitPath(const QString &path) const {
    QLineEdit *line_edit = qobject_cast<QLineEdit *>(parent());

    QStringList string_list;
    int         index = path.mid(0, line_edit->cursorPosition()).lastIndexOf(delimiter) + 1;
    QString     str   = path.mid(index, line_edit->cursorPosition() - index).trimmed();
    string_list << str;
    return string_list;
}

void DelimitedCompleter::connectSignals() {
    connect(this, SIGNAL(activated(const QString &)), this, SLOT(onActivated(const QString &)));

    connect(qobject_cast<QLineEdit *>(parent()), SIGNAL(cursorPositionChanged(int, int)), this,
            SLOT(onCursorPositionChanged(int, int)));
}

void DelimitedCompleter::onActivated(const QString &text) {
    cursor_pos = -1;
}

void DelimitedCompleter::onCursorPositionChanged(int old_pos, int new_pos) {
    if(old_pos != cursor_pos) cursor_pos = -1;
}
