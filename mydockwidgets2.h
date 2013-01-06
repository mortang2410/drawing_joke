#ifndef MYDOCKWIDGETS2_H
#define MYDOCKWIDGETS2_H

#include "mydockwidgets.h"

#include "ui_mytransformdockwidget.h"
#include "ui_shapeDock.h"
class MyTransformDockWidget : public QDockWidget, private Ui::MyTransformDockWidget
{
    Q_OBJECT
public:
    explicit MyTransformDockWidget(QWidget *parent = 0);
    qreal getScale() const;
    qreal getRotation() const;
    void setScale(qreal newval);
    void setRotation(qreal newval);
    void setItem(MyTransform *  theitem);
    void doneEditing();
public slots:
    void applyTransform() const;
private:
    MyTransform *  item;
    bool isEditing;
};




class MyShapeDock : public QDockWidget, private Ui::ShapeDock
{
    Q_OBJECT
public:
    MyShapeDock(QWidget * parent =0);
    void setScene(MyGraphicsScene * Scene);
private:
    MyGraphicsScene * scene;
public slots:
    void treeMode();
    void listMode();
};




#endif // MYDOCKWIDGETS2_H
