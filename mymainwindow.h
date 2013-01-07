
#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H
#include "misc.h"
#include "mycentralwidget.h"
#include "renderarea.h"
#include "commands.h"
#include "mydialogs.h"
#include "mydockwidgets.h"
#include "mydockwidgets2.h"
#include "saveLoad.h"

//optimize later
class MyMainWindow : public QMainWindow
{
    Q_OBJECT
public:


    explicit MyMainWindow(QWidget *parent = 0);
    ~MyMainWindow();
    //can only run these current* functions when
    //there is at least a renderarea
    QMdiSubWindow * currentSubWindow() const{
        return mycentralwidget->currentSubWindow();
    }
    QStringList windowNameList() const;
    Renderarea * currentRenderarea() const {
        QMdiSubWindow * sub = mycentralwidget->currentSubWindow();
        return (sub == 0) ? 0 : (Renderarea *) sub->widget();
    }
    MyGraphicsScene * currentScene() const {
        QMdiSubWindow * sub = mycentralwidget->currentSubWindow();
        return (sub == 0) ? 0 : ((Renderarea *) sub->widget())->accessScene();
    }
    MyMode currentMode() const {
        MyGraphicsScene * scene = currentScene();
        return (scene == 0) ? MyMode() : currentScene()->currentMode;
    }
    //assume only 1 item is selected
    //return pointer to the selected item
    //else return 0;
    MyShape * onlySelectedItem() const {
        if (currentScene() == 0) return 0;
        if (currentScene()->selectedItems().size()==1)
            return  (MyShape *) currentScene()->selectedItems().first();
        else return 0;
    }
    Renderarea * lastRenderarea;
    //to let renderarea use stuff from mymainwindow
    friend class Renderarea;
public slots:
    void scaleSlot(int k);
    void renScaleChanged(int newval);
    void myabout();
    void newdocument();
    void showSceneContextMenu();
    void editThisTransform(MyTransform *  tran);
    void exportFile();
    void showGrid(bool Enabled);
    void showHide();
    void modified(bool state);
    void showTips(QString s=QString());
    void showFurtherTips(QString s=QString());
    void selectAll();
    void preference();

    void readSettings();
    void writeSettings();

    void changedSubwindow(QMdiSubWindow * sub);
    void changedRenderarea();
    void changedModes();
    void selectionChanged();
    //when user uses lineColor or brushColor
    void itemColorChanged(bool UsingSlider=false);
    void backgroundColorChanged(bool UsingSlider=false);
    //check if we can redo or undo
    void canRedoUndo();
    bool saveDialog();
    bool loadDialog();
    bool loadfile(QString filename);
    bool savefile(QString filename);
    bool save(bool *a = 0);
    void deleteItem();
    void openModifyDialog();
    void requestToAdd(bool state);
    void undo();
    void redo();        
    void debugslot() { qDebug()<<"what is dis";}
protected:
    void closeEvent(QCloseEvent * event);
private:
    bool eventFilter(QObject *, QEvent *);
    void createActions();
    void createToolbars();
    void createMenus();
    void createMenubar();
    void createDocks();
    void createStatusbar();
    void createDialogs();

    //setting up the zoombox and slide and tying them
    void zoomboxandslide();    


    QAction * aboutAction;
    QAction * newDocumentAction;
    QAction * saveAsAction;
    QAction * saveAction;
    QAction * loadAction;
    QAction * exportAction;
    QAction * quitAction;


    QAction * preferenceAction;    
    QAction * deleteAction;
    QAction * undoAction;
    QAction * redoAction;
    QAction * modifyAction;    
    QAction * setGridAction;

    QMenu * showHideMenu;
    QAction * hidePointsAction;
    QAction * showPointsAction;
    QAction * hideAction;
    QAction * showAllAction;

    QAction * selectAllAction;

    QAction * addPointAction;
    QMenu * addSegmentMenu;
    QAction * addSegmentAction;
    QAction * addLineAction;
    QAction * addRayAction;
    QAction * addRectAction;
    QAction * addPolygonAction;
    QMenu * addEllipseMenu;
    QAction * addCircleAction;
    QAction * addEllipseAction;
    QAction * addTransformAction;
    QAction * normalAction;
    QActionGroup * addActionGroup;

    QMap<Add, QAction*> mapAddActions;


    QMenu * fileMenu;
    QMenu * editMenu;
    QMenu * drawMenu;
    QMenu * aboutMenu;

    QMenuBar * myMenuBar;
    QDockWidget * dock1;
    QToolBar * bar1;
    QToolBar * bar2;
    QSpinBox * zoomBox;
    QSlider * zoomSlide;
    QLabel * modeName;
    QLabel * modeTips;
    MyChooseColorButton * lineColorButton;
    MyChooseColorButton * brushColorButton;
    MyChooseColorButton * backgroundColorButton;
    QUndoView * undoview;

    MyTransformDockWidget * transformDockWidget;
    MyShapeDock * shapeDock;
//my dialogs
    ModifyShapeDialog * modifyDialog;
    ExportDialog * exportDialog;

//showing scene coordinates
    mylabel * sceneCoor;
//central widget, a class based on qmdiarea
    MyCentralWidget * mycentralwidget;
    QSet<QAction*> dependentActions;
    bool subwindowPresent;
    PreferencesDialog * pref;
    QCommonStyle * style;
    QSettings * settings;
    bool updateMode;
};

#endif // MYMAINWINDOW_H
