
#ifndef __TLS_H__
#define __TLS_H__

inline QDebug operator<<(QDebug debug, const std::string& s) {
    debug.noquote() << QString::fromStdString(s);
    return debug;
}

inline QString fmt(double num, uint precision, uint len_int = 1, QChar with_prefix = ' ') {

    QString res = QString::number(num, 'f', precision);

    if (len_int == 0 && precision > 0) {
        if (res.left(1) == "0") res = res.mid(1);
        else
            if (res.left(2) == "-0")
                res = QString("-") + res.mid(2);
    }

    int res_len = len_int + (!precision ? 0 : 1 + precision);

    if (with_prefix != 0)
        while (res.length() < res_len)
            res = with_prefix + res;

    return res;
}

inline QString getEditText(QLineEdit* ed, const QString& def) {

    QString s = ed->text();
    if (!s.isEmpty())
        return s;

    ed->setText(def);
    return def;
}

#endif
