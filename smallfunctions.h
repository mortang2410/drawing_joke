#ifndef SMALLFUNCTIONS_H
#define SMALLFUNCTIONS_H
#include "qtmodules.h"
inline qreal mymod( qreal x, qreal y){
    return (x - qFloor(x/y)*y);
}

QString giveName(int i);

inline int getNearestPowerOfTwo(qreal x){
    Q_ASSERT(x>0);
    return qFloor(log(x)/log(2.));

}

inline qreal dotProduct(const QPointF & pointa,const  QPointF & pointb){
    return (pointa.x()*pointb.x() + pointa.y()*pointb.y());
}


inline qreal normalize(const QPointF & pointa){
    return qSqrt(pointa.x()*pointa.x() + pointa.y()*pointa.y());
}

inline QString tr ( const char * sourceText, const char * disambiguation = 0, int n = -1 ){
    return QObject::tr(sourceText,disambiguation,n);
}

inline QString number(int x){
    return QString::number(x);
}
inline QString number(qreal x){
    return QString::number(x);
}
inline QString bool2String(bool x){
    return QVariant(x).toString();
}
inline bool string2Bool(QString x){
    if ( x.toLower() == "true") return true;
    else return false;
}

inline void mydisconnect(QObject * obj1, QObject * obj2){
    if (obj1) QObject::disconnect(obj1,0,obj2,0);
    if (obj2) QObject::disconnect(obj2,0,obj1,0);
}
inline QPointF squarePoint(QPointF Old, QPointF New) {
    qreal X = New.x() - Old.x();
    qreal Y = New.y() - Old.y();
    (qAbs(X) > qAbs(Y)) ? (X = ( X>0 ? 1 : -1 ) * qAbs(Y)  )
                        : (Y = ( Y>0 ? 1 : -1 ) * qAbs(X)  );
    return Old + QPointF(X,Y);
}

QRectF enlargeRect(QPointF point, QRectF rect);

#endif // SMALLFUNCTIONS_H
