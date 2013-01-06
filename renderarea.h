#ifndef RENDERAREA_H
#define RENDERAREA_H
#include "myshape.h"

class MyMainWindow;
class MyGraphicsScene;
class Renderarea;
class MyTreeView;
class Trash : public QObject {
    Q_OBJECT
public:
    ~Trash() {qDeleteAll(bin);}
    QSet<MyShape *> bin;
};





class Add{
public:
    Add(MyShape::KindOfShape Kind = MyShape::Nothing, int Variation = 1)
        :kind(Kind),variation(Variation){}
    MyShape::KindOfShape kind;
    int variation;
    bool operator<(const Add & other) const;
    bool operator==(const Add & other) const;
};

class MyMode
{
public:
    enum ModeName { NormalMode, AddMode, EditTransformMode, LoadingMode};
    MyMode(ModeName name = NormalMode, Add theadd = Add())
        :modename(name),add(theadd){}
    ModeName modename;
    Add add;
    bool operator ==(const MyMode & other) const;
};

class ShapeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum DisplayMode { Tree, List };
    enum {NoOfHeaders = 3};
    enum Header {Label=0,Visible,Kind};
    ShapeModel(MyGraphicsScene * Scene, QObject * parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column
                      , const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section
                        , Qt::Orientation orientation, int role) const;
    QModelIndex lookup(MyShape * shape) const;
    QModelIndex lookupParent(MyShape * shape) const;
    void beginInsertShape(MyShape * shape);
    void endInsertShape();
    void beginRemoveShape(MyShape * shape);
    void endRemoveShape();
    void changedShape(MyShape * shape);
    void changedShapes(const QList<MyShape *> & shapes)
        {foreach (MyShape * shape, shapes) changedShape(shape);}
    void beginResetModel() { QAbstractItemModel::beginResetModel();}
    void endResetModel() { QAbstractItemModel::endResetModel();}

    DisplayMode getDisplaymode() const {return displayMode;}

signals:

public slots:
    void treeMode();
    void listMode();
private:
    MyGraphicsScene * scene;
    DisplayMode displayMode;
    bool removingShape;
    bool addingShape;
};




class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    friend class MyMainWindow;
    friend class Renderarea;
    enum MouseMode {Normal, Select, Pan};
    explicit MyGraphicsScene(QWidget *parent = 0);
    Renderarea * myRender() const{
        return   (Renderarea *) views().first();
    }
    void setMode(MyMode newmode );
    void setClean();

    QRectF visibleRect() const;
    void move(const QList<MyShape *> & theitems
                ,const QPointF & thedistance);
    void support(MyPoint * pillar, MyBuilding * building, bool ok=true);
    void spawnPillar(MyPoint * spawnedpillar, MyBuilding * building,
                     bool ok=true);
    void addItemOne(MyShape * item);
    void addItemMacro(MyShape * item);
    void setColor(MyShape * item);
    //a recursive function, like dominoes
    void removeItemOne(MyShape * item);
    void removeItemMacro(MyShape * item);
    void setParentOne(MyShape * child, MyShape * parent,bool ok=true);
    void transformOne(MyTransform *  tran, qreal newangle, qreal newscale);
    void showHideOne(const QList<MyShape *> &shapes, bool Visible);
    void showHideOne(MyShape * shape, bool Visible);
    void renameOne(MyShape * shape, const QString & newlabel);

    void checkPillarsAndBuildings() ;

    ~MyGraphicsScene();
    void myclear();
    void setZMAX(int newmax);
    int zmaxvalue() {return zmax;}    

    QList<MyShape *> & accessMyItems() {return myItems_;}
    const QList<MyShape *> & myItemsConst() const { return myItems_;}
    QList<MyShape *> & topLevelItems()  {return topLevelItems_;}
    const QList<MyShape *> & topLevelItemsConst() const {return topLevelItems_;}
    QMap<QString,MyShape *> & accessMapLabel()  {return mapLabel_;}
    const QMap<QString,MyShape *> & mapLabelConst() const {return mapLabel_;}
    QList<MyShape *> recalcItems() const;
    void checkItemList() const;
    //myshape version of selecteditems()
    QList<MyShape *> mySelectedItems();
    int getSceneScale() const {return SceneScale;}
    void setSceneScale(int a) {SceneScale = a;}
    ShapeModel * accessModel()  {return model;}
    Trash * trash;
    void clearTrash();    
    QFile & accessFile() {return file;}
    MyTreeView * accessTreeView()  const {return treeView;}
    void setTreeView(MyTreeView * view) {treeView = view;}
    MouseMode & accessMouseMode() {return mouseMode;}
    QList<MyPoint*> & accessBuffer() {return buffer;}
signals:
    void changedModes();
    void needContextMenu();
    void showTips(QString string = QString());
    void editThisTransform(MyTransform * );
    void modified(bool state = true);
public slots:
    void checkState();
    void treeSelectionChanged();
    void sceneSelectionChanged();
    void clearDecor();
    void clearBuffer();
protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
private:    
    void startPanning(const QPointF & pos);
    void startSelectDrag(const QPointF & pos);
    QList<MyShape *> myItems_;
    QMap<QString,MyShape *> mapLabel_;
    QList<MyShape *> topLevelItems_;

    MouseMode mouseMode;
    QPointF mouseSelectAnchor;
    QPoint mouseDragAnchor;

    QFile file;
    ShapeModel * model;
    MyTreeView * treeView;
    MyMode currentMode;
    MyMode lastMode;
    QPen currentPen;
    QBrush currentBrush;
    //the scene holds the undo stack
    QUndoStack * undoStack;
    bool inMacro;    
    //the buffer that holds temporary pointsfor the modes
    //gets cleaned when the mode changed
    QList<MyPoint *> buffer;

    //highest value of zvalue, all points have
    //zvalue of zmax + 1 by default
    int zmax;
    QPointF startmove;
    QPointF endmove;
    bool ismoving;
    QList<MyShape *> movinglist;
    QGraphicsPathItem * mouseDecor;
    int SceneScale;    
};


class Renderarea : public QGraphicsView
{
    Q_OBJECT
public:
    enum {lengthOfTick=50};
    explicit Renderarea(QWidget *parent = 0);
    ~Renderarea() {delete myscene;}
//    void paintEvent(QPaintEvent *);
    QRectF visibleRect() const {
        return mapToScene(0,0,width(),height()).boundingRect();
    }
    virtual void drawBackground(QPainter *painter, const QRectF &rect);
    void setGrid(bool Enabled) ;


    bool isGridEnabled() const {return gridEnabled;}
    MyGraphicsScene * accessScene() const {return myscene;}
    int getScale() const {return RenderareaScale;}
    void setSubwin(QMdiSubWindow * win) { subWindow = win;}
    QMdiSubWindow * accessSubwin() const {return subWindow;}
    QPoint & accessLastPan() {return lastPan;}
protected:
    void closeEvent(QCloseEvent *);
    void mouseMoveEvent(QMouseEvent *event);
private:
    MyGraphicsScene * myscene;
    int RenderareaScale;
    bool gridEnabled;
    QMdiSubWindow * subWindow;
    QPoint lastPan;
signals:
    void scaleChanged(int newval);
    void savePls(bool * a = 0);
public slots:    
    void scale_slot(int k, bool signal = false);

    // k = 60 thi scale la 60%

};




#endif // RENDERAREA_H

