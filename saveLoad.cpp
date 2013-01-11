#include "saveLoad.h"




bool MyWriter::saveTo(const QString &fileName, MyGraphicsScene *scene)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(0,tr("Error saving file"),tr("Can't open file")+ fileName);
        return false;
    }
    setDevice(&file);
    setAutoFormatting(true);
    writeStartDocument();
    writeStartElement("SaveFile");
    writeTextElement("ProgramVersion",number(PROGRAM_VERSION));
    write("BGBrush",scene->backgroundBrush());
    foreach (MyShape * shape, scene->myItemsConst()){
        write("MyShape",shape);
    }
    writeEndElement();
    writeEndDocument();
    file.close();
    return true;
}

void MyWriter::writeNameOnly(const QString &elementName, const MyShape *shape)
{
    writeStartElement(elementName);
    writeStartElement("IsNull");
    if (bool a = (shape == 0)) {
        writeCharacters(bool2String(a));
        writeEndElement();
    }
    else {
        writeCharacters(bool2String(a));
        writeEndElement();
        writeTextElement("Label",shape->getLabel());
        writeTextElement("Description",shape->getDescript());
        writeTextElement("Type",number(shape->getType()));
        writeTextElement("Id",QString::number( (intptr_t) shape ));
    }
    writeEndElement();

}

void MyWriter::writeNameOnly(const QString &elementName,
                             const QList<MyShape *> &list)
{
    writeStartElement(elementName);
    foreach (MyShape * shape, list){
        writeNameOnly("MyShapeRef",shape);
    }
    writeEndElement();
}

void MyWriter::writeNameOnly(const QString &elementName
                             , const QList<MyPoint *> &list)
{
    writeStartElement(elementName);
    foreach (MyPoint * shape, list){
        writeNameOnly("MyPointRef",shape);
    }
    writeEndElement();
}

void MyWriter::writeNameOnly(const QString &elementName
                             , const QList<MyBuilding *> &list)
{
    writeStartElement(elementName);
    foreach (MyBuilding * shape, list){
        writeNameOnly("MyBuildingRef",shape);
    }
    writeEndElement();
}

void MyWriter::write(const QString &elementName, const QColor &color)
{
    writeStartElement(elementName);
    writeCharacters(QString::number(color.rgba(),16));
    writeEndElement();
}


void MyWriter::write(const QString &elementName, const QPen &pen)
{
    writeStartElement(elementName);
    write("Color",pen.color());
    writeTextElement("Style",number(pen.style()));
    writeTextElement("WidthF",number(pen.widthF()));
    writeEndElement();
}

void MyWriter::write(const QString &elementName, const QPointF &point)
{
    writeStartElement(elementName);
    writeTextElement("X",number(point.x()));
    writeTextElement("Y",number(point.y()));
    writeEndElement();
}

void MyWriter::write(const QString &elementName, const QBrush &brush)
{
    writeStartElement(elementName);
    write("Color",brush.color());
    writeTextElement("Style",number(brush.style()));
    writeEndElement();
}

void MyWriter::write(const QString &elementName, const MyShape *shape)
{
    writeStartElement(elementName);
    writeTextElement("Label",shape->getLabel());
    writeTextElement("Description",shape->getDescript());
    writeTextElement("Id",number((intptr_t) shape));
    writeTextElement("Type",number(shape->getType()));
    write("mypen",shape->getPen());
    write("mybrush",shape->getBrush());
    writeTextElement("isImage",bool2String(shape->isImage()));
    writeTextElement("TopLevel",bool2String(shape->isToplevel()));
    writeNameOnly("parentTransform",shape->getParentTransform());
    writeTextElement("Visible",bool2String(shape->isVisible()));
    writeStartElement("SpecificInfo");
    if (shape->isPoint()) {
        MyPoint * point = (MyPoint *) shape;
        write("Pos",point->pos());
        writeTextElement("Moveable",bool2String(point->isMoveable()));
        writeNameOnly("buildings",point->getBuildings().toList());
    }
    else {
        MyBuilding * building = (MyBuilding *) shape;
        writeTextElement("variation",number(building->getVariation()));
        writeTextElement("requiredPillars"
                         ,number(building->getRequiredPillars()));
        switch (building->getType()) {
        case MyShape::Rect:
            {
                MyRect * rect = (MyRect *) building;
                writeNameOnly("mypoints",rect->getPoints());
            }
            break;
        case MyShape::Polygon: {
                MyPolygon * poly = (MyPolygon *) building;
                writeNameOnly("mypoints",poly->getPoints());
                writeTextElement("FillRule",number(poly->getFillRule()) );
            }
            break;
        case MyShape::Segment: {
                MySegment * seg =(MySegment *) building;
                writeNameOnly("point1",seg->getPoint(1));
                writeNameOnly("point2",seg->getPoint(2));
            }
            break;
        case MyShape::Transform:{
                MyTransform * trans = (MyTransform *) building;
                writeTextElement("myangle",number(trans->getMyAngle()));
                writeTextElement("myscale",number(trans->getMyScale()) );
                writeNameOnly("origin",trans->getOrigin());
                writeNameOnly("fromPoints",trans->getFromPoints());
                writeNameOnly("toPoints",trans->getToPoints());
                writeNameOnly("fromBuildings",trans->getFromBuildings());
                writeNameOnly("toBuildings",trans->getToBuildings());
            }
        }
    }
    writeEndElement();
    writeEndElement();
}

bool MyReader::loadFrom(const QString &fileName, MyGraphicsScene *Scene)
{
    mapId.clear();
    QFile file(fileName);    
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,tr("Error loading file"),tr("Can't open ")+ fileName);
        return false;
    }
    try {
        setDevice(&file);
        firstPass();
        setDevice(0);
        file.reset();
        setDevice(&file);
        secondPass();
    }
    catch (ParseError a){
        QMessageBox::critical(0,tr("Error parsing save file"),a.message);
        qDeleteAll(mapId.values().toSet());
        mapId.clear();
        file.close();
        return false;
    }
    Scene->myclear();
    Scene->setMode(MyMode(MyMode::LoadingMode));
    QList<MyShape*> list(mapId.values());
    foreach (MyShape * shape, list) {
        if ( (shape->scene() == 0) && (shape->isPoint()) )
            Scene->addItemMacro(shape);
    }
    foreach (MyShape * shape, list) {
        if ( (shape->scene() == 0) && (!shape->isPoint()) )
            Scene->addItemMacro(shape);
    }
    foreach (MyShape * shape, list) {
        shape->jiggle(10);
    }
    file.close();
    Scene->setMode(MyMode());
    Scene->setBackgroundBrush(bgbrush);
    return true;
}



QColor MyReader::readColor()
{
    QString code = readElementText();
    bool a;
    QColor col = QColor::fromRgba(code.toUInt(&a,16));
    if (!a) corruptData();
    else return col;
}

QPen MyReader::readPen()
{
    QString current(name().toString());
    QStringRef key;
    QColor col;
    Qt::PenStyle style;
    qreal widthf;
    readNext();
    while ( !isEndElement() || (name() != current) ){
        if (isStartElement()){
            key = name();
            if (key == "Color")  { col = readColor(); continue;}
            if (key == "Style"){
                style = (Qt::PenStyle) readInt();
                continue;
            }
            if (key == "WidthF")  {
                widthf = readReal();
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    QPen pen(col); pen.setStyle(style); pen.setWidthF(widthf);
    return pen;
}

QBrush MyReader::readBrush()
{
    QString current(name().toString());
    QStringRef key;
    QColor col;
    Qt::BrushStyle style;
    readNext();
    while ( !isEndElement() || (name() != current) ) {
        if (isStartElement()){
            key = name();
            if (key == "Color") { col = readColor(); continue; }
            if (key=="Style"){
                style = (Qt::BrushStyle) readInt();
                continue;
            }
        }
        readNext();
    }
    return QBrush(col,style);
}

QPointF MyReader::readPointF()
{
    QString current(name().toString());
    QStringRef key;
    qreal x=0; qreal y=0;
    readNext();
    while ( !isEndElement() || (name() != current) ){
        if (isStartElement()){
            key=name();
            if (key == "X") {
                x =readReal();
                continue;
            }
            if (key == "Y"){
                y=readReal();
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    return QPointF(x,y);
}

qreal MyReader::readReal()
{
    bool a;
    qreal kq = readElementText().toDouble(&a);
    if (!a) corruptData();
    else return kq;
}

int MyReader::readInt(int base)
{
    bool a;
    int kq = readElementText().toInt(&a,base);
    if (!a) corruptData();
    else return kq;
}

qlonglong MyReader::readLongLong(int base)
{
    bool a;
    qlonglong kq = readElementText().toLongLong(&a,base);
    if (!a) corruptData();
    else return kq;
}

bool MyReader::readBool()
{
    QString string = readElementText().toLower();
    if (string == "true") return true;
    if (string == "false") return false;
    corruptData();
}







MyShape *MyReader::readShapeRef(MyShape::KindOfShape kind)
{
    QString current(name().toString());
    QStringRef key;
    qlonglong id=0;
    readNext();
    while ( !isEndElement() || (name() != current) ){
        if (isStartElement()) {
            key=name();
            if (key=="IsNull") {
                bool a= readBool();
                if (a) return 0;
                continue;
            }
            if (key=="Id") {
                id = readLongLong();
                continue;
            }
            if (key=="Type") {
                int type = readInt();
                if (kind == MyShape::Shape) continue;
                if (type != kind) throw ParseError(tr("Invalid ShapeRef.") );
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    MyShape * shape = mapId.value(id);
    check(shape);
    return shape;
}

QList<MyShape *> MyReader::readShapeList()
{
    QList<MyShape*> list;
    QString current(name().toString());
    QStringRef key;
    MyShape * shape = 0;
    readNext();
    while ( !isEndElement() || (name() != current) ){
        if (isStartElement()) {
            key=name();
            if (key=="MyShapeRef") {
                shape = readShapeRef();
                if (shape) list << shape;
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    return list;
}

QList<MyBuilding *> MyReader::readBuildingList()
{
    QList<MyBuilding*> list;
    QString current(name().toString());
    QStringRef key;
    MyBuilding * shape = 0;
    readNext();
    while ( !isEndElement() || (name() != current) ){
        if (isStartElement()){
            key=name();
            if (key=="MyBuildingRef") {
                shape = (MyBuilding *) readShapeRef();
                if (shape) list << shape;
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    return list;
}

QList<MyPoint *> MyReader::readPointList()
{
    QList<MyPoint*> list;
    QString current(name().toString());
    QStringRef key;
    MyPoint * shape = 0;
    readNext();
    while ( !isEndElement() || (name() != current) ){
        if (isStartElement()){
            key=name();
            if (key=="MyPointRef") {
                shape = (MyPoint *) readShapeRef(MyShape::Point);
                if (shape) list << shape;
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    return list;
}

void MyReader::firstPass()
{
    QStringRef key;
    bool legitFile = false;
    while ((!atEnd()) && (!isStartElement() || (name() != "SaveFile" )))
        readNext();
    readNext();
    while (!atEnd())  {
        if (isStartElement()){
            key=name();
            if (key=="MyShape") {
                readShapeFirstPass();
                continue;
            }
            if (key == "ProgramVersion") {
                int ver = readInt();
                if (ver != PROGRAM_VERSION)
                    throw ParseError( tr("Wrong program version."));
                legitFile = true;
                continue;
            }
            if (key=="BGBrush") {
                bgbrush = readBrush();
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    if (!legitFile) throw ParseError(tr("Not a legitimate save file."));
}

void MyReader::secondPass()
{
    QStringRef key;
    while (!isStartElement() || (name() != "SaveFile" )) readNext();
    readNext();
    while ( !atEnd() ) {
        if (isStartElement()) {
            key=name();
            if (key=="MyShape") {
                readShapeSecondPass();
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
}


MyShape *MyReader::readShapeFirstPass()
{
    QString current(name().toString());
    QStringRef key;
    MyShape::KindOfShape kind;
    MyShape * shape = 0;
    qlonglong id =0;
    QString label;
    readNext();
    while ( !isEndElement() || (name() != current) ){
        if (isStartElement()) {
            key=name();
            if (key == "Type") {
                kind = (MyShape::KindOfShape) readInt();
                switch (kind)  {
                case MyShape::Point:
                    shape = new MyPoint; break;
                case MyShape::Rect:
                    shape = new MyRect; break;
                case MyShape::Polygon:
                    shape = new MyPolygon; break;
                case MyShape::Segment:
                    shape = new MySegment; break;
                case MyShape::Transform:
                    shape = new MyTransform; break;
                default:
                    throw ParseError(tr("Unrecognized kind of shape.") );
                }
                continue;
            }
            if (key == "Id") {
                id = readLongLong();
                continue;
            }
            if (key=="Label") {
                label = readElementText();
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    if ( (shape ==0) || (id == 0)) return 0;
    mapId[id]=shape;
    shape->setLabel(label);
    return shape;
}

MyShape *MyReader::readShapeSecondPass()
{
    QString current(name().toString());
    QStringRef key;
    readNext();
    MyShape * shape = 0;
    while ( !isEndElement() || (name() != current) ){
        if (isStartElement()){
            key=name();
            if (key == "Id") {
                qlonglong id = readLongLong();
                shape = mapId.value(id);
                check(shape);
                continue;
            }
            if (key == "Type") {
                check(shape);
                if (shape->getType() != readInt())
                    throw ParseError( tr("Type and Id mismatch in reference." ));
                continue;
            }
            if (key == "mypen") {
                check(shape);
                shape->setPen(readPen());
                continue;
            }
            if (key == "mybrush") {
                check(shape);
                shape->setBrush(readBrush());
                continue;
            }
            if (key == "isImage") {
                check(shape);
                shape->setIsImage(readBool());
                continue;
            }
            if (key == "Visible") {
                check(shape);
                shape->setVisible(readBool());
                continue;
            }
            if (key == "TopLevel") {
                check(shape);
                shape->setToplevel(readBool());
                continue;
            }
            if (key== "parentTransform") {
                check(shape);
                shape->setParentTransform(
                            (MyTransform *)readShapeRef(MyShape::Transform));
                continue;
            }
            if (key == "SpecificInfo") {
                check(shape);
                readSpecificInfo(shape);
                continue;
            }
            skipCurrentElement();
        }
        readNext();
    }
    return shape;
}

void MyReader::readSpecificInfo(MyShape *shape)
{
    QString current(name().toString());
    QStringRef key;
    readNext();
    if (shape->isPoint()) {
        MyPoint * point = (MyPoint *) shape;
        bool moveable;
        while ( !isEndElement() || (name() != current) ) {
            if (isStartElement()) {
                key = name();
                if (key == "Pos") {
                    point->setPos(readPointF());
                    continue;
                }
                if (key == "Moveable"){
                    moveable = readBool();
                    continue;
                }
                skipCurrentElement();
            }
            readNext();
        }
        check(point);
        point->neoinit(moveable);
    }
    else
    {
        int variation = 1; int requiredPillars =0 ;
        switch (shape->getType()) {
        case MyShape::Rect:
            {
                MyRect * rect = (MyRect *) shape;
                check(rect);
                while ( !isEndElement() || (name() != current) ) {
                    if (isStartElement()) {
                        key = name();
                        if (key == "variation") {
                            variation= readInt();
                            continue;
                        }
                        if (key == "requiredPillars"){
                            requiredPillars = readInt();
                            continue;
                        }
                        if (key == "mypoints") {
                            rect->neoinit(readPointList());
                            continue;
                        }
                        skipCurrentElement();
                    }
                    readNext();
                }

            }
            break;
        case MyShape::Polygon:
            {
                MyPolygon * poly =( MyPolygon *) shape;
                QList<MyPoint*> list;
                Qt::FillRule rule;
                while ( !isEndElement() || (name() != current) ) {
                    if (isStartElement()) {
                        key = name();
                        if (key == "variation") {
                            variation= readInt();
                            continue;
                        }
                        if (key == "requiredPillars"){
                            requiredPillars = readInt();
                            continue;
                        }
                        if (key == "mypoints") {
                            list = readPointList();
                            continue;
                        }
                        if (key == "FillRule") {
                            rule = (Qt::FillRule) readInt();
                            continue;
                        }
                        skipCurrentElement();
                    }
                    readNext();
                }
                poly->neoinit(list,rule);
            }
            break;
        case MyShape::Segment:
            {
                MySegment * seg = (MySegment * ) shape;
                MyPoint* pointa = 0;
                MyPoint* pointb=0;
                while ( !isEndElement() || (name() != current) ){
                    if (isStartElement()) {
                        key = name();
                        if (key == "variation") {
                            variation= readInt();
                            continue;
                        }
                        if (key == "requiredPillars") {
                            requiredPillars = readInt();
                            continue;
                        }
                        if (key == "point1") {
                            pointa = (MyPoint*) readShapeRef(MyShape::Point);
                            continue;
                        }
                        if (key == "point2") {
                            pointb = (MyPoint*) readShapeRef(MyShape::Point);
                            continue;
                        }

                        skipCurrentElement();
                    }
                    readNext();
                }
                check(pointa);
                check(pointb);
                seg->neoinit(pointa,pointb);
            }
            break;
        case MyShape::Transform:
            {
                MyTransform* trans = (MyTransform *) shape;
                MyPoint * origin =0;
                QList<MyPoint *> fromPoints;
                QList<MyPoint *> toPoints;
                QList<MyBuilding *> fromBuildings;
                QList<MyBuilding *> toBuildings;
                qreal myangle = 0; qreal myscale = 0;
                while ( !isEndElement() || (name() != current) ) {
                    if (isStartElement()) {
                        key = name();
                        if (key == "variation") {
                            variation= readInt();
                            continue;
                        }
                        if (key == "requiredPillars"){
                            requiredPillars = readInt();
                            continue;
                        }
                        if (key == "myscale") {
                            myscale = readReal();
                            continue;
                        }
                        if (key == "myangle") {
                            myangle = readReal();
                            continue;
                        }
                        if (key == "origin"){
                            origin = (MyPoint*) readShapeRef(MyShape::Point);
                            continue;
                        }
                        if (key == "fromPoints"){
                            fromPoints = readPointList();
                            continue;
                        }
                        if (key == "toPoints"){
                            toPoints = readPointList();
                            continue;
                        }
                        if (key == "fromBuildings"){
                            fromBuildings = readBuildingList();
                            continue;
                        }
                        if (key == "toBuildings"){
                            toBuildings = readBuildingList();
                            continue;
                        }
                        skipCurrentElement();
                    }
                    readNext();
                }
                check( (fromBuildings.size() == toBuildings.size() )
                       && (fromPoints.size() == toPoints.size())
                       && (origin!=0));
                trans->neoinit(origin,fromPoints,toPoints
                               ,fromBuildings,toBuildings
                               ,myangle,myscale);
            }
            break;
        default:
            throw ParseError(tr("Unrecognized kind of shape."));
        }
        MyBuilding * build = (MyBuilding *) shape;
        build->neoinitBuilding(variation,requiredPillars);
    }
}
