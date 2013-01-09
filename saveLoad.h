#ifndef SAVELOAD_H
#define SAVELOAD_H
#include "qtmodules.h"
#include "renderarea.h"


class ParseError {
public:
    ParseError(QString s)
        {message = s;}
    QString message;
};

class MyWriter : public QXmlStreamWriter
{

public:
    bool saveTo(const QString & fileName, MyGraphicsScene * scene);

protected:
    void writeNameOnly(const QString & elementName,const MyShape * shape);
    void writeNameOnly(const QString & elementName
                       , const QList<MyShape *> & list) ;
    void writeNameOnly(const QString & elementName
                       ,const QList<MyPoint *> & list);
    void writeNameOnly(const QString & elementName
                       ,const QList<MyBuilding *> & list) ;
    void write(const QString & elementName ,const QColor & color) ;
    void write(const QString & elementName ,const QPen & pen) ;
    void write(const QString & elementName ,const QPointF & point) ;
    void write(const QString & elementName ,const QBrush & brush);
    void write(const QString &elementName, const MyShape * shape) ;
};

class MyReader : public QXmlStreamReader
{

public:

    bool loadFrom(const QString & fileName, MyGraphicsScene * Scene);
protected:
    void firstPass();
    void secondPass();
    void check(bool x)
        {if (!x) throw ParseError(tr("Failed boolean check."));}
    void check(void * s)
        {if (s==0) throw ParseError( tr("Null pointer encountered.") );}
    void corruptData()
        {throw ParseError( tr("Corrupt data in save file."));}
    QColor readColor();
    QPen readPen();
    QBrush readBrush();
    QPointF readPointF();
    qreal readReal();
    int readInt(int base = 10);
    qlonglong readLongLong(int base = 10);
    bool readBool();

    MyShape * readShapeFirstPass();
    MyShape * readShapeSecondPass();
    void readSpecificInfo(MyShape * shape);
    MyShape * readShapeRef(MyShape::KindOfShape kind= MyShape::Shape);
    QList<MyShape *> readShapeList();
    QList<MyBuilding *> readBuildingList();
    QList<MyPoint *> readPointList();

    QMap<qlonglong,MyShape *> mapId;
    QBrush bgbrush;
};





#endif // SAVELOAD_H
