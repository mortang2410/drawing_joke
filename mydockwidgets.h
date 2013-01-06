#ifndef MYTRANSFORMDOCKWIDGET_H
#define MYTRANSFORMDOCKWIDGET_H
#include "myshape.h"

//namespace Ui {
//class MyTransformDockWidget;
//class ShapeDock;
//}

class MySlider: public QSlider{
    Q_OBJECT
public:
    MySlider(QWidget * parent =0)
        :QSlider(parent)
    { connect(this,SIGNAL(valueChanged(int))
              ,this,SLOT(emitValueChangedDoubleFromInt(int))); }
signals:
    void valueChangedDouble(double val);
public slots:
    void emitValueChangedDoubleFromInt(int val)
    {emit valueChangedDouble(double(val)); }
    void setValueDouble(double val) { setValue( int(val));}
};

class ShapeModel;
class MyTreeDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    MyTreeDelegate(QObject * parent = 0);
    QWidget * createEditor(QWidget *parent
                           , const QStyleOptionViewItem &option
                           , const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
private:
    int yesInd;
    int noInd;
};

class MyTreeView : public QTreeView
{
    Q_OBJECT
public:
    MyTreeView(QWidget * parent = 0);
    void setScene(MyGraphicsScene * Scene);
    ShapeModel * myModel() const;
protected:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

public slots:
    void sceneSelectionChanged();
private:
    MyGraphicsScene * scene;
    bool updateMode;
};



#endif // MYTRANSFORMDOCKWIDGET_H
