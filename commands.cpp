#include "commands.h"




ModifyShapeCommand::ModifyShapeCommand(const QList<MyShape *> &ItemList
                                       , const QPen &newpen
                                       , const QBrush &newbrush
                                       , bool UsingSlider
                                       , bool LineChanged
                                       , bool BrushChanged)
    :myitemlist(ItemList),mynewpen(newpen)
    ,mynewbrush(newbrush),usingSlider(UsingSlider)
    ,lineChanged(LineChanged),brushChanged(BrushChanged)

{
    foreach (MyShape * item, ItemList) {
        myoldpens << item->getPen();
        myoldbrushes << item->getBrush();
    }
    setText(QObject::tr("changing sth"));
}

bool ModifyShapeCommand::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) return false;
    ModifyShapeCommand * theOther = (ModifyShapeCommand *) other;
    if ( !(usingSlider && theOther->usingSlider) ) {
        return false;
    }
    if (theOther->lineChanged) mynewpen = theOther->mynewpen;
    if (theOther->brushChanged) mynewbrush = theOther->mynewbrush;
    lineChanged |= theOther->lineChanged;
    brushChanged |=theOther->brushChanged;

    return true;
}


void ModifyShapeCommand::redo(){    
    foreach (MyShape * myitem, myitemlist){
        if (lineChanged) myitem->setPen(mynewpen);
        if (brushChanged) myitem->setBrush(mynewbrush);
        myitem->prepareGeometryChange();
    }
}

void ModifyShapeCommand::undo(){    
    for (int i =0; i<myitemlist.size(); i++){
        MyShape * myitem(myitemlist[i]);
        if (lineChanged) myitem->setPen(myoldpens[i]);
        if (brushChanged) myitem->setBrush(myoldbrushes[i]);
        myitem->prepareGeometryChange();
    }
}

bool TransformCommand::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) return false;
    TransformCommand * theOther = (TransformCommand *) other;
    newangle = theOther->newangle;
    newscale = theOther->newscale;    
    return true;
}

void TransformCommand::redo()
{
    item->setMyAngle(newangle);
    item->setMyScale(newscale);
}

void TransformCommand::undo()
{
    item->setMyAngle(oldangle);
    item->setMyScale(oldscale);
}

void BGChangeCommand::redo()
{
    scene_->setBackgroundBrush(newbrush);
}

void BGChangeCommand::undo()
{
    scene_->setBackgroundBrush(oldbrush);
}

BGChangeCommand::BGChangeCommand(QGraphicsScene *Scene, const QBrush &Newbrush, bool UsingSlider)
    :scene_(Scene),oldbrush(Scene->backgroundBrush()),newbrush(Newbrush),
      usingSlider(UsingSlider)
{
    setText(QObject::tr("Changing background"));
}

bool BGChangeCommand::mergeWith(const QUndoCommand *other)
{
     if (other->id() != id()) return false;
     BGChangeCommand * theOther= (BGChangeCommand *) other;
     newbrush = theOther->newbrush;
     return true;
}

ShowCommand::ShowCommand(const QList<MyShape *> &Shapes, MyGraphicsScene *Scene, bool Visible, bool MergeAble)
    :visible(Visible),mergeAble(MergeAble),scene(Scene)
{
    foreach (MyShape * shape, Shapes)
        if (shape->isVisible() != visible) shapes << shape;

    if (visible)   setText(QObject::tr("showing sth"));
    else  setText(QObject::tr("hiding sth"));
}

bool ShowCommand::mergeWith(const QUndoCommand *other)
{
    if ( (other->id() != id()) || (!mergeAble) ) return false;
    ShowCommand * theOther = (ShowCommand *) other;
    if (visible != theOther->visible) return false;
    shapes << theOther->shapes;
    return true;
}

void ShowCommand::redo()
{
    foreach (MyShape * shape, shapes) {
        shape->setVisible(visible);
    }
    scene->accessModel()->changedShapes(shapes);
}

void ShowCommand::undo()
{
     foreach (MyShape * shape, shapes) shape->setVisible(!visible);
     scene->accessModel()->changedShapes(shapes);
}

void Common::add(MyGraphicsScene *myscene, MyShape *myitem, bool topLevel)
{
    myscene->accessModel()->beginInsertShape(myitem);
    if (myitem->getLabel().isEmpty()) myitem->setLabel(giveName(0));
    int i = 0;
    while (myscene->accessMapLabel().contains(myitem->getLabel()))
    {
        myitem->setLabel(giveName(i));
        ++i;
    }
    myscene->QGraphicsScene::addItem(myitem);
    myscene->accessMapLabel()[myitem->getLabel()] = myitem;

    myscene->accessMyItems() << myitem;
    if (topLevel) myscene->topLevelItems() << myitem;
    myscene->accessModel()->endInsertShape();
}

void Common::remove(MyGraphicsScene *myscene, MyShape *myitem, bool topLevel)
{

    myscene->accessModel()->beginRemoveShape(myitem);
    myscene->QGraphicsScene::removeItem(myitem);
    myscene->accessMapLabel().remove(myitem->getLabel());
    myscene->accessMyItems().removeAll(myitem);

    if (topLevel) myscene->topLevelItems().removeAll(myitem);
    myscene->accessModel()->endRemoveShape();
}

RenameCommand::RenameCommand(MyShape *Shape, MyGraphicsScene *Scene, const QString &Newlabel)
{
    shape = Shape; scene= Scene;
    oldlabel = shape->getLabel(); newlabel = Newlabel;
    setText(tr("renaming ") + oldlabel + tr(" to ") + newlabel);
    if ( (shape == 0) || (oldlabel == newlabel) || (shape->scene() != scene))
        DoNothing = true;
}

void RenameCommand::redo()
{
    if (!DoNothing)  {
        shape->setLabel(newlabel);
        scene->accessMapLabel().remove(scene->accessMapLabel().key(shape));
        scene->accessMapLabel()[newlabel] = shape;
        scene->accessModel()->changedShape(shape);
    }
}

void RenameCommand::undo()
{
    if (!DoNothing) {
        shape->setLabel(oldlabel);
        scene->accessMapLabel().remove(scene->accessMapLabel().key(shape));
        scene->accessMapLabel()[oldlabel] = shape;
        scene->accessModel()->changedShape(shape);
    }
}


