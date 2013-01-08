#include "smallfunctions.h"

QString giveName(int i){
    int k='Z' - 'A';
    int alpha = (i % k) + 'A';
    int numeric = i / k;
    return QString("%1%2").arg(char(alpha)).arg(QString::number(numeric));
}

QRectF enlargeRect(QPointF point, QRectF rect) {
    QPointF d (rect.width(),rect.height());
    QRectF arect = QRectF(point+d,point-d).normalized();
    return arect.united(rect);
}
