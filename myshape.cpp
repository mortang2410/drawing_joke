#include "myshape.h"
#include "mymainwindow.h"
#include "renderarea.h"

void MyShape::preinit()
{
    inited = false;
    isImage_ = false;
    parentTransform = 0;
    setCursor(QCursor(Qt::BusyCursor));
    setFlag(ItemIsSelectable);
    setPen(QPen(Qt::blue));
    setBrush(QBrush(QColor(199, 210, 219,50)));
    mypenSelected.setStyle(Qt::SolidLine);
    mypenSelected.setColor(QColor(0,0,255,120));
    topLevel = true;
}


void MyShape::penAndBrush(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (isSelected()) {
        mypenSelected.setWidthF(6*fixedSize());
        painter->setPen(mypenSelected);
        painter->setBrush(mybrushSelected);
        painter->drawPath(shape());
    }
    painter->setPen(mypen);
    painter->setBrush(mybrush);
}

MyShape * MyShape::myParent() const
{
    if (isPoint()) {
        MyPoint * point = (MyPoint *) this;
        if (point->parentBuilding)
            return point->parentBuilding;
    }
    else {
        MyBuilding * building((MyBuilding *)this);
        if (building->parentTransform)
            return building->parentTransform;
    }
    return 0;
}

int MyShape::rankInParent() const
{
    if (hasParent()) {
        return myParent()->myChildren().indexOf(
                    const_cast<MyShape *>(this));
    }
    else {
        if (!scene()) return -1;
        MyGraphicsScene * Scene = (MyGraphicsScene *) scene();        
        return Scene->topLevelItemsConst().indexOf(
                    const_cast<MyShape *>(this));
    }
}

void MyShape::neoinitShape(bool IsImage, MyTransform *ParentTransform)
{
    isImage_ = IsImage;
    parentTransform = ParentTransform;
}


void MyPoint::preinit(){
    setFlag(ItemSendsScenePositionChanges);
    parentBuilding = 0;
}

void MyPoint::postinit(QPointF point, bool Moveable)
{
    setPos(point);
    moveAble = Moveable;
    inited = true;
}

void MyPoint::neoinit(bool Moveable)
{
    moveAble = Moveable;
    inited =true;
}

MyPoint::MyPoint()
{
    preinit();
}

MyPoint::MyPoint(qreal x, qreal y, bool MoveAble)
{
    preinit();
    postinit(QPointF(x,y),MoveAble);
}
MyPoint::MyPoint(QPointF point, bool MoveAble)
{
    preinit();
    postinit(point,MoveAble);
}


int MyShape::getTheScale() const{
    MyGraphicsScene * currentScene = (MyGraphicsScene *) this->scene();
    if (currentScene == 0) return 100;
    return currentScene->getSceneScale() * (this->scale());
}

QVariant MyShape::itemChange(QGraphicsItem::GraphicsItemChange change
                             , const QVariant &value)
{
    //only moveable pillars move
    if ( change == QGraphicsItem::ItemScenePositionHasChanged)
    {
        if (MyPoint * point = dynamic_cast<MyPoint *>(this)) {
            foreach (MyBuilding * building, point->buildings){
                if (building->isImage_==false)
                building->prepareGeometryChange();

            }
        }
    }
    if (change==ItemSelectedHasChanged){
        if (MyBuilding * building = dynamic_cast<MyBuilding *>(this)) {
            bool state = value.toBool();
            if (state) foreach (MyPoint * pillar, building->pillars){
                pillar->setSelected(true);
            }
        }
    }
    return QGraphicsItem::itemChange(change,value);
}

void MyShape::myMoveBy(QPointF dpoint)
{
    if (isPoint()) {
        MyPoint * point = (MyPoint *) this;
        if (point->isMoveable())
            point->setPos(point->pos() + dpoint);
        for (int i=1;i<3;i++){
            //ugliest hack in history
            //Feel free to delete this loop
            //and watch as MyTransform gets wrecked.
            //Maybe a redesign of MyTransform is needed....
            point->moveBy(1,1);
            point->moveBy(-1,-1);
        }
    }    
}

void MyShape::jiggle(int n)
{
    //lol
    if (isPoint()) {
        MyPoint * point = (MyPoint *) this;
        if (!point->isMoveable()) return;
        for (int i=1;i<n;i++){
            point->moveBy(1,1);
            point->moveBy(-1,-1);
        }
    }
}

QPainterPath MyShape::fattenPath(const QPainterPath &path, int padding) const
{
    qreal half = padding / (getTheScale()/100.0);
    QPainterPathStroker stroker;
    stroker.setWidth(half);
    return stroker.createStroke(path);
}


QRectF MyPoint::boundingRect() const{    
    qreal half = (PointSize/2.0) / (getTheScale()/100.0);
    return QRectF(-half,-half,2*half,2*half)
            .normalized();    
}

void MyPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget){

    qreal point=100.0/ getTheScale();
    QBrush thebrush(mybrush);
    if (isMoveable()) {
        thebrush.setColor(QColor(52, 45, 164));
    }
    else{
        thebrush.setColor(Qt::black);
    }
    if (isSelected()){
        QPen thepen(mypen);
        thepen.setWidthF(mypen.widthF()+2*point);
        thepen.setColor(QColor(143, 224, 218));
        painter->setPen(thepen);
        painter->setBrush(thebrush);
    }
    else  {
        painter->setPen(mypen);
        painter->setBrush(thebrush);
    }
    painter->drawEllipse(boundingRect());
}


QString MyRect::getDescript() const{
    switch (variation){
    case NormalRect:
        return tr("Rectangle");
    case Ellipse:
        return tr("Ellipse");
    case Circle:
        return tr("Circle");
    }
    return QString();
}

MyRect::MyRect()
{
    preinit();
}

void MyRect::preinit()
{
    setPos(0,0);
}

MyRect::MyRect(MyPoint * point1, MyPoint * point2, bool IsImage, int Variation)
{    
    preinit();
    postinit(point1,point2,IsImage,Variation);
}
void MyRect::postinit(MyPoint *point1
                      , MyPoint *point2, bool IsImage, int Variation)
{
    variation = Variation;
    isImage_ = IsImage;
    mypoints << point1 << point2;
    if ( (variation==NormalRect) || (variation==Ellipse)) {
        QPointF pt0(mypoints[0]->scenePos());
        QPointF pt1(mypoints[1]->scenePos());
        mypoints << new MyPoint(pt0.x(),pt1.y(),false)
                 << new MyPoint(pt1.x(),pt0.y(),false);
        mypoints[2]->setToplevel(false); mypoints[3]->setToplevel(false);
    }
    requiredPillars = mypoints.size();
    inited = true;
}

void MyRect::neoinit(const QList<MyPoint *> &MyPoints)
{
    mypoints = MyPoints;
    inited = true;
}

MyRect::MyRect(const QList<MyPoint *> &buildList, bool IsImage, int Variation)
{
    preinit();
    postinit(buildList[0],buildList[1],IsImage,Variation);
}

QRectF MyRect::boundingRect() const {
     qreal tlw = getPen().widthF()/2;
     QPointF corner0;
     QPointF corner1;
     QPointF a(mypoints[0]->scenePos());
     QPointF b(mypoints[1]->scenePos());
     if (isImage_){
         a = mapFromScene(a);
         b = mapFromScene(b);
     }
     if ( (variation==NormalRect) || (variation==Ellipse)) {      
         corner0 = a;
         corner1 = b;
     }
     else  {         
         qreal r = normalize(b-a);
         corner0 = a-QPointF(r,r);
         corner1 = a+QPointF(r,r);
     }
     return QRectF(corner0,corner1)
             .adjusted(-tlw,-tlw,tlw,tlw).normalized();
}

void MyRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    penAndBrush(painter,option,widget);
    if ( (variation==Circle) || (variation==Ellipse)) {
        painter->drawEllipse(boundingRect());
    }
    else painter->drawRect(boundingRect());
}

QPainterPath MyRect::shape() const
{
    QPainterPath path;
    if ( (variation==Circle) || (variation==Ellipse)) {
        path.addEllipse(boundingRect());
    }
    else path.addRect(boundingRect());
    return path;
}

QPainterPath MyRect::circle(const QPointF &center, const QPointF &pointa)
{
    qreal r = normalize(center-pointa);
    QPainterPath path; path.addEllipse(center,r,r);
    return path;
}


void MyRect::prepareGeometryChange()
{
    if ( (variation==NormalRect) || (variation==Ellipse)) {
        if (!isImage_){
            QPointF pt0(mypoints[0]->scenePos());
            QPointF pt1(mypoints[1]->scenePos());
            mypoints[2]->setPos(pt0.x(),pt1.y());
            mypoints[3]->setPos(pt1.x(),pt0.y());
        }
    }
    QGraphicsItem::prepareGeometryChange();
}



void MyPolygon::preinit()
{
    myfillrule=Qt::OddEvenFill;
}

void MyPolygon::postinit(const QList<MyPoint *> &thepoints)
{
    mypoints=thepoints;
    requiredPillars = mypoints.size();
}

void MyPolygon::neoinit(const QList<MyPoint *> &thepoints, Qt::FillRule FillRule)
{
    mypoints=thepoints;
    myfillrule = FillRule;
    inited = true;
}

MyPolygon::MyPolygon()
{
    preinit();
}

MyPolygon::MyPolygon(const QList<MyPoint *> &thepoints)
{
    preinit();
    postinit(thepoints);
}

QRectF MyPolygon::boundingRect() const{        
    qreal tlw = getPen().widthF()/2;
    return getPolygon().boundingRect().adjusted(-tlw,-tlw,tlw,tlw);
}

QPainterPath MyPolygon::shape() const{    
    QPainterPath path;
    path.addPolygon(getPolygon());
    path.closeSubpath();
    return path;
}
void MyPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    penAndBrush(painter,option,widget);
    painter->drawPolygon(getPolygon(),myfillrule);
}

QPolygonF MyPolygon::getPolygon() const
{
    QPolygonF poly;
    foreach (MyPoint * point, mypoints) poly << point->scenePos();
    return poly;
}

QString MySegment::getDescript() const
{
    switch (variation){
    case NormalSegment:
        return tr("Segment");
    case Line:
        return tr("Line");
    case Ray:
        return tr("Ray");
    }
    return QString();
}

void MySegment::preinit()
{
    point1=0;
    point2=0;
}

void MySegment::postinit(MyPoint *pointa, MyPoint *pointb, int Variation)
{
    variation = Variation;
    point1 = pointa;
    point2 = pointb;
    requiredPillars = 2;
    inited = true;
}

void MySegment::neoinit(MyPoint *pointa, MyPoint *pointb)
{
    point1 = pointa;
    point2 = pointb;
    inited = true;
}

MySegment::MySegment()
{
    preinit();
}

MySegment::MySegment(MyPoint * pointa, MyPoint * pointb,
                     int Variation)
{
    preinit();
    postinit(pointa,pointb,Variation);
}



QRectF MySegment::boundingRect() const
{
    qreal tlw = getPen().widthF()/2;
    switch (variation){
    case NormalSegment:
        {
            return  QRectF(point1->scenePos(),point2->scenePos())
                    .adjusted(-tlw,-tlw,tlw,tlw);
        }


    case Line:
        {
            Renderarea * theview = (Renderarea *) scene()->views().first();
            if (theview) return theview->visibleRect();
            else return QRectF();
        }
    case Ray:
        {
            Renderarea * theview = (Renderarea *) scene()->views().first();
            if (theview) return infiniteRay(point1->scenePos(),
                                            point2->scenePos(),
                                            theview->visibleRect())
                                .boundingRect();
            else return QRectF();
        }
    }
    return QRectF();
}

QPainterPath MySegment::preshape() const
{
    switch (variation) {
    case NormalSegment:
        {
            QPainterPath path;
            path.moveTo(point1->scenePos());
            path.lineTo(point2->scenePos());
            return path;
        }
        break;
    case Line:
        return infiniteLine(point1->scenePos(),point2->scenePos()
                                      ,boundingRect());
        break;
    case Ray:
        return infiniteRay(point1->scenePos(),point2->scenePos()
                           ,boundingRect());
    }
    return QPainterPath();
}

QPainterPath MySegment::shape() const
{     
    return fattenPath(preshape(),20*fixedSize());
}

void MySegment::penAndBrush(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    if (isSelected()){
        mypenSelected.setWidthF(6*fixedSize());
        painter->setPen(mypenSelected);
        painter->setBrush(mybrushSelected);
        painter->drawPath(preshape());
    }
    painter->setPen(mypen);
    painter->setBrush(mybrush);

}


QPainterPath MySegment::infiniteLine(const QPointF &pointa, const QPointF &pointb, const QRectF &rect)
{
    QLineF line(pointa,pointb);
    QPointF pt1;  QLineF border1;
    QPointF pt2;  QLineF border2;
    if ( mymod(line.angle() - 45,180) < 90) {
        //meets the upper bar
       border1.setPoints(rect.topLeft(),rect.topRight());
       border2.setPoints(rect.bottomLeft(),rect.bottomRight());
    }
    else {
        border1.setPoints(rect.topLeft(),rect.bottomLeft());
        border2.setPoints(rect.topRight(),rect.bottomRight());
    }
    line.intersect(border1,&pt1);
    line.intersect(border2,&pt2);
    QPainterPath path;
    path.moveTo(pt1);
    path.lineTo(pt2);
    return path;
}

QPainterPath MySegment::infiniteRay(const QPointF &pointa, const QPointF &pointb, const QRectF &rect)
{
    QLineF line(pointa,pointb);
    QPointF pt1;  QLineF border1;
    QPointF pt2;  QLineF border2;
    if ( mymod(line.angle() - 45,180) < 90) {
        //meets the upper bar
       border1.setPoints(rect.topLeft(),rect.topRight());
       border2.setPoints(rect.bottomLeft(),rect.bottomRight());
    }
    else {
        border1.setPoints(rect.topLeft(),rect.bottomLeft());
        border2.setPoints(rect.topRight(),rect.bottomRight());
    }
    line.intersect(border1,&pt1);
    line.intersect(border2,&pt2);
    QPointF rayStart; QPointF rayEnd; QPainterPath path;
    bool P1( dotProduct(pt1-pointa,pointb-pointa) >=0 );
    bool P2( dotProduct(pt2-pointa,pointb-pointa) >=0 );
    if ( P1 && P2) { rayStart = pt1; rayEnd = pt2;}
    if (P1 && !P2) {rayStart = pointa; rayEnd = pt1;}
    if (!P1 && P2) {rayStart = pointa; rayEnd = pt2;}
    if (!P1 && !P2) {return path;}
    path.moveTo(rayStart); path.lineTo(rayEnd);
    return path;
}



void MySegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{    
    penAndBrush(painter,option,widget);    
    painter->setBrush(Qt::SolidPattern);
    painter->drawPath(preshape());
}

const MyPoint * MySegment::getPoint(int i) const
{
    if (i==1) return point1;
    if (i==2) return point2;
    return 0;
}



void MyTransform::preinit()
{
    myangle = 0; myscale = 1; origin = 0;
}

void MyTransform::postinit(MyPoint *originPoint, const QList<MyShape *> &source, qreal Angle, qreal Scale)
{
    myangle = Angle; myscale= Scale; origin = originPoint;
    QList<MyShape *> updatedSource = source;
    foreach (MyShape * item, source){
        if (!item->isPoint()) {
            MyBuilding * thesource = (MyBuilding *)item;
            if (thesource->getType() == Transform) {
                updatedSource.removeAll(thesource);
                break;
            }
            foreach (MyPoint * pillar, thesource->pillars){
                if (!updatedSource.contains(pillar))
                    updatedSource << (MyShape *)(pillar);
            }
        }
    }

    requiredPillars= 1;

    foreach (MyShape * item, updatedSource){
        if (item->isPoint()) {
            requiredPillars +=2;
            MyPoint * theitem = (MyPoint *)item;
            MyPoint * newpoint( new MyPoint(theitem->scenePos()
                                           ,false));
            newpoint->setIsImage(true);
            newpoint->setToplevel(false);
            newpoint->setParentTransform(this);
            fromPoints << theitem; toPoints << newpoint;
        }
    }
    for (int i=0; i<updatedSource.size();i++){
        MyShape * item(updatedSource[i]);
        if (item->isPoint() == false ){
            MyBuilding * thesource = (MyBuilding *) item;
            QList<MyPoint *> newPillars;
            foreach (MyPoint * oldPillar, thesource->pillars){
                newPillars << toPoints[fromPoints.indexOf(oldPillar)];
            }
            fromBuildings << thesource;
            MyBuilding * theimage =  (MyBuilding *)(Builder::build(
                                           newPillars
                                           ,thesource->getType()
                                           ,true
                                           ,thesource->getVariation()));
            toBuildings << theimage;
            theimage->setParentTransform(this);
            theimage->setToplevel(false);
            Q_ASSERT(toBuildings.last() != 0);
        }
    }
    foreach (MyPoint * point, toPoints) point->setPos(0,0);
    foreach (MyBuilding * building,toBuildings) building->setPos(0,0);
    inited = true;
}

void MyTransform::neoinit(MyPoint *originPoint
                          , const QList<MyPoint *> &FromPoints
                          , const QList<MyPoint *> &ToPoints
                          , const QList<MyBuilding *> &FromBuildings
                          , const QList<MyBuilding *> &ToBuildings
                          , qreal Angle, qreal Scale)
{
    myangle = Angle; myscale= Scale; origin = originPoint;
    fromPoints = FromPoints;
    toPoints = ToPoints;
    fromBuildings = FromBuildings;
    toBuildings = ToBuildings;
    inited = true;
}

MyTransform::MyTransform(){
    preinit();
}

MyTransform::MyTransform(MyPoint * originPoint, const QList<MyShape *> &source,
                         qreal Angle, qreal Scale)    
{
    preinit();
    postinit(originPoint,source,Angle,Scale);
}


void MyTransform::prepareGeometryChange()
{

    for (int i =0;i<fromPoints.size();++i){
        MyPoint * point = toPoints[i];
        QPointF Ori(point->mapFromScene(origin->scenePos()));

        point->setTransformOriginPoint(Ori);
        point->setRotation(myangle);
        point->setScale(myscale);

    }
    foreach (MyBuilding * building, toBuildings){
        if (building->getType()==Rect){
            QPointF Ori(building->mapFromScene(origin->scenePos()));
            building->setTransformOriginPoint(Ori);
            building->setRotation(myangle);
            building->setScale(myscale);
        }
//        building->prepareGeometryChange();
    }
    QGraphicsItem::prepareGeometryChange();
}

QList<MyShape *> MyTransform::myChildren() const
{
    QList<MyShape *> list(MyBuilding::myChildren());
    foreach (MyBuilding * subBuilding,toBuildings )
        list << subBuilding;
    return list;
}

QList<MyShape *> MyBuilding::myChildren() const
{
    QList<MyShape *> list;
    foreach (MyPoint * pillar, spawnedPillars)
        list << pillar;
    return list;
}

void MyBuilding::neoinitBuilding(int Variation, int RequiredPillars)
{
    variation = Variation;
    requiredPillars = RequiredPillars;
}


MyBuilding * Builder::build(const QList<MyPoint *> &buildList,
                           MyShape::KindOfShape kind,
                           bool creatingImage,
                           int Variation)
{
    switch (kind){
    case MyShape::Rect:
        {
            int a;
            if (Variation == MyRect::Circle) a=2;
            else a = 4;
            if (buildList.size()!=a) return 0;
            MyRect * rect(new MyRect);
            rect->neoinitShape(creatingImage);
            rect->neoinitBuilding(Variation,a);
            rect->neoinit(buildList);
            return rect;
        }
        break;

    case  MyShape::Polygon:
        {
            if (buildList.size() ==0) return 0;
            return new MyPolygon(buildList);
        }
        break;
    case MyShape::Segment:
        {
            if (buildList.size() != 2) return 0;
            return new MySegment(buildList[0],buildList[1],Variation);
        }
        break;
    default:
        return 0;
    }
}


