#ifndef MYDIALOGS_H
#define MYDIALOGS_H
#include "myshape.h"
#include "misc.h"
#include "ui_ModifyDialog.h"
#include "ui_exportdialog.h"
#include "ui_preference.h"
//the dialog stores the new pen and brush
//the dialog only hides, never dies

class ModifyShapeDialog : public QDialog, private Ui::ModifyDialog
{
    Q_OBJECT
public:    
    ModifyShapeDialog(QWidget * parent = 0);
    //get information from item
    void setItem(MyShape * item);
    //gets the result from the child widgets
    //usually after the user pushes OK
    void collectResult();
    QPen mypen;
    QBrush mybrush;
public slots:
    void chooseLineColor();
    void chooseBrushColor();
private:
    MyShape * myitem;

};


namespace Ui {
class ExportDialog;
class Preferences;
}

class ExportDialog : public QDialog, private Ui::ExportDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = 0);
    int getX() const;
    int getY() const;
    QString getFileType() const;
    void setScene(QGraphicsScene * Scene);
public slots:
    void chooseFile();
    void saveFile();
private:
    QGraphicsScene * scene_;
};

class PreferencesDialog : public QDialog, public Ui::Preferences
{
    Q_OBJECT
public:
    enum Lang { EN, VN};
    explicit PreferencesDialog(QWidget *parent = 0);

    Lang lang;
    bool experimental;
signals:
    void settingsChanged();
public slots:
    void defaultSettings(bool youSure = false);
private:

};

#endif // MYDIALOGS_H
