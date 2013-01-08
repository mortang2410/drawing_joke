#include "renderarea.h"
#include "commands.h"
#include "mydockwidgets.h"
#include <Qt>
Renderarea::Renderarea(QWidget *parent) :
    QGraphicsView(parent)
{   
    gridEnabled = true;
    RenderareaScale = 100;
    subWindow = 0;
    setMinimumSize(300,300);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    myscene= new MyGraphicsScene();
    myscene->setObjectName("myownscene");
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setScene(myscene);    
    penBackground = QPen(Qt::lightGray);
    penBackground.setCosmetic(true);
    setCacheMode(QGraphicsView::CacheNone);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState,true);
//    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setRenderHint(QPainter::SmoothPixmapTransform);
//    setRenderHint(QPainter::Antialiasing);
//    setRenderHint(QPainter::NonCosmeticDefaultPen);
}


void Renderarea::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter,rect);
    if (gridEnabled) {
        painter->setWorldMatrixEnabled(true);
        setRenderHint(QPainter::NonCosmeticDefaultPen,false);
        painter->setPen(penBackground);
        painter->setRenderHint(QPainter::NonCosmeticDefaultPen,false);
        qreal rawTick = normalize(mapToScene(lengthOfTick,0)
                                  - mapToScene(0,0));
        qreal actualTick = qPow(2,getNearestPowerOfTwo(rawTick));
        QFont newfont(painter->font());
        newfont.setPixelSize((int) (mapToScene(0,20)-mapToScene(0,0))
                             .manhattanLength());
        newfont.setStretch(QFont::SemiCondensed);
        painter->setFont(newfont);
//        painter->setPen(QColor(Qt::lightGray));
        for (qreal x=qCeil(rect.left()/actualTick)*actualTick;
             x<= qFloor(rect.right()/actualTick)*actualTick;
             x += actualTick)
        {
            if (mymod(x/actualTick,5)==0){
                painter->save();
                painter->setPen( QPen(QBrush(Qt::darkGray),0)  );
                painter->drawText(QPointF(x,rect.bottom()),
                                  QString::number(x,'g',3));

            }
            painter->drawLine(x,rect.bottom(),x,rect.top());
            if (mymod(x/actualTick,5)==0){
                painter->restore();
            }
        }
        for (qreal y=qCeil(rect.top()/actualTick)*actualTick;
             y<= qFloor(rect.bottom()/actualTick)*actualTick; y += actualTick)
        {
            if (mymod(y/actualTick,5)==0){
                painter->save();
                painter->setPen( QPen(QBrush(Qt::darkGray),0)  );
                painter->drawText(QPointF(rect.left(),y),
                                  QString::number(-y,'g',3));
            }
            painter->drawLine(rect.left(),y,rect.right(),y);
            if (mymod(y/actualTick,5)==0){
                painter->restore();
            }
        }
    }
}

void Renderarea::setGrid(bool Enabled)
{
    if (gridEnabled != Enabled) {
        gridEnabled = Enabled;
        myscene->update();
    }
}

void Renderarea::closeEvent(QCloseEvent * event)
{
    if (subWindow->isWindowModified()) {
        QMessageBox msgBox;
        msgBox.setText(tr("The document has been modified."));
        msgBox.setInformativeText(tr("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Save:
            {
                bool a;
                emit savePls(&a);
                if (a) event->accept();
                else event->ignore();
            }
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        default:

            break;
        }
        return;
    }
    event->accept();
}

void Renderarea::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    if ((myscene->accessMouseMode() == MyGraphicsScene::Pan)
            && (event->buttons() == Qt::LeftButton ))
    {
        QPoint newpoint = event->pos();
        QPoint delta = (newpoint - lastPan);
        if (delta.manhattanLength() < 80) return;
        QPointF newcenter = mapToScene(rect().center() - delta);
        QRectF currentRect = visibleRect();
        QRectF totalRect = myscene->sceneRect();
        QRectF desiredRect = enlargeRect(newcenter,currentRect);
        if (!totalRect.contains(desiredRect)) {
            myscene->setSceneRect(totalRect.united(desiredRect));
        }
        centerOn(newcenter);
        lastPan = newpoint;
    }
}


QList<MyShape *>  MyGraphicsScene::mySelectedItems()
{
    //don't use this too much unless you really need a list of MyShapes
    //as this will create a totally new list
    QList<MyShape *> thelist;
    QGraphicsItem * item;
    foreach (item, selectedItems()) {
        if (item != mouseDecor) thelist.append( (MyShape *) item);
    }
    return thelist;
}

//THIS FUNCTION WILL BE OPTIMIZED LATER. IT IS NO LONGER NECESSARY.
//FOR A VERY LONG TIME I STRUGGLED WITH DELETING THESE ITEMS
//THE APP WOULD MYSTERIOUSLY SEGFAULT AND I TRIED EVERYTHING I COULD
//THAT'S WHY THERE ARE CHECKS EVERYWHERE (WILL REMOVE THEM LATER).
//THEN THANKS TO GDB'S BACKTRACE I FIGURED OUT WHY
//DO NOT USE QGraphicsScene::BspTreeIndex !!!!! ****!!!!
//WE ARE ADDING AND REMOVING THINGS CONTINUOUSLY
//THE INDEX, WELL, IS MEANT FOR STATIC SCENES
//AND WILL CRASH WHEN YOU CLEAR OUT MANY THINGS AT ONCE!!!!
void MyGraphicsScene::clearTrash()
{
    if (trash->bin.isEmpty()) return;
    foreach (MyShape * item,trash->bin){
        Q_ASSERT(item->scene() == 0);
        Q_ASSERT( (QGraphicsScene::items().contains(item)== false));
        Q_ASSERT(item->childItems().isEmpty());
        Q_ASSERT(item->parentItem()==0);
        if (item->isPoint()){
            MyPoint * point= (MyPoint *)item ;
            Q_ASSERT(point->buildings.isEmpty());

        }
        else {
            MyBuilding * building ( (MyBuilding *) item);
            Q_ASSERT( (building->pillars.isEmpty()) &&
                     (building->spawnedPillars.isEmpty()) );
            trash->bin.remove(item);
        }
    }
    foreach (MyShape * item,trash->bin){
        if (item->isPoint() ) trash->bin.remove(item);
    }
}



void Renderarea::scale_slot(int k, bool signal)
{    
    int ka = qMax(qMin(k,1000),1);
    if (ka== RenderareaScale) return;
    QTransform mat;
    RenderareaScale = ka;
    myscene->setSceneScale(ka);
    mat.scale(ka/100.0,ka/100.0);
    setTransform(mat);
    if (signal) emit scaleChanged(ka);
}





MyGraphicsScene::MyGraphicsScene(QWidget *parent)
    :QGraphicsScene(parent),startmove(),endmove(),ismoving(false)
{
    connect(this,SIGNAL(selectionChanged())
            ,this,SLOT(sceneSelectionChanged()));
    mouseMode = Normal;
    treeView = 0;
    trash = new Trash;
    currentBrush.setStyle(Qt::SolidPattern);
    QColor defaultcolor(Qt::black); defaultcolor.setAlphaF(0);
    currentBrush.setColor(defaultcolor);
    setBackgroundBrush(QBrush(QColor(0,0,0,0),Qt::SolidPattern));
    setItemIndexMethod(QGraphicsScene::NoIndex); //very important!
    undoStack = new QUndoStack(this);
    connect(undoStack,SIGNAL(cleanChanged(bool)),this,SLOT(checkState()));
    undoStack->setClean();
    SceneScale=100;    
    zmax = 1;
    mouseDecor = new QGraphicsPathItem();
    mouseDecor->setPen(QPen(Qt::blue,0));
    inMacro = false;
    model = new ShapeModel(this,this);


//    MyPoint * C( new MyPoint(-130,55));
//    MyPoint * A( new MyPoint(10,10)); MyPoint * B( new MyPoint(70,70));
//    MyRect * Hinh(new MyRect(A,B));

//    addItemMacro(A); addItemMacro(B); addItemMacro(C); addItemMacro(Hinh);

//    QList<MyShape *> source;
//    source << (MyShape *) A << (MyShape * )B << (MyShape *)Hinh;
//    MyTransform *  bien(new MyTransform(C,source,45,1));
//    addItemMacro(bien);
//    source.clear();
//    foreach(MyPoint * point, bien->toPoints) source << point;
//    source << bien->toBuildings.first();
//    MyTransform *  bien2(new MyTransform(C,source,45,1));
//    addItemMacro(bien2);
//    checkItemList();
//    MyTransform *  bien2(new MyTransform(C,allShapes.toList(),45,2));
    //    addItemMacro(bien2);
}

void MyGraphicsScene::setMode(MyMode newmode)
{    
    lastMode = currentMode;
    currentMode = newmode;
    buffer.clear();
    emit changedModes();
}

void MyGraphicsScene::setClean()
{
    undoStack->setClean();
}



QRectF MyGraphicsScene::visibleRect() const
{
    Renderarea * view = (Renderarea *) views().first();
    if (view) return view->visibleRect();
    else return QRectF();
}

void MyGraphicsScene::move(const QList<MyShape *> &theitems, const QPointF &thedistance)
{
    if (!theitems.isEmpty())
        undoStack->push(new MoveCommand(theitems,thedistance));
}

void MyGraphicsScene::support(MyPoint * pillar, MyBuilding * building, bool ok)
{
    if (pillar && building)
        undoStack->push(new SupportCommand(pillar,building,ok));
}

void MyGraphicsScene::spawnPillar(MyPoint * spawnedpillar, MyBuilding * building, bool ok)
{
    if (spawnedpillar && building)
        undoStack->push(new SpawnPillarCommand(spawnedpillar,building,ok));
}


void MyGraphicsScene::addItemOne(MyShape * item)
{
    if (item && (item->scene() == 0))
        undoStack->push(new AddShapeCommand(item,this));        
}


void MyGraphicsScene::addItemMacro(MyShape * item)
{    
    if (item == 0) return;
    bool firstRecursion = false;
    if (inMacro == false){
        inMacro = true;
        firstRecursion=true;
        undoStack->beginMacro(tr("adding %1").arg(item->label));
    }
    switch (item->getType()){
    case MyShape::Rect:{
            MyRect * theitem= (MyRect *) item;
            switch (theitem->variation){
            case MyRect::NormalRect:
            case MyRect::Ellipse:
                {
                    for (int i=2; i<=3; ++i ){
                        spawnPillar(theitem->mypoints[i],theitem);                
                    }
                    for (int i=0; i<=3; ++i ){
                        MyPoint * point = theitem->mypoints[i];
                        support(point,theitem);
                        if (point->scene() == 0) addItemMacro(point);
                    }
                }
                break;
            case MyRect::Circle:
                {
                    for (int i=0; i<=1; ++i) {
                        support(theitem->mypoints[i],theitem);
                    }
                }
                break;
            }

            break;
        }
    case MyShape::Polygon:{
            MyPolygon *  theitem((MyPolygon * )item);
            foreach (MyPoint * point, theitem->mypoints)
                support(point,theitem);
            break;
        }
    case MyShape::Segment:{
            MySegment *  theitem( (MySegment * )item);
            support(theitem->point1,theitem);
            support(theitem->point2,theitem);
            break;
        }
    case MyShape::Point:{
            item->setZValue(zmax+1);
            break;
        }
    case MyShape::Transform:{
            MyTransform *  theitem ( (MyTransform * )item);
            support(theitem->origin,theitem);
            for (int i=0; i<theitem->fromPoints.size();++i) {
                MyPoint * From(theitem->fromPoints[i]);
                MyPoint * To(theitem->toPoints[i]);
                support(From,theitem); support(To,theitem);
                spawnPillar(To,theitem);
                addItemMacro(From);
                addItemMacro(To);
                setParentOne(To,From);
            }
            for (int i=0; i<theitem->fromBuildings.size();++i){
                MyBuilding * From(theitem->fromBuildings[i]);
                MyBuilding * To(theitem->toBuildings[i]);
                if (From->scene() ==0) addItemMacro(From);
                if (To->scene() ==0) addItemMacro(To);
                if (From->getType() ==MyShape::Rect)
                    setParentOne(To,From);
//                To->prepareGeometryChange();
            }

            break;
        }
    }
    addItemOne(item);    
    if (currentMode.modename != MyMode::LoadingMode) setColor(item);
    if (firstRecursion){
        inMacro = false;
        undoStack->endMacro();        
        checkPillarsAndBuildings();
        item->prepareGeometryChange();
    }
}

void MyGraphicsScene::setColor(MyShape *item)
{
    if  ( item && (item->isPoint() == false)) {
        item->setBrush(currentBrush);
        item->setPen(currentPen);
        item->prepareGeometryChange();
    }
}

void MyGraphicsScene::removeItemOne(MyShape * item)
{
    if ( item && (item->scene() == this) )
        undoStack->push(new DeleteCommand(item,this));
}


void MyGraphicsScene::removeItemMacro(MyShape * item)
{
    bool firstRecursion = false;
    if (item == 0) return;
    if (inMacro == false){
        inMacro = true;
        firstRecursion=true;
        undoStack->beginMacro(tr("removing macro"));
    }    
    if (item->parentItem())
        setParentOne(item,(MyShape *)(item->parentItem()),false);
    if (item->childItems().size()) {
        foreach (QGraphicsItem * subitem,item->childItems()) {
            setParentOne( (MyShape *) subitem,item,false);
        }
    }
    removeItemOne(item);    
    Q_ASSERT(item->scene() == 0);
    QSet<MyShape *> toWorkOn;
    if (item->isPoint()) {
        MyPoint *  theitem ( (MyPoint * )item);
        foreach (MyBuilding * building,
                 Builder::onScene(theitem->buildings))
        {
            support(theitem,building,false);
            toWorkOn << (MyShape *)(building);
        }
    }
    else {
        MyBuilding * theitem ( (MyBuilding *)item);
        foreach (MyPoint * pillar, theitem->pillars)
            support(pillar,theitem,false);
        foreach (MyPoint * spawnedPillar,theitem->spawnedPillars){
            spawnPillar(spawnedPillar,theitem,false);
            toWorkOn << (MyShape *)(spawnedPillar);
        }
    }
    foreach (MyShape * subitem,toWorkOn) {
        removeItemMacro(subitem);
    }
    if (firstRecursion){
        inMacro = false;
        undoStack->endMacro();
        checkPillarsAndBuildings();
    }
}

void MyGraphicsScene::setParentOne(MyShape * child, MyShape * parent,bool ok)
{
    if ( child && parent ) undoStack->push(new SetParentCommand(child,parent,ok));
}

void MyGraphicsScene::transformOne(MyTransform *  tran, qreal newangle, qreal newscale)
{
    if (tran) undoStack->push(new TransformCommand(tran,newangle,newscale));
}

void MyGraphicsScene::showHideOne(const QList<MyShape *> &shapes, bool Visible)
{     
    if (!shapes.isEmpty())
        undoStack->push(new ShowCommand(shapes,this,Visible));
}

void MyGraphicsScene::showHideOne(MyShape *shape, bool Visible)
{
    if (shape) {
        QList<MyShape*> list; list << shape;
        showHideOne(list,Visible);
    }
}

void MyGraphicsScene::renameOne(MyShape *shape, const QString &newlabel)
{
    if (shape && (shape->getLabel() != newlabel)) {
        if (mapLabel_.contains(newlabel)) {
            QMessageBox::warning(0,tr("Can't rename"),tr("This name has already been used."));
        }
        else undoStack->push(new RenameCommand(shape,this,newlabel));
    }
}

void MyGraphicsScene::checkState()
{
    emit modified(!undoStack->isClean());
}

void MyGraphicsScene::treeSelectionChanged()
{
    if (treeView == 0) return;
    clearSelection();
    QModelIndexList list = treeView->selectionModel()->selectedIndexes();
    foreach (QModelIndex ind, list) {
         ((MyShape *) ind.internalPointer())->setSelected(true);
    }
}

void MyGraphicsScene::sceneSelectionChanged()
{
    if (treeView) treeView->sceneSelectionChanged();
}

void MyGraphicsScene::clearDecor()
{
    if (mouseDecor->scene() == this) removeItem(mouseDecor);
}

void MyGraphicsScene::clearBuffer()
{
    buffer.clear();
}

void MyGraphicsScene::checkPillarsAndBuildings()
{
    foreach (QGraphicsItem * item, QGraphicsScene::items()) {
        if (MyPoint * pillar = dynamic_cast<MyPoint *>(item)){
            foreach (MyBuilding * building, pillar->getBuildings())
                Q_ASSERT(building->scene());
        }
        if (MyBuilding * building = dynamic_cast<MyBuilding *>(item)){
            foreach (MyPoint * pillar, building->getPillars()){
                Q_ASSERT(pillar->scene());
            }
        }
    }
}

MyGraphicsScene::~MyGraphicsScene()
{
    treeView = 0;
    myclear();
    clearTrash();
    delete mouseDecor;
    delete model;
    //this order is important
    delete undoStack;        
    delete trash;
}



void MyGraphicsScene::myclear()
{
    while (undoStack->canUndo()) {
        undoStack->undo();
    }
    undoStack->clear();
}

void MyGraphicsScene::setZMAX(int newmax)
{
    zmax=newmax;
    foreach (MyShape * item, myItemsConst()){
        if (item->getType()== MyShape::Point) item->setZValue(zmax+1);
    }
}

QList<MyShape *> MyGraphicsScene::recalcItems() const
{
    MyShape * shape; QList<MyShape*> list;
    foreach (QGraphicsItem * item, items()) {
         shape = dynamic_cast<MyShape*>(item);
         if (shape) list << shape;
    }
    return list;
}

void MyGraphicsScene::checkItemList() const
{
    int a = myItemsConst().size();
    int b = recalcItems().size();
    Q_ASSERT(a == b);
}


void MyGraphicsScene::keyPressEvent(QKeyEvent *event) {
    if (currentMode.modename != MyMode::NormalMode) {
        switch (event->key()){
        case (Qt::Key_Escape):
            setMode( MyMode());
            if (mouseDecor->scene()) removeItem(mouseDecor);
            break;
        default:
            ;
        }
    }
    if (currentMode.modename == MyMode::NormalMode) {
        Renderarea * ren = myRender();
        switch (event->key()) {
        case Qt::Key_Escape:
            clearSelection();
            break;
        case Qt::Key_Plus:
            ren->scale_slot(ren->getScale()+30,true);
            break;
        case Qt::Key_Equal:
            ren->scale_slot(ren->getScale()+10,true);
            break;
        case Qt::Key_Minus:
            ren->scale_slot(ren->getScale()-10,true);
            break;
        case Qt::Key_Underscore:
            ren->scale_slot(ren->getScale()-30,true);
            break;
        default:
            qDebug() << QString::number(event->key(),16);
        }
    }

    QGraphicsScene::keyPressEvent(event);
}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = event->scenePos();
    MyShape * item = dynamic_cast<MyShape *>(itemAt(pos,QTransform()));

    if (currentMode.modename == MyMode::NormalMode) {
        if (event->button()==Qt::LeftButton) {            
            if (event->modifiers() == Qt::NoModifier) {
                if (item) {                    
//                    qDebug() << "the label, pos, scenepos, parent "
//                             <<  "of this item are "
//                              << item->label
//                              <<item->pos() << item->scenePos()
//                             << ( (dad)? dad->label : "hello" );
//                    qDebug() << "rotation = " << item->rotation();
//                    qDebug() << "origin, in item and scene coor, is at "
//                             << item->transformOriginPoint()
//                             << item->mapToScene(item->transformOriginPoint());
//                    if (MyRect * rect = dynamic_cast<MyRect*>(item)){
//                        qDebug() << rect->mypoints[0]->label
//                                 << rect->mypoints[1]->label;
//                    }

                    if (item->isSelected()) {
                        ;
                    }
                    else {
                        clearSelection();
                        item->setSelected(true);
                    }
                    //only move when there is a point that can move
                    foreach (MyShape * stuff,mySelectedItems()){
                        if (stuff->isPoint())
                            if ( ((MyPoint *)stuff)->isMoveable())
                            {
                                ismoving = true;
                                startmove = pos;
                                break;
                            }
                    }
                }
                else{
                    clearSelection();
                    startSelectDrag(pos);
                }
            }
            if (event->modifiers() == Qt::CTRL){
                if (item) item->setSelected(true);
                else startSelectDrag(pos);
            }
            if (event->modifiers() == Qt::SHIFT) {
               startPanning(pos);
            }
        }
        if (event->button()==Qt::RightButton) {
            emit needContextMenu();
        }
    }
    if (currentMode.modename==MyMode::AddMode){
        switch (currentMode.add.kind){
        case MyShape::Point:
            {
                if ((event->modifiers() == Qt::NoModifier) &&
                        (event->button()==Qt::LeftButton)){
                    MyPoint * newpoint(new MyPoint(pos,true));
                    addItemMacro(newpoint);
                }
            }
            break;
        case MyShape::Rect:
            {
                bool shift = (event->modifiers() == Qt::ShiftModifier);
                if  ( ((event->modifiers() == Qt::NoModifier) || shift )
                     &&
                     (event->button()==Qt::LeftButton))
                {
                    if (shift && buffer.size() &&
                            ((currentMode.add.variation == MyRect::NormalRect)
                             || (currentMode.add.variation == MyRect::Ellipse))
                            )
                    {
                        pos = squarePoint(buffer.first()->scenePos(),pos);

                        item = dynamic_cast<MyShape *>(itemAt(pos,QTransform()));
                    }
                    if ( item && (item->isPoint()) ){
                        MyPoint * newpoint ((MyPoint *)item);
                        buffer<<newpoint;
                    }
                    else  {
                        MyPoint * newpoint(new MyPoint(pos));
                        buffer<<newpoint;
                        addItemMacro(newpoint);
                    }
                    if (buffer.size()==2) {
                        MyRect * rect(new MyRect(buffer.first()
                                                ,buffer.last()
                                                ,false
                                                ,currentMode.add.variation));
                        addItemMacro(rect);
                        buffer.clear();
                    }
                }

            }
            break;
        case MyShape::Polygon:
            {
                if ((event->modifiers() == Qt::NoModifier) &&
                        (event->button()==Qt::LeftButton))
                {
                    if  ( item && (item->isPoint()) ) {
                        MyPoint * point( (MyPoint *) item );
                        if ( buffer.size() && (point == buffer.first())){
                            MyPolygon *   mypoly(new MyPolygon(buffer));
                            addItemMacro(mypoly);
                            buffer.clear();
                        }
                        else buffer<<point;
                    }
                    else  {
                        MyPoint * newpoint (new MyPoint(pos));
                        buffer<<newpoint;
                        addItemMacro(newpoint);
                    }                                      

                }
                if ((event->modifiers() == Qt::NoModifier) &&
                        (event->button()==Qt::RightButton))
                {
                    if (buffer.size()){
                        MyPolygon *   mypoly(new MyPolygon(buffer));
                        addItemMacro(mypoly);
                        buffer.clear();
                    }
                }

            }
            break;
        case MyShape::Segment:
            {
                if ((event->modifiers() == Qt::NoModifier) &&
                        (event->button()==Qt::LeftButton)){
                    if (item && (item->isPoint())){
                        MyPoint * point ( (MyPoint *)item);
                        buffer<<point;
                    }
                    else  {
                        MyPoint * newpoint(new MyPoint(pos));
                        buffer<<newpoint;
                        addItemMacro(newpoint);
                    }
                    if (buffer.size()==2) {
                        MySegment *  seg(new MySegment(buffer.first(),buffer.last(),
                                                     currentMode.add.variation));
                        addItemMacro(seg);
                        buffer.clear();
                    }
                }
            }
            break;
        case MyShape::Transform:
            {
                if ((event->modifiers() == Qt::NoModifier) &&
                        (event->button()==Qt::LeftButton)) {
                    if (item) item->setSelected(!(item->isSelected()));
                }
                if ((event->modifiers() == Qt::NoModifier) &&
                        (event->button()==Qt::RightButton)) {
                    if (item && (item->isPoint())){
                        MyPoint * point( (MyPoint *) item);
                        buffer << point;
                    }
                    else{
                        MyPoint * newpoint(new MyPoint(pos));
                        buffer << newpoint;
                        addItemMacro(newpoint);
                    }
                    QList<MyShape *> list(mySelectedItems());
                    if (list.size()){
                        MyTransform *  tran(new MyTransform(buffer.first(),
                                                          list,50,1));
                        addItemMacro(tran);
                        emit editThisTransform(tran);
                    }
                    buffer.clear();
                }             
            }
            break;
        }
    }

    if (buffer.isEmpty() && mouseDecor->scene()) removeItem(mouseDecor);
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{    
    QPointF pos = event->scenePos();
    if ((buffer.size() == 0) && mouseDecor->scene())    
        QGraphicsScene::removeItem(mouseDecor); 
    if (ismoving){        
        foreach (QGraphicsItem * item, selectedItems())
            ((MyShape*) item)->myMoveBy(pos- event->lastScenePos());
    }
    if (currentMode.modename == MyMode::AddMode) {
        switch (currentMode.add.kind) {
        case MyShape::Rect:
            {
                bool shift = (event->modifiers() == Qt::ShiftModifier);
                bool bl = ((currentMode.add.variation == MyRect::NormalRect)
                             || (currentMode.add.variation == MyRect::Ellipse));
                if (buffer.size()) {
                    if (shift && bl) pos = squarePoint(buffer.first()->scenePos(),pos);
                    QPainterPath thepath;
                    QRectF rect(buffer.first()->scenePos(),pos);
                    switch (currentMode.add.variation) {
                    case MyRect::NormalRect:
                        thepath.addRect(rect);
                        break;
                    case MyRect::Ellipse:
                        thepath.addEllipse(rect);
                        break;
                    case MyRect::Circle:
                        thepath = MyRect::circle(buffer.first()->scenePos(),
                                                 pos);
                    }
                    mouseDecor->setPath(thepath);
                    if (mouseDecor->scene() ==0)
                        QGraphicsScene::addItem(mouseDecor);
                }
            }
            break;
        case MyShape::Polygon:
            {
                if (buffer.size()) {
                    QPainterPath thepath;
                    QPolygonF poly;
                    foreach (MyPoint * pt, buffer) poly << pt->scenePos();
                    poly << pos;
                    thepath.addPolygon(poly);
                    mouseDecor->setPath(thepath);
                    if (mouseDecor->scene() ==0)
                        QGraphicsScene::addItem(mouseDecor);
                }
            }
            break;
        case MyShape::Segment:
            {
                if (buffer.size()) {
                    QPainterPath thepath;
                    QPointF a(buffer.first()->scenePos());
                    QPointF b(pos);
                    switch (currentMode.add.variation){
                    case MySegment::NormalSegment:
                        thepath.moveTo(a);
                        thepath.lineTo(b);
                        break;
                    case MySegment::Ray:
                        thepath = MySegment::infiniteRay(a,b,visibleRect());
                        break;
                    case MySegment::Line:
                        thepath = MySegment::infiniteLine(a,b,visibleRect());
                    }
                    mouseDecor->setPath(thepath);
                    if (mouseDecor->scene() ==0)
                        QGraphicsScene::addItem(mouseDecor);
                }
            }
            break;
        }
    }
    if (currentMode.modename == MyMode::NormalMode) {
        switch (mouseMode) {
        case Select:
            {
                QPainterPath thepath;
                QRectF rect(mouseSelectAnchor,pos);
                thepath.addRect(rect);
                mouseDecor->setPath(thepath);
                if (mouseDecor->scene() ==0)
                    QGraphicsScene::addItem(mouseDecor);
                foreach (QGraphicsItem * item, mouseDecor->collidingItems())
                    if (!item->isSelected()) item->setSelected(true);
            }
            break;
        case Pan:
            {

                QPainterPath thepath;
                Renderarea * ren = myRender();
                QPointF a = ren->mapToScene(ren->accessLastPan());
                thepath.moveTo(a);
                thepath.lineTo(pos);
                mouseDecor->setPath(thepath);
                if (mouseDecor->scene() ==0)
                    QGraphicsScene::addItem(mouseDecor);
            }
            break;
        }
    }
    event->accept();
}

void MyGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{

    int a=  (event->delta() > 0) ? 1 : -1;
    int d = 10;
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        if (event->modifiers().testFlag(Qt::AltModifier)) {
            d *= 3;
        }
        myRender()->scale_slot(myRender()->getScale() + a*d,true);
    }
    event->accept();
}


void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (mouseMode != Normal) {
        mouseMode = Normal;
        myRender()->unsetCursor();
    }
    if ((currentMode.modename == MyMode::NormalMode) && mouseDecor->scene())
        QGraphicsScene::removeItem(mouseDecor);
    if (ismoving){
        endmove = event->scenePos();
        if (endmove != startmove){            
            move(mySelectedItems(),endmove-startmove);
        }
        ismoving = false;
    }         

    update();
    clearTrash();
}
void MyGraphicsScene::startSelectDrag(const QPointF & pos)
{
    Renderarea * ren = myRender();
    if (!ren) return;
    mouseMode = Select;
    mouseSelectAnchor = pos;
    ren->setCursor(Qt::CrossCursor);
}

void MyGraphicsScene::startPanning(const QPointF &pos)
{
    Renderarea * ren = myRender();
    if (!ren) return;
    mouseMode = Pan;
    ren->accessLastPan() = ren->mapFromScene(pos);
    ren->setCursor(Qt::ClosedHandCursor);
}

bool Add::operator <(const Add &other) const
{
    if (this->kind != other.kind) return (this->kind < other.kind);
    if (this->variation != other.variation)
        return (this->variation < other.variation);

    return false;
}

bool Add::operator ==(const Add &other) const
{
    return ( (this->kind == other.kind) && (this->variation == other.variation));
}

bool MyMode::operator ==(const MyMode &other) const
{
    return ( ( this->modename == other.modename) &&
             (this->add == other.add));
}



Qt::ItemFlags ShapeModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        if ( (index.column() == Label) || (index.column() == Visible)) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable
                    | Qt::ItemIsEditable;
        }
        else return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else return Qt::NoItemFlags;
}

QVariant ShapeModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()){
        MyShape * shape = (MyShape *) index.internalPointer();
        switch (role){
        case Qt::DisplayRole:{
                switch (index.column()){
                case Label:
                    return QVariant(shape->getLabel());
                case Visible:
                    return QVariant(QVariant(shape->isVisible()).toString());
                case Kind:
                    return QVariant(shape->getDescript());
                }
            }
            break;
        case Qt::DecorationRole: {
                if (index.column() == Visible){
                    if (shape->isVisible()){
                        return QVariant(QIcon(":/actions/package-reinstall.svg"));
                    }
                    else return QVariant(QIcon(":/actions/package-purge.svg"));
                }
            }
            break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant ShapeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( (orientation == Qt::Horizontal) && (role == Qt::DisplayRole) ){
        switch (section){
        case Label:
            return QVariant(tr("Label"));
        case Visible:
            return QVariant(tr("Visible"));
        case Kind:
            return QVariant(tr("Kind"));
        }
    }

    return QVariant();
}

QModelIndex ShapeModel::lookup(MyShape * shape) const
{
    if (displayMode == Tree) {
        if (shape) {
            QModelIndex ind(createIndex(shape->rankInParent(),0,shape));
            if (ind.isValid()) return ind;
        }
        return QModelIndex();
    }
    if (displayMode == List) {
        if (shape) {
            int i = scene->myItemsConst().indexOf(shape);
            QModelIndex ind(createIndex(i,0,shape));
            if (ind.isValid()) return ind;
        }
        return QModelIndex();
    }
}

QModelIndex ShapeModel::lookupParent(MyShape * shape) const
{
    if (displayMode == Tree) {
        if (shape) {
            if (shape->hasParent()){
                MyShape * parentShape = shape->myParent();
                return lookup(parentShape);
            }
        }
        return QModelIndex();
    }
    if (displayMode == List) {
        return QModelIndex();
    }
}

void ShapeModel::beginInsertShape(MyShape * shape)
{
    QModelIndex ind(lookupParent(shape));
    int place(rowCount(ind));
    if (place >= 0) {
        beginInsertRows(ind,place,place);
        addingShape = true;
    }
}

void ShapeModel::endInsertShape()
{
    if (addingShape) {
        endInsertRows();
        addingShape = false;
    }
}

void ShapeModel::beginRemoveShape(MyShape * shape)
{
    QModelIndex ind(lookupParent(shape));
    QModelIndex childInd = lookup(shape);
    if (childInd.isValid()) {
        int place = childInd.row();
        beginRemoveRows(ind,place,place);
        removingShape = true;
    }
}

void ShapeModel::endRemoveShape()
{
    if (removingShape)  {
        endRemoveRows();
        removingShape = false;
    }
}

void ShapeModel::changedShape(MyShape * shape)
{
    QModelIndex ind1(lookup(shape));
    if (ind1.isValid()) {
        QModelIndex ind2 = ind1.sibling(ind1.row(),NoOfHeaders-1);
        emit dataChanged(ind1,ind2);
    }
}

void ShapeModel::treeMode()
{
    if ( displayMode != Tree) {
         beginResetModel();
         displayMode = Tree;
         endResetModel();
    }
}

void ShapeModel::listMode()
{
    if ( displayMode != List) {
         beginResetModel();
         displayMode = List;
         endResetModel();
    }
}



ShapeModel::ShapeModel(MyGraphicsScene *Scene, QObject *parent)
    :QAbstractItemModel(parent)
{
    Q_ASSERT(Scene != 0);
    scene = Scene;
    displayMode = Tree;
    removingShape = false;
    addingShape = false;
}

int ShapeModel::rowCount(const QModelIndex &parent) const
{        
    if (parent.column() > 0)
        return 0;
    if (displayMode == Tree ) {
        if (parent.isValid()) {
            MyShape * parentItem = (MyShape *) parent.internalPointer();
            return parentItem->myChildren().size();
        }
        else{
            //parent is invalid if it is root
            return scene->topLevelItemsConst().size();
        }
    }
    if (displayMode == List) {

       if (parent.isValid()) return 0;
       else {
           return scene->myItemsConst().size();
       }

    }

}

int ShapeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.column()>0)  return 0;
    return NoOfHeaders;
}

QModelIndex ShapeModel::index(int row, int column, const QModelIndex &parent) const
{
    //hasIndex is just using rowCount and columnCount
    //if false, return root
    if (!hasIndex(row,column,parent)) return QModelIndex();

    //parent is invalid if it is root
    MyShape * shape = 0;
    if (displayMode ==Tree ) {
        if (!parent.isValid()) {
            shape=scene->topLevelItemsConst()[row];
        }
        else{
            MyShape * parentItem = (MyShape *) parent.internalPointer();
            shape = parentItem->myChildren()[row];
        }
        if (shape) return createIndex(row,column,shape);
        else return QModelIndex();
    }

    if (displayMode ==List) {
        if (parent.isValid()) return QModelIndex();
        else {
            shape = scene->myItemsConst()[row];
            if (shape) return createIndex(row,column,shape);
            else return QModelIndex();
        }
    }
}

QModelIndex ShapeModel::parent(const QModelIndex &child) const
{
    if (displayMode ==Tree ) {
        if (child.isValid()) {
            MyShape * shape = (MyShape *) child.internalPointer();
            if (shape && shape->hasParent()){
                MyShape * theParent = shape->myParent();
                return createIndex(theParent->rankInParent(),0,theParent);
            }
            else return QModelIndex();
        }
        else return QModelIndex();
    }
    if (displayMode ==List ) {
        return QModelIndex();
    }
}



