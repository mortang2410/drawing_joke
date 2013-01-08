#ifndef MYSHAPE_H
#define MYSHAPE_H
#include "qtmodules.h"
#include "misc.h"
class MyGraphicsScene;
class MyShape;
class MyPoint;
class MyBuilding;
class MyPolygon;
class MySegment;
class MyRect;
class MyTransform;


class MyShape: public QGraphicsItem
{
public:
    friend class MyGraphicsScene;
    enum KindOfShape {Shape=UserType+1, Point, Building, Rect,
                      Polygon, Segment, Transform,Nothing};                

    virtual KindOfShape getType() const = 0;
    virtual QString getDescript() const = 0;
    bool isPoint() const {return (getType() == Point); }
    void preinit();
    MyShape() { preinit();}
    virtual QRectF boundingRect() const = 0;
    virtual ~MyShape(){ qDebug() << label << "is dying";}

     int getTheScale() const;
     qreal fixedSize() const {return (100.0/getTheScale());}
     MyGraphicsScene * theScene() {
         return  (MyGraphicsScene *) scene();
     }     
     //getters and setters for convenient purposes
     const QPen & getPen() const{return mypen; }
     const QBrush & getBrush() const{return mybrush;}
     void setPen( const QPen & pen) { mypen = pen; mypen.setCosmetic(true);}
     void setBrush(const QBrush & brush)
        { mybrush = brush;}
     virtual void prepareGeometryChange(){
         QGraphicsItem::prepareGeometryChange();
     }
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);

     void myMoveBy(QPointF dpoint);
     void jiggle(int n = 5);

//     void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//     void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      QPainterPath fattenPath(const QPainterPath & path, int padding=0) const;
     virtual void penAndBrush(QPainter *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget *widget);     
     virtual QList<MyShape *> myChildren() const
            {return QList<MyShape *>();}
     virtual MyShape * myParent() const;
     virtual bool hasParent() const { return (myParent() != 0);}
     virtual int rankInParent() const;

     const QString & getLabel() const {return label;}
     void setLabel(QString newlabel) {label = newlabel;}
     bool isImage() const {return isImage_;}
     void setIsImage(bool newval) {isImage_ = newval;}
     bool isToplevel() const {return topLevel;}
     void setToplevel(bool state) {topLevel = state;}
     const MyTransform * getParentTransform() const {return parentTransform;}
     void setParentTransform(MyTransform * trans)
            {parentTransform = trans;}
     void neoinitShape(bool IsImage,MyTransform *  ParentTransform=0);
protected:
     //the pen and brush for when it's selected
     //used to draw the "selection rectangle"
     //The pen should be cosmetic
     QPen mypenSelected;
     QBrush mybrushSelected;
     //the pen and brush
     QPen mypen;
     QBrush mybrush;     
     QString label;
     bool isImage_;
     MyTransform *  parentTransform;
     bool topLevel;
     bool inited;
};

class MyPoint : public MyShape
{
public:    
    friend class MyShape;
    friend class MyGraphicsScene;
    friend class MyTransform;
    friend class Builder;
    KindOfShape getType() const {return Point;}
    QString getDescript() const {return QObject::tr("Point");}
    enum { PointSize=14};    
    bool isMoveable() {return moveAble; }
    void setMoveable(bool state) { moveAble = state;}
    void preinit();
    void postinit(QPointF point,bool Moveable=true);
    void neoinit(bool Moveable);
    MyPoint();
    MyPoint(qreal x, qreal y,bool MoveAble= true);
    MyPoint(QPointF point,bool MoveAble=true);
    ~MyPoint() { qDebug() << "a point dying";}

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option
                ,QWidget *widget=0);
//     QVariant itemChange(GraphicsItemChange change, const QVariant &value);    
    const QSet<MyBuilding *> & getBuildings() const {
        return buildings;
    }
    bool canStand() const
        {return (scene());}
    const MyBuilding * getParentBuilding() const
        {return parentBuilding;}
private:
    bool moveAble;
    QSet<MyBuilding *> buildings;
    MyBuilding * parentBuilding;
};

class MyBuilding : public MyShape
{
public:
    friend class MyShape;
    friend class MyGraphicsScene;
    friend class Builder;    
    friend class MyTransform;
    MyBuilding()
        :variation(1),requiredPillars(0) {}
    virtual KindOfShape getType() const {return Building;}    
    bool canStand() const
        {return ((scene()) && (pillars.size() == requiredPillars));}
    int getVariation() const{ return variation;}
    QList<MyShape *> myChildren() const;
    const QList<MyPoint *> & getPillars() const {return pillars;}
    const QList<MyPoint *> & getSpawned() const {return spawnedPillars;}
    void neoinitBuilding(int Variation, int RequiredPillars);
    int getRequiredPillars() const {return requiredPillars;}
protected:
    QList<MyPoint *> pillars;
    QList<MyPoint *> spawnedPillars;
    int requiredPillars;
    int variation;

};


class MyRect : public MyBuilding
{
public:
    friend class MyGraphicsScene;
    friend class Builder;
    enum Var { NormalRect, Ellipse, Circle };
    KindOfShape getType() const {return Rect;}
    QString getDescript() const;
    void preinit();
    MyRect();
    MyRect(MyPoint * point1, MyPoint * point2,bool IsImage = false,
           int Variation=NormalRect );
    MyRect(const QList<MyPoint *>  & buildList,bool IsImage = false
            , int Variation = NormalRect);
    void postinit(MyPoint * point1, MyPoint * point2
                  , bool IsImage, int Variation);
    void neoinit(const QList<MyPoint *> & MyPoints);
    ~MyRect() { qDebug() << "a rect dying"; }
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option
               , QWidget *widget=0);
    QPainterPath shape() const;
    qreal width() const{return mypoints[0]->x()-mypoints[1]->x();}
    qreal height() const{return mypoints[0]->y()-mypoints[1]->y();}
    static QPainterPath circle(const QPointF & center,const QPointF & pointa);

    //repositioning mypoint 3 and mypoint 4;
    void prepareGeometryChange();
    void setIsImage(bool state) {isImage_ = state;}
    const QList<MyPoint *> & getPoints() const {return mypoints;}
    //     QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:     
    QList<MyPoint *> mypoints;
    //4 points for ellipse and rect, with the 3rd and 4th as spawned
    //2 points for circle
};


class MyPolygon : public MyBuilding
{
public:
    friend class MyGraphicsScene;
    friend class Builder;
    KindOfShape getType() const {return Polygon;}
    QString getDescript() const {return tr("Polygon");}
    void preinit();
    void postinit(const QList<MyPoint *> &thepoints);
    void neoinit(const QList<MyPoint *> &thepoints
                 ,Qt::FillRule FillRule=Qt::OddEvenFill);
    MyPolygon();
    MyPolygon(const QList<MyPoint *> &thepoints);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option
               , QWidget *widget=0);
    Qt::FillRule getFillRule() const {return myfillrule;}
    void setFillRule(Qt::FillRule newfillrule )
        {prepareGeometryChange(); myfillrule=newfillrule;}        
    QPolygonF getPolygon() const;
    const QList<MyPoint *> & getPoints() const {return mypoints;}
private:
    QList<MyPoint *>  mypoints;
    Qt::FillRule myfillrule;
};

class MySegment : public MyBuilding
{
public:
    friend class MyGraphicsScene;
    friend class Builder;
    enum Var {NormalSegment, Line, Ray};
     KindOfShape getType() const {return Segment;}
     QString getDescript() const;
     void preinit();
     void postinit(MyPoint * pointa, MyPoint * pointb
                   , int Variation=NormalSegment);
     void neoinit(MyPoint * pointa, MyPoint * pointb);
     MySegment();
     MySegment(MyPoint * pointa, MyPoint * pointb
               , int Variation=NormalSegment);
     QRectF boundingRect() const;
     QPainterPath preshape() const;
     QPainterPath shape() const;
     void penAndBrush(QPainter *painter
                      , const QStyleOptionGraphicsItem *option
                      , QWidget *widget);
     static bool lineMeetsRect(const QPointF & pointa
                               , const  QPointF & pointb,
                               const QRectF & rect);
     static QPainterPath infiniteLine(const QPointF & pointa
                                      , const  QPointF & pointb,
                                      const QRectF & rect);
     static QPainterPath infiniteRay(const QPointF &pointa
                                     , const QPointF &pointb,
                                     const QRectF &rect);

     void paint(QPainter *painter,
                const QStyleOptionGraphicsItem *option, QWidget *widget);
     const MyPoint * getPoint(int i) const;
private:
     MyPoint * point1;
     MyPoint * point2;
};


class MyTransform : public MyBuilding
{
public:
    friend class MyGraphicsScene;
    friend class Builder;
    KindOfShape getType() const {return Transform;}
    QString getDescript() const {return QObject::tr("Transform");}
    MyTransform(MyPoint * originPoint, const QList<MyShape *> &source,
                qreal Angle = 60,
                qreal Scale=1);    
    MyTransform();
    void preinit();
    void postinit(MyPoint * originPoint, const QList<MyShape *> &source,
                  qreal Angle = 60,
                  qreal Scale=1);
    void neoinit(MyPoint * originPoint
                 , const QList<MyPoint *> &FromPoints
                 , const QList<MyPoint *> &ToPoints
                 , const QList<MyBuilding *> &FromBuildings
                 , const QList<MyBuilding *> &ToBuildings
                 ,qreal Angle = 60
                 ,qreal Scale=1);
    QRectF boundingRect() const {return QRectF();}
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option
                , QWidget *widget=0){}
     //repositioning mypoint 3 and mypoint 4;
     void prepareGeometryChange();
     qreal getMyAngle(){return myangle;}
     qreal getMyScale(){return myscale;}
     void setMyAngle(qreal newangle)
        { myangle=newangle; prepareGeometryChange();}
     void setMyScale(qreal newscale)
        {myscale = newscale; prepareGeometryChange();}
     QList<MyShape *> myChildren() const;
     const MyPoint * getOrigin() const {return origin;}
     const QList<MyPoint *> & getFromPoints() const {return fromPoints;}
     const QList<MyPoint *> & getToPoints() const {return toPoints;}
     const QList<MyBuilding *> & getFromBuildings() const
            {return fromBuildings;}
     const QList<MyBuilding *> & getToBuildings() const
            {return toBuildings;}
private:
     QList<MyPoint *> fromPoints;
     QList<MyPoint *> toPoints;
     QList<MyBuilding *> fromBuildings;
     QList<MyBuilding *> toBuildings;
     MyPoint * origin;
     qreal myangle;
     qreal myscale;
};

//this class takes care of pillars and buildings
class Builder {
public:
//    static bool isSupport(MyPoint * pillar,  MyBuilding * building){
//        return ((pillar->getBuildings().contains(building)) &&
//                (building->getPillars().contains(pillar)));
//    }
    static bool isSupport(MyPoint * pillar, MyBuilding * building ){
        return  ( pillar->buildings.contains(building) &&
                  building->pillars.contains(pillar));
    }
//    static bool isSpawned(MyPoint * pillar, MyBuilding * building){
//        return (building->getSpawned().contains(pillar));
//    }
    static bool isSpawned(MyPoint * pillar, MyBuilding * building){
        return (building->spawnedPillars.contains(pillar));
    }
    static void support(MyPoint * pillar, MyBuilding * building,bool ok=true){
        Q_ASSERT( isSupport(pillar,building) != ok );
        if (ok){
            pillar->buildings << building;
            building->pillars << pillar;
        }
        else {
            pillar->buildings.remove(building);
            building->pillars.removeAll(pillar);
        }
    }
    static void spawnPillar(MyPoint * spawnedPillar, MyBuilding * building,
                            bool ok = true){
        Q_ASSERT(isSpawned(spawnedPillar,building) != ok);
        if (ok){
            building->spawnedPillars << spawnedPillar;
            spawnedPillar->parentBuilding = building;
        }
        else {
            building->spawnedPillars.removeAll(spawnedPillar);
            spawnedPillar->parentBuilding = 0;
        }
    }
    static QSet<MyBuilding *> onScene (const QSet<MyBuilding *> & set){
        QSet<MyBuilding *> kq;
        foreach (MyBuilding * building,set)
            if (building->scene() != 0) kq << building;
        return kq;
    }
    static QSet<MyPoint *> onScene (const QSet<MyPoint *> & set){
        QSet<MyPoint *> kq;
        foreach (MyPoint * point,set)
            if (point->scene() != 0) kq << point;
        return kq;
    }
    static bool isOnScene(const QSet<MyPoint *> & set){
        foreach (MyPoint * point,set)
            if (point->scene() ==0) return false;
        return true;
    }
    static bool isOnScene(const QSet<MyBuilding *> & set){
        foreach (MyBuilding * member,set)
            if (member->scene() ==0) return false;
        return true;
    }        
    static MyPoint * clonePoint(MyPoint * source){
        return new MyPoint(source->scenePos());
    }
    static QList<MyPoint *> cloneListOfPoints(QList<MyPoint *> source){
        QList<MyPoint *> New;
        foreach(MyPoint * point, source) New << clonePoint(point);
        return New;
    }
    //Use the build function for saving/loading/transforming
    //The intention is to just provide the list of pillars and
    //build a buiding from that list (but the supporting will be
    //provided by MyGraphicsScene)
    static MyBuilding * build(const QList<MyPoint *> & buildList
                              ,MyShape::KindOfShape kind
                              ,bool creatingImage = false
                              ,int Variation = 1);  
};
#endif // MYSHAPE_H
