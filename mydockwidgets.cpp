#include "mydockwidgets.h"
#include "commands.h"





MyTreeView::MyTreeView(QWidget *parent)
    :QTreeView(parent)
{
    scene = 0;
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    updateMode = false;
    MyTreeDelegate * dele = new MyTreeDelegate(this);
    setItemDelegate(dele);
}


void MyTreeView::setScene(MyGraphicsScene *Scene)
{
    if (scene==Scene) return;
    if (scene) {
        scene->setTreeView(0);
    }

    scene = Scene;

    if (scene) {
        ShapeModel * model(scene->accessModel());        
        setModel(model);
        scene->setTreeView(this);
    }
    else setModel(0);
}

void MyTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QTreeView::selectionChanged(selected,deselected);
    if (updateMode) return;
    if (scene) scene->treeSelectionChanged();
}

ShapeModel *MyTreeView::myModel() const
{
    return (model() == 0) ? 0 : dynamic_cast<ShapeModel *>(model());
}

void MyTreeView::sceneSelectionChanged()
{
    updateMode = true;
    QItemSelectionModel * model = selectionModel();
    if (model)
    {
        model->clearSelection();
        foreach (QGraphicsItem * item, scene->selectedItems()) {
            QModelIndex ind = myModel()->lookup((MyShape *) item);
            if (ind.isValid()) {
                model->select(ind,QItemSelectionModel::Select);
            }
        }
    }
    updateMode = false;
}

QWidget *MyTreeDelegate::createEditor(QWidget *parent
                                      , const QStyleOptionViewItem &option
                                      , const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    switch (index.column()) {
    case ShapeModel::Label:
        {
            QLineEdit * edit = new QLineEdit(parent);
            edit->setFrame(false);
            return edit;
        }
    case ShapeModel::Visible:
        {
            QComboBox * box = new QComboBox(parent);
            box->insertItem(yesInd,tr("true","boxedit"));
            box->insertItem(noInd,tr("false","boxedit"));
            return box;
        }
    }
    return 0;
}

void MyTreeDelegate::setEditorData(QWidget *editor
                                   , const QModelIndex &index) const
{
    if (!index.isValid()) return;

    MyShape * shape = static_cast<MyShape*>(index.internalPointer());
    if (shape == 0) return;
    switch (index.column()) {
    case ShapeModel::Label:
        {
            QLineEdit * edit = static_cast<QLineEdit*>(editor);
            if (edit == 0)  return;
            edit->setText(shape->getLabel());
        }
        break;
    case ShapeModel::Visible:
        {
            QComboBox * box = static_cast<QComboBox*>(editor);
            if (box ==0 ) return;
            box->setCurrentIndex( shape->isVisible() ? yesInd : noInd );
        }
        break;
    }
}

void MyTreeDelegate::setModelData(QWidget *editor
                                  , QAbstractItemModel *model
                                  , const QModelIndex &index) const
{
    if (!index.isValid()) return;

    MyShape * shape = static_cast<MyShape*>(index.internalPointer());
    if (shape == 0) return;
    switch (index.column()) {
    case ShapeModel::Label:
        {
            QLineEdit * edit = static_cast<QLineEdit*>(editor);
            QString newlabel = edit->text();
            if ( (edit == 0) || (newlabel == shape->getLabel() )) return;
            if (shape->scene()) {
                MyGraphicsScene * thescene = static_cast<MyGraphicsScene*>(shape->scene());
                if (thescene) thescene->renameOne(shape,newlabel);
            }
        }
        break;
    case ShapeModel::Visible:
        {
            QComboBox * box = static_cast<QComboBox*>(editor);
            if (box ==0 ) return;
            if (shape->scene()) {
                MyGraphicsScene * thescene = static_cast<MyGraphicsScene*>(shape->scene());
                if (thescene == 0) return;
                bool a = (box->currentIndex() == yesInd );
                if (a!=shape->isVisible())
                    thescene->showHideOne(shape,a);
            }
        }
        break;
    }
}

void MyTreeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

MyTreeDelegate::MyTreeDelegate(QObject *parent)
    :QItemDelegate(parent)
{
    yesInd = 0;
    noInd  = 1;
}
