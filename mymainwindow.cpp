#include "mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowIcon(QIcon(":/myappicon.svg"));
    updateMode = false;

    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,QDir::currentPath());
    settings = new QSettings(QSettings::IniFormat,QSettings::UserScope,
                             "7sac","Drawing",this);

    qDebug() << settings->fileName();
    style = new QCommonStyle;
    zoomboxandslide();
    createActions();
    createToolbars();
    createMenus();
    createMenubar();
    createDocks();
    createDialogs();


    createStatusbar();
    mycentralwidget = new MyCentralWidget(this);
    setCentralWidget(mycentralwidget);
    lastRenderarea = 0;
    connect(mycentralwidget,SIGNAL(subWindowActivated(QMdiSubWindow*))
            ,this,SLOT(changedSubwindow(QMdiSubWindow*)));


    readSettings();
}

MyMainWindow::~MyMainWindow()
{
    delete style;
    shapeDock->setScene(0);
}

QStringList MyMainWindow::windowNameList() const
{
    QStringList list;
    foreach (QMdiSubWindow * sub, mycentralwidget->subWindowList())
        list << sub->windowTitle();
    return list;
}

void MyMainWindow::scaleSlot(int k)
{
    if (updateMode) return;
    Renderarea * ren = currentRenderarea();
    if (ren) ren->scale_slot(k,false);
}

void MyMainWindow::renScaleChanged(int newval)
{
    updateMode = true;
    zoomBox->setValue(newval);
    updateMode = false;
}



void MyMainWindow::createActions(){

     aboutAction = new QAction(tr("About Qt"),this);
     aboutAction->setIcon(QIcon(":/actions/help-about.svg"));
     connect(aboutAction,SIGNAL(triggered()),this,SLOT(myabout()));

     saveAction = new QAction(tr("Save"),this);
     connect(saveAction,SIGNAL(triggered()),this,SLOT(save()));
     saveAction->setShortcut(QKeySequence::Save);
     saveAction->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
     saveAsAction = new QAction(tr("Save as ..."),this);
     saveAsAction->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
     saveAsAction->setShortcut(QKeySequence::SaveAs);
     connect(saveAsAction,SIGNAL(triggered()),this,SLOT(saveDialog()));

     loadAction = new QAction(tr("Open ..."),this);
     loadAction->setIcon(QIcon(":/actions/document-import.svg"));
     loadAction->setShortcut(QKeySequence::Open);
     connect(loadAction,SIGNAL(triggered()),this,SLOT(loadDialog()));
     exportAction = new QAction(tr("Export..."),this);
     exportAction->setIcon(QIcon(":/actions/document-export.svg"));
     connect(exportAction,SIGNAL(triggered()),this,SLOT(exportFile()));
     newDocumentAction = new QAction(tr("New..."),this);
     connect(newDocumentAction,SIGNAL(triggered()),this,SLOT(newdocument()));
     newDocumentAction->setShortcut(QKeySequence::New);
     newDocumentAction->setIcon(QIcon(":/actions/document-new.svg"));
     quitAction = new QAction(tr("Quit"),this);
     quitAction->setShortcut(QKeySequence::Quit);
     quitAction->setIcon(QIcon(":/actions/application-exit.svg"));
     connect(quitAction,SIGNAL(triggered()),this,SLOT(close()));

     preferenceAction = new QAction(tr("Preferences"),this);
     preferenceAction->setIcon(QIcon(":/actions/system-run.svg"));
     connect(preferenceAction,SIGNAL(triggered()),this,SLOT(preference()));

     setGridAction = new QAction(tr("Show grid"),this);
     setGridAction->setCheckable(true);
     connect(setGridAction,SIGNAL(toggled(bool)),this,SLOT(showGrid(bool)));
     deleteAction = new QAction(tr("Delete"),this);
     deleteAction->setIcon(QIcon(":/actions/edit-delete.svg"));
     deleteAction->setShortcut(QKeySequence::Delete);
     connect(deleteAction,SIGNAL(triggered()),this,SLOT(deleteItem()));
     undoAction = new QAction(tr("Undo"),this);
     undoAction->setShortcut(QKeySequence::Undo);
     undoAction->setIcon( QIcon(":/actions/edit-undo.svg") );
     connect(undoAction,SIGNAL(triggered()),this,SLOT(undo()));    
     redoAction = new QAction(tr("Redo"),this);
     redoAction->setIcon(QIcon(":/actions/edit-redo.svg"));
     redoAction->setShortcut(QKeySequence::Redo);
     connect(redoAction,SIGNAL(triggered()),this,SLOT(redo()));


     hidePointsAction = new QAction(tr("Hide all points"),this);
     showPointsAction = new QAction(tr("Show all points"),this);
     hideAction =  new QAction(tr("Hide selected items"),this);
     hideAction->setShortcut(QKeySequence(tr("Ctrl+H")));
     showAllAction =  new QAction(tr("Show all hidden items"),this);
     connect(hidePointsAction,SIGNAL(triggered()),this,SLOT(showHide()));
     connect(showPointsAction,SIGNAL(triggered()),this,SLOT(showHide()));
     connect(hideAction,SIGNAL(triggered()),this,SLOT(showHide()));
     connect(showAllAction,SIGNAL(triggered()),this,SLOT(showHide()));

     selectAllAction = new QAction(tr("Select all items"),this);
     connect(selectAllAction,SIGNAL(triggered()),this,SLOT(selectAll()));
     selectAllAction->setShortcut(QKeySequence::SelectAll);
     selectAllAction->setIcon(QIcon(":/actions/edit-select-all.svg"));
     undoview=new QUndoView();          

     modifyAction = new QAction(tr("Modify shape..."),this);
     connect(modifyAction,SIGNAL(triggered()),this,SLOT(openModifyDialog()));
     lineColorButton = new MyChooseColorButton(this);
     lineColorButton->setText(tr("Line Color"));
     connect(lineColorButton,SIGNAL(colorChanged(bool))
             ,this,SLOT(itemColorChanged(bool)));
     brushColorButton = new MyChooseColorButton(this);
     brushColorButton->setText(tr("Brush Color"));
     connect(brushColorButton,SIGNAL(colorChanged(bool))
             ,this,SLOT(itemColorChanged(bool)));
     backgroundColorButton = new MyChooseColorButton(this);
     backgroundColorButton->setText(tr("Background Color"));
     connect(backgroundColorButton,SIGNAL(colorChanged(bool)),
             this,SLOT(backgroundColorChanged(bool)));

     addPointAction = new QAction(tr("Add point"),this);
     addPointAction->setIcon(QIcon(":/actions/point.svg"));
     addRectAction = new QAction(tr("Add rectangle"),this);     
     addRectAction->setIcon(QIcon(":/actions/rect.svg"));

     addPolygonAction = new QAction(tr("Add polygon"),this);
     addPolygonAction->setIcon(QIcon(":/actions/polygon.svg"));

     addEllipseAction = new QAction(tr("Add Ellipse"),this);         
     addEllipseAction->setIcon(QIcon(":/actions/ellipse.svg"));

     addLineAction = new QAction(tr("Add Line"),this);
     addLineAction->setIcon(QIcon(":/actions/line.svg"));
     addSegmentAction = new QAction(tr("Add Segment"),this);
     addSegmentAction->setIcon(QIcon(":/actions/segment.svg"));
     addRayAction = new QAction(tr("Add Ray"),this);
     addRayAction->setIcon(QIcon(":/actions/ray.svg"));
     addCircleAction=new QAction(tr("Add circle"), this);
     addCircleAction->setIcon(QIcon(":/actions/circle.svg"));
     addTransformAction = new QAction(tr("Add transform"),this);
     normalAction = new QAction(tr("Normal mode"), this);
     normalAction->setCheckable(true);

     addActionGroup = new QActionGroup(this);
     addActionGroup->addAction(normalAction);
     addActionGroup->addAction(addPointAction);
     addActionGroup->addAction(addCircleAction);
     addActionGroup->addAction(addEllipseAction);
     addActionGroup->addAction(addPolygonAction);
     addActionGroup->addAction(addRectAction);
     addActionGroup->addAction(addLineAction);
     addActionGroup->addAction(addRayAction);
     addActionGroup->addAction(addSegmentAction);
     addActionGroup->addAction(addTransformAction);


     mapAddActions.insert( Add(MyShape::Nothing),normalAction);
     mapAddActions.insert( Add(MyShape::Point),addPointAction);
     mapAddActions.insert(Add(MyShape::Rect,MyRect::Circle)
                          ,addCircleAction);
     mapAddActions.insert(Add(MyShape::Rect,MyRect::Ellipse)
                          ,addEllipseAction);
     mapAddActions.insert( Add(MyShape::Rect,MyRect::NormalRect)
                           ,addRectAction);
     mapAddActions.insert(Add(MyShape::Polygon),addPolygonAction);
     mapAddActions.insert(Add(MyShape::Segment,MySegment::Line)
                          ,addLineAction);
     mapAddActions.insert(Add(MyShape::Segment,MySegment::Ray),
                          addRayAction);
     mapAddActions.insert(Add(MyShape::Segment,MySegment::NormalSegment)
                          ,addSegmentAction);
     mapAddActions.insert(Add(MyShape::Transform),
                          addTransformAction);
     foreach (QAction * action, mapAddActions.values()) {         
             action->setCheckable(true);
             connect(action,SIGNAL(toggled(bool)),this,SLOT(requestToAdd(bool)));        
    }    

}
void MyMainWindow::createMenus(){
    aboutMenu= new QMenu(tr("About...") );
    aboutMenu->addAction(aboutAction);

    fileMenu= new QMenu(tr ("File"));
    fileMenu->addAction(newDocumentAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(exportAction);
    fileMenu->addAction(quitAction);

    showHideMenu = new QMenu(tr("Show/Hide"),this);
    showHideMenu->addAction(hidePointsAction);
    showHideMenu->addAction(showPointsAction);
    showHideMenu->addAction(hideAction);
    showHideMenu->addAction(showAllAction);

    editMenu= new QMenu(tr ("Edit"));
    editMenu->addAction(preferenceAction);
    editMenu->addAction(setGridAction);    
    editMenu->addAction(deleteAction);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addAction(selectAllAction);
    editMenu->addMenu(showHideMenu);


    addSegmentMenu = new QMenu(tr("Segment, lines,..."),this);    
    addEllipseMenu = new QMenu(tr("Ellipse, circle..."),this);    
    drawMenu = new QMenu(tr("Draw"),this);    

    addSegmentMenu->addAction(addSegmentAction);
    addSegmentMenu->addAction(addLineAction);
    addSegmentMenu->addAction(addRayAction);
    addEllipseMenu->addAction(addEllipseAction);
    addEllipseMenu->addAction(addCircleAction);
    drawMenu->addMenu(addSegmentMenu);
    drawMenu->addMenu(addEllipseMenu);
    drawMenu->addAction(addRectAction);
    drawMenu->addAction(addTransformAction);



    dependentActions += addActionGroup->actions().toSet()
            + editMenu->actions().toSet()
            + showHideMenu->actions().toSet();
    dependentActions.remove(preferenceAction);
    dependentActions << saveAsAction << saveAction
                        << exportAction;

}
void MyMainWindow::createToolbars(){
    bar1 = new QToolBar(tr("first bar"));
    bar1->setIconSize(QSize(35,35));
    bar1->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    bar1->addActions(addActionGroup->actions());

    bar1->addWidget(zoomBox);
    bar1->addWidget(zoomSlide);    



    bar2=new QToolBar(tr("Second bar"));
    bar2->addWidget(lineColorButton);
    bar2->addWidget(brushColorButton);
    bar2->addWidget(backgroundColorButton);
    addToolBar(bar1);
    addToolBarBreak();
    addToolBar(bar2);
}

void MyMainWindow::createMenubar(){
    myMenuBar= new QMenuBar;
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(drawMenu);        
    menuBar()->addMenu(aboutMenu);
}

void MyMainWindow::createDocks()
{
    dock1 = new QDockWidget(tr("Undo history"));
    dock1->setWidget(undoview);
    addDockWidget(Qt::RightDockWidgetArea,dock1,Qt::Vertical);

    transformDockWidget = new MyTransformDockWidget(this);
    addDockWidget(Qt::BottomDockWidgetArea,transformDockWidget,
                  Qt::Horizontal);
    transformDockWidget->doneEditing();

    shapeDock = new MyShapeDock(this);
    addDockWidget(Qt::LeftDockWidgetArea,shapeDock,
                  Qt::Vertical);
}

void MyMainWindow::createDialogs(){
    modifyDialog = new ModifyShapeDialog(this);
    exportDialog = new ExportDialog(this);
    pref = new PreferencesDialog(this);
    connect(pref,SIGNAL(settingsChanged()),this,SLOT(writeSettings()));
}




void MyMainWindow::createStatusbar(){
    sceneCoor = new mylabel;
    sceneCoor->setText("bat dau");

    modeName = new QLabel(this);
    modeName->setWordWrap(true);
    modeTips = new QLabel(this);
    modeTips->setWordWrap(true);
    statusBar()->addWidget(sceneCoor);
    statusBar()->addWidget(modeName);
    statusBar()->addWidget(modeTips);
}

void MyMainWindow::zoomboxandslide(){
    zoomBox = new QSpinBox;
    zoomSlide = new QSlider(Qt::Horizontal);
    QPushButton* mybutton = new QPushButton(QObject::tr("cai chi mo"));
    zoomBox->setRange(1, 1000);
    zoomBox->setSuffix(" %");
    zoomSlide->setRange(1, 1000);
    QObject::connect(zoomBox, SIGNAL(valueChanged(int)),
                     zoomSlide, SLOT(setValue(int)));
    QObject::connect(zoomSlide, SIGNAL(valueChanged(int)),
                     zoomBox, SLOT(setValue(int)));
    zoomBox->setValue(100);
    connect(zoomBox,SIGNAL(valueChanged(int)),this,SLOT(scaleSlot(int)));
}

void MyMainWindow::newdocument(){
    Renderarea* ren= new Renderarea();
    QMdiSubWindow * sub = new QMdiSubWindow;
    sub->setWidget(ren);
    sub->setAttribute(Qt::WA_DeleteOnClose);

    if (mycentralwidget->addSubWindow(sub) == 0) return;
    ren->setSubwin(sub);
    sub->show();

    connect(ren,SIGNAL(scaleChanged(int))
            ,this,SLOT(renScaleChanged(int)));
    connect(ren,SIGNAL(savePls(bool*)),this,SLOT(save(bool*)));

    MyGraphicsScene * scene = ren->accessScene();
    if (scene == 0) return;

    connect(scene,
            SIGNAL(changedModes()),this,SLOT(changedModes()));
    scene->installEventFilter(this);
    connect(scene,
            SIGNAL(needContextMenu()),this,SLOT(showSceneContextMenu()));
    connect(scene,SIGNAL(selectionChanged()),
            this,SLOT(selectionChanged()));
    selectionChanged();
    connect(scene->undoStack,SIGNAL(canRedoChanged(bool)),
            this,SLOT(canRedoUndo()));
    connect(scene->undoStack,SIGNAL(canUndoChanged(bool)),
            this,SLOT(canRedoUndo()));
    connect(scene,SIGNAL(editThisTransform(MyTransform * )),
            this,SLOT(editThisTransform(MyTransform * )));
    connect(scene,SIGNAL(showTips(QString)),this,SLOT(showTips(QString)));
    connect(scene,SIGNAL(modified(bool)),this,SLOT(modified(bool)));
    scene->setClean();
    QString title; int i = 0;
    do {
        title = "[*]Untitled" + giveName(i) ;
        ++i;
    }
    while ( windowNameList().contains(title));
    sub->setWindowTitle(title);
    changedRenderarea();
    changedModes();
}

void MyMainWindow::showSceneContextMenu()
{
    //this menu will die when this function finishes.
    //No big deal.
    QMenu menu(this);
    menu.addMenu(editMenu);
    menu.exec(QCursor::pos());
}

void MyMainWindow::editThisTransform(MyTransform *  tran)
{
    MyGraphicsScene * scene = (MyGraphicsScene *) sender();
    if (scene == 0) return;
    transformDockWidget->setItem(tran);
    scene->setMode(MyMode(MyMode::EditTransformMode
                          ,Add(MyShape::Transform)));
}

void MyMainWindow::exportFile()
{
    MyGraphicsScene * scene =  currentScene();
    if (scene ==0) return;
    exportDialog->setScene(scene);
    exportDialog->exec();
}

void MyMainWindow::showGrid(bool Enabled)
{
    if (currentRenderarea())
        currentRenderarea()->setGrid(Enabled);
}

void MyMainWindow::showHide()
{
    //warning: unreadable code
    MyGraphicsScene * scene = currentScene();
    if ( (scene == 0) || scene->items().isEmpty()) return;
    switch ( ( (sender()==hidePointsAction )<< 3)
             +((sender() == showPointsAction) << 2 )
             +((sender() == hideAction ) << 1)
             +((sender() == showAllAction)))
    {
    case 8:
        {
            QList<MyShape *> list;
            foreach (MyShape * shape, scene->myItemsConst())
                if (shape->isPoint()) list << shape;
            scene->showHideOne(list,false);
        }
        break;
    case 4:
        {
            QList<MyShape *> list;
            foreach (MyShape * shape, scene->myItemsConst())
                if (shape->isPoint()) list << shape;
            scene->showHideOne(list,true);
        }
    case 2:
        {
            scene->showHideOne(scene->mySelectedItems(),false);
        }
        break;
    case 1:
        {
            scene->showHideOne(scene->myItemsConst(),true);
        }
    }
}

void MyMainWindow::modified(bool state)
{
    MyGraphicsScene * scene = (MyGraphicsScene *) sender();
    QMdiSubWindow * sub = (scene) ? scene->myRender()->accessSubwin()
                                       : 0;
    if (sub) sub->setWindowModified(state);
    sub = (currentSubWindow()) ? currentSubWindow() : 0;
    saveAction->setEnabled( (sub) ? sub->isWindowModified() : false);
}

void MyMainWindow::showTips(QString s)
{
    modeTips->setText(s);
}

void MyMainWindow::showFurtherTips(QString s)
{
    modeTips->setText(modeTips->text() + " " + s);
}

void MyMainWindow::selectAll()
{
    if (currentScene()) foreach (QGraphicsItem * item, currentScene()->items())
        item->setSelected(true);
}

void MyMainWindow::preference()
{
    bool a= pref->exec();
    if (a) writeSettings();
    readSettings();
}

void MyMainWindow::readSettings()
{

    int lang = settings->value("Language",PreferencesDialog::EN).toInt();
    switch (lang) {
    case PreferencesDialog::EN:
        pref->enButton->setChecked(true); break;
    case PreferencesDialog::VN:
        pref->vnButton->setChecked(true); break;
    }
    bool exp = settings->value("Experimental",false).toBool();
    pref->experimentalBox->setChecked(exp);
    if (exp) {
        addActionGroup->addAction(addTransformAction);
    }
    else {
        addActionGroup->removeAction(addTransformAction);
    }
    addTransformAction->setCheckable(exp);
    addTransformAction->setEnabled(exp);
    addTransformAction->setVisible(exp);
}

void MyMainWindow::writeSettings()
{
    PreferencesDialog::Lang lang;
    if (pref->enButton->isChecked()) lang = PreferencesDialog::EN;
    if (pref->vnButton->isChecked()) lang = PreferencesDialog::VN;
    settings->setValue("Language",lang);
    settings->setValue("Experimental",pref->experimentalBox->isChecked());
}

void MyMainWindow::changedSubwindow(QMdiSubWindow *sub)
{
    if (subwindowPresent != (sub !=0)) {
        subwindowPresent = (sub !=0);
        foreach (QAction * action, dependentActions)
            action->setEnabled(subwindowPresent);
    }
    changedRenderarea();
}


void MyMainWindow::changedModes() {
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return;
    MyMode themode(scene->currentMode);
    QAction * theaction = mapAddActions[themode.add];
    if (theaction == 0) return;
     if (!theaction->isChecked()) theaction->setChecked(true);
    if (theaction) modeName->setText(theaction->text());
    modeTips->setText("");

//    foreach (QAction * action, mapAddActions.values()){
//        if (action &&  (action != mapAddActions[themode.add]) && (action->isChecked()))
//           action->setChecked(false);
//    }
    if (themode.modename != MyMode::EditTransformMode)
        transformDockWidget->doneEditing();

    if (themode.modename == MyMode::AddMode){
        switch (themode.add.kind) {
        case MyShape::Transform:
            {
                char * tip = "Left-click to select more items. Right-click to choose the point of origin for the transformation.";
                showTips(tr(tip));
            }
            break;
        case MyShape::Polygon:
            showTips(tr("Left-click to add points. Right-click to close the polygon."));
            break;
        case MyShape::Rect:
            bool a = (themode.add.variation == MyRect::NormalRect);
            bool b = (themode.add.variation == MyRect::Ellipse);
            if (a || b) {
                QString s = tr("Hold shift while choosing the second point to create a ");
                if (a) s += tr("square. ");
                if (b) s += tr("circle. ");
                showTips(s);
            }
            break;
        }
    }
    if (themode.modename == MyMode::NormalMode){
        showTips(tr("Hold left button and drag to select multiple items. Ctrl + left-click to select extra items. Shift + left-click to pan."));
    }
    else {
        showFurtherTips(tr(" Press ESC to go back to normal mode."));
    }

}

void MyMainWindow::selectionChanged()
{    
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return;
    if (currentScene()->selectedItems().size() == 0) {
        modifyAction->setEnabled(false);
        deleteAction->setEnabled(false);
    }
    else {
        modifyAction->setEnabled(true);
        deleteAction->setEnabled(true);
    }

}

void MyMainWindow::itemColorChanged(bool UsingSlider)
{
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return;
    if (sender() == lineColorButton)
        scene->currentPen.setColor(lineColorButton->color());
    if (sender() == brushColorButton)
        scene->currentBrush.setColor(brushColorButton->color());
    if (scene->selectedItems().size())
    {
        QList<QGraphicsItem *> list(scene->selectedItems());
        QList<MyShape *> listFiltered;
        foreach (QGraphicsItem * item, list) {
            MyShape * shape = (MyShape *) item;
            if ( !shape->isPoint()) listFiltered << shape;
        }
        if (sender() == lineColorButton)
        {            
            scene->undoStack->push(new ModifyShapeCommand(
                                       listFiltered,QPen(lineColorButton->color()),
                                       QBrush()
                                       ,UsingSlider,true,false));

        }
        if (sender() == brushColorButton)
        {
            scene->undoStack->push(new ModifyShapeCommand(
                                       listFiltered,QPen(),
                                       QBrush(brushColorButton->color())
                                       ,UsingSlider,false,true));
        }        
    }
}

void MyMainWindow::backgroundColorChanged(bool UsingSlider)
{
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return;
    scene->undoStack->push(new BGChangeCommand(currentScene(),
                                               backgroundColorButton->color(),
                                               UsingSlider));
}

void MyMainWindow::canRedoUndo()
{    
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return;
    if (scene->undoStack->canRedo())
        redoAction->setEnabled(true);
    else redoAction->setEnabled(false);
    if (scene->undoStack->canUndo())
        undoAction->setEnabled(true);
    else undoAction->setEnabled(false);
}

bool MyMainWindow::saveDialog()
{
    QString filename = QFileDialog::getSaveFileName(this);
    if (filename.isEmpty()) {
        QMessageBox::warning(this,tr("Error"),tr("Filename is empty."));
        return false;
    }
    return savefile(filename);
}

bool MyMainWindow::loadDialog()
{
    QString filename = QFileDialog::getOpenFileName(this);
    return loadfile(filename);
}

bool MyMainWindow::loadfile(QString filename)
{
    newdocument();
    QMdiSubWindow * sub = currentSubWindow();
    if (sub == 0) return false;
    MyReader reader;
    bool success = reader.loadFrom(filename,currentScene());
    if (!success) {
        sub->close();
        return false;
    }
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return false;
    scene->accessFile().setFileName(filename);
    QFileInfo info(filename);
    sub->setWindowTitle("[*]"+info.fileName());
    scene->setClean();
    changedRenderarea();
}

bool MyMainWindow::savefile(QString filename)
{
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return false;
    MyWriter writer;
    bool a = writer.saveTo(filename,currentScene());
    if (!a) return a;
    scene->accessFile().setFileName(filename);
    QFileInfo info(filename);

    if (currentSubWindow())
        currentSubWindow()->setWindowTitle("[*]"+info.fileName());
    scene->setClean();
    return a;
}

bool MyMainWindow::save(bool *a)
{
    bool b;
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return ( (a) ?  (*a = false) : false);
    QFile file(scene->accessFile().fileName());
    if (file.exists()) b= savefile(file.fileName());
    else b= saveDialog();
    return ( (a) ?  (*a = b) : b);
}

void MyMainWindow::changedRenderarea() {
    Renderarea * render= currentRenderarea();
    if (lastRenderarea == render) return;
    MyGraphicsScene * scene = (render !=0) ? render->accessScene() : 0;
    if (scene) {
        scene->setMode(MyMode());
        undoview->setStack(scene->undoStack);
        lineColorButton->updateColor(scene->currentPen.color());
        brushColorButton->updateColor(scene->currentBrush.color());
        backgroundColorButton->updateColor(scene->backgroundBrush().color());
    }
    if (render) {
        zoomBox->setValue(render->getScale());
        setGridAction->setChecked(render->isGridEnabled());
    }

    shapeDock->setScene(scene);
    if (currentSubWindow()) {
        saveAction->setEnabled(currentSubWindow()->isWindowModified());
    }
    canRedoUndo();
    lastRenderarea=render;    
}


bool MyMainWindow::eventFilter(QObject * object, QEvent * event){

    if ((object->objectName()=="myownscene" ) &&(event->type()== QEvent::GraphicsSceneMouseMove) ) {
        QGraphicsSceneMouseEvent *myevent = (QGraphicsSceneMouseEvent * ) event ;
        sceneCoor->setText( QString("%1,%2").arg(myevent->scenePos().x() )
                 .arg(-myevent->scenePos().y()) ) ;
    }
    return QMainWindow::eventFilter(object, event);
}




void MyMainWindow::myabout(){
    QMessageBox::aboutQt(this,tr("About Qt"));
}

void MyMainWindow::deleteItem() {
    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return;
    scene->undoStack->beginMacro(tr("Delete stuff"));
    foreach (QGraphicsItem * item, currentScene()->selectedItems()) {
        MyShape * shape = (MyShape *) item;
        if (shape->scene()) scene->removeItemMacro(shape);
    }
    scene->undoStack->endMacro();
}

void MyMainWindow::undo(){    
    MyGraphicsScene * scene = currentScene();
    if (scene) {
        scene->undoStack->undo();
        scene->clearDecor();
        scene->clearBuffer();
    }
}

void MyMainWindow::redo(){
    MyGraphicsScene * scene = currentScene();
    if (scene) {
        scene->undoStack->redo();
        scene->clearDecor();
        scene->clearBuffer();
    }
}

void MyMainWindow::closeEvent(QCloseEvent * event)
{
    bool a = false;
    foreach (QMdiSubWindow * sub, mycentralwidget->subWindowList()) {
        if (sub->isWindowModified()) {a = true; break;}
    }
    if (a) {
        QMessageBox msgBox;
        msgBox.setText(tr("Are you sure you want to quit?"));
        msgBox.setInformativeText(tr("There are unsaved windows. Do you want to discard all the changes you've made?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes:
            writeSettings();
            event->accept();
            break;
        case QMessageBox::No:
            event->ignore();
            break;
        }
        return;
    }
    writeSettings();
    event->accept();
}

void MyMainWindow::openModifyDialog(){    
//    if (item){
//        modifyDialog->setItem(item);
//        if (modifyDialog->exec()) {
//            modifyDialog->collectResult();
//            ModifyShapeCommand * command = new ModifyShapeCommand(
//                        item,
//                        modifyDialog->mypen,
//                        modifyDialog->mybrush);
//            currentScene()->undoStack->push(command);
//        }
//    }
//    else qDebug() << "0 items selected";
}


void MyMainWindow::requestToAdd(bool state)
{

    MyGraphicsScene * scene = currentScene();
    if (scene == 0) return;
    Add whatToAdd = mapAddActions.key((QAction *) sender());
    if (state) {
        if (whatToAdd==Add() ) scene->setMode(MyMode());
        else scene->setMode(MyMode(MyMode::AddMode,whatToAdd));
    }
}



