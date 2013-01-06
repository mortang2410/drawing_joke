#ifndef COMMANDS_H
#define COMMANDS_H
#include "qtmodules.h"
#include "renderarea.h"

//implementing undo/redo


class MyUndo : public QUndoCommand{
public:
    enum {Move =10000, Delete, Support,SpawnPillar
          , SetParent, ModifyShape, Transform, BGChange, Show, Rename};
    MyUndo() :DoNothing(false){}
protected:
    //it might be that a command doesn't have anything to do
    //because some conditions are not met
    //In such cases, we want redo() and undo() to do nothing;
    bool DoNothing;
};

class MoveCommand : public MyUndo
{
public:
    //the first time, do nothing because the items should already
    //be at endmove
    MoveCommand(const QList<MyShape *> & theitems
                ,const QPointF & thedistance)
        :items(theitems),distance(thedistance),firstTime(true) {
        setText(QObject::tr("moving sth"));
    }
    void redo(){
        if (firstTime) firstTime = false;
        else foreach (MyShape * item, items) {
            item->myMoveBy(distance);
        }
    }
    void undo(){
        foreach (MyShape * item, items){
            item->myMoveBy(-distance);
        }
    }
private:
    bool firstTime;
    QList<MyShape *> items;
    QPointF distance;
};

class SupportCommand : public MyUndo
{
public:
    SupportCommand(MyPoint * pillar, MyBuilding * building, bool ok=true)
        :mypillar(pillar),mybuilding(building),myok(ok)
    {
        if ( Builder::isSupport(pillar,building) == ok) DoNothing = true;
    }
    void redo() { if (DoNothing) return;
        Builder::support(mypillar,mybuilding,myok);}
    void undo() { if (DoNothing) return;
        Builder::support(mypillar,mybuilding,!myok);}
public:
    MyPoint * mypillar;
    MyBuilding * mybuilding;
    bool myok;
};

class SpawnPillarCommand : public MyUndo
{
public:
    SpawnPillarCommand(MyPoint * spawnedPillar, MyBuilding * building,
                       bool ok = true)
        :myspawned(spawnedPillar),mybuilding(building),myok(ok)
    {
        if (Builder::isSpawned(spawnedPillar,building) == ok )
            DoNothing = true;
    }
    void redo() { if (DoNothing) return;
        Builder::spawnPillar(myspawned,mybuilding,myok);}
    void undo() { if (DoNothing) return;
        Builder::spawnPillar(myspawned,mybuilding,!myok);}
private:
    MyPoint * myspawned;
    MyBuilding * mybuilding;
    bool myok;
    bool doSomething;
};

class Common{
public:
static void add(MyGraphicsScene * myscene,MyShape * myitem, bool topLevel);
static void remove(MyGraphicsScene * myscene,MyShape * myitem, bool topLevel);
};

class DeleteCommand : public MyUndo
{
public:
    DeleteCommand(MyShape * theitem, MyGraphicsScene * thescene)
        :myitem(theitem),myscene(thescene)
    {
        if (myitem->scene() != myscene) DoNothing = true;
        setText(QObject::tr("deleting sth"));
        topLevel = theitem->isToplevel();
    }
    void redo() {
        if (DoNothing) return;
        Common::remove(myscene,myitem,topLevel);
    }
    void undo() {
        if (DoNothing) return;
        Common::add(myscene,myitem,topLevel);
    }
private:
    MyGraphicsScene * myscene;
    MyShape * myitem;
    bool topLevel;
};


class AddShapeCommand : public MyUndo
{
public:
    AddShapeCommand(MyShape * item, MyGraphicsScene * scene)
    :myscene(scene)
        {
        if (item->scene()) {
            DoNothing = true;
        }
        else myitem= item;
        setText(QObject::tr("adding sth"));
        topLevel = item->isToplevel();
        }
    ~AddShapeCommand() { myscene->trash->bin << myitem;}
    void redo() {
        if (DoNothing) return;
        Common::add(myscene,myitem,topLevel);
    }
    void undo() {
        if (DoNothing) return;
        Common::remove(myscene,myitem,topLevel);
    }
private:
    MyShape * myitem;
    MyGraphicsScene * myscene;
    bool topLevel;
};

class SetParentCommand : public MyUndo
{
public:
    SetParentCommand(MyShape * Child, MyShape * Parent, bool Ok = true)
        :child(Child),parent(Parent),ok(Ok) {}
    void redo(){
        if (ok) {
            child->setParentItem(parent);
        }
        else child->setParentItem(0);
    }
    void undo(){
        if (ok){
            child->setParentItem(0);
            Q_ASSERT(child->parentItem() == 0);
        }
        else child->setParentItem(parent);
    }
private:
    MyShape * child;
    MyShape * parent;
    bool ok;
};


class ModifyShapeCommand :  public MyUndo
{
public:
    ModifyShapeCommand(const QList<MyShape *> &ItemList, const QPen &  newpen,
                       const QBrush &newbrush,
                       bool UsingSlider=false,
                       bool LineChanged = true,
                       bool BrushChanged = true);
    int id() const{return ModifyShape;}
    bool mergeWith(const QUndoCommand *other);
    void redo();
    void undo();
private:
   QList<MyShape *> myitemlist;
   QList<QPen> myoldpens;
   QPen mynewpen;
   QList<QBrush> myoldbrushes;
   QBrush mynewbrush;  
   bool usingSlider;
   bool lineChanged;
   bool brushChanged;
};

class TransformCommand : public MyUndo
{
public:
    TransformCommand(MyTransform *  Item, qreal Newangle,qreal Newscale)
        :item(Item),oldangle(item->getMyAngle()),newangle(Newangle)
        ,oldscale(item->getMyScale()),newscale(Newscale)
    {setText(QObject::tr("transforming sth"));}

    int id() const { return Transform; }
    bool mergeWith(const QUndoCommand *other);
    void redo();
    void undo();
private:
    MyTransform *  item;
    qreal oldangle;
    qreal newangle;
    qreal oldscale;
    qreal newscale;
};

class BGChangeCommand : public MyUndo
{
public:
    BGChangeCommand(QGraphicsScene * Scene,const QBrush & Newbrush,
                    bool UsingSlider=false);
    int id() const {return BGChange;}
    bool mergeWith(const QUndoCommand *other);
    void redo();
    void undo();
private:
    QGraphicsScene * scene_;
    QBrush oldbrush;
    QBrush newbrush;
    bool usingSlider;
};

class ShowCommand : public MyUndo
{
public:
    ShowCommand(const QList<MyShape *> & Shapes, MyGraphicsScene * Scene,
                bool Visible, bool MergeAble = false );
    int id() const {return Show;}
    bool mergeWith(const QUndoCommand *other);
    void redo();

    void undo();

private:
    QList<MyShape *> shapes;
    MyGraphicsScene * scene;
    bool visible;
    bool mergeAble;
};

class RenameCommand : public MyUndo
{
public:
    RenameCommand(MyShape * Shape,MyGraphicsScene * Scene,
                  const QString & Newlabel);
    void redo();
    void undo();
private:
    MyShape * shape;
    MyGraphicsScene * scene;
    QString oldlabel;
    QString newlabel;
};

#endif // COMMANDS_H
