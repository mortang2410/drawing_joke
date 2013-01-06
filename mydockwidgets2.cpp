#include "mydockwidgets2.h"
#include "renderarea.h"
MyTransformDockWidget::MyTransformDockWidget(QWidget *parent)
    :QDockWidget(parent)
{
    setupUi(this);
    setWindowTitle(tr("Transform Editor"));
    connect(rotationSpinBox,SIGNAL(valueChanged(double)),
            rotationSlider,SLOT(setValueDouble(double)));
    connect(scaleSpinBox,SIGNAL(valueChanged(double)),
            scaleSlider,SLOT(setValueDouble(double)));
    connect(rotationSlider,SIGNAL(valueChangedDouble(double)),
            rotationSpinBox,SLOT(setValue(double)));
    connect(scaleSlider,SIGNAL(valueChangedDouble(double)),
            scaleSpinBox,SLOT(setValue(double)));

    connect(rotationSpinBox,SIGNAL(valueChanged(double)),
            this,SLOT(applyTransform()));
    connect(scaleSpinBox,SIGNAL(valueChanged(double)),
            this,SLOT(applyTransform()));
    isEditing = false;
    setEnabled(false);
    hide();
}

qreal MyTransformDockWidget::getScale() const
{
    return scaleSpinBox->value();
}

qreal MyTransformDockWidget::getRotation() const
{
    return rotationSpinBox->value();
}

void MyTransformDockWidget::setScale(qreal newval)
{
    scaleSpinBox->setValue(newval);
}

void MyTransformDockWidget::setRotation(qreal newval)
{
    rotationSpinBox->setValue(newval);
}


void MyTransformDockWidget::setItem(MyTransform *  theitem)
{
    isEditing = false;
    item = theitem;
    if (!isEnabled()) setEnabled(true);
    setScale( item->getMyScale() );
    setRotation(item->getMyAngle());
    if (!isVisible()) show();
    isEditing = true;
}

void MyTransformDockWidget::doneEditing()
{
    if (!isEditing) return;
    isEditing = false;
    item = 0;
    setEnabled(false);
    if (isVisible()) hide();
}


void MyTransformDockWidget::applyTransform() const
{
    if ( (isEditing) && (item != 0) ){
        if (item->scene() == 0) return;
        MyGraphicsScene * scene = (MyGraphicsScene *) item->scene();
        scene->transformOne(item,getRotation(),getScale());
    }
}

MyShapeDock::MyShapeDock(QWidget *parent)
    :QDockWidget(parent)
{
    setupUi(this);
    scene=0;
    setWindowTitle(tr("Shapes"));
    connect(treeButton,SIGNAL(clicked()),this,SLOT(treeMode()));
    connect(listButton,SIGNAL(clicked()),this,SLOT(listMode()));
}

void MyShapeDock::setScene(MyGraphicsScene *Scene)
{
    if (scene==Scene) return;
    scene = Scene;
    shapeTree->setScene(Scene);
}

void MyShapeDock::treeMode()
{
    if (shapeTree->myModel())
        shapeTree->myModel()->treeMode();
}

void MyShapeDock::listMode()
{
    if (shapeTree->myModel())
        shapeTree->myModel()->listMode();
}
