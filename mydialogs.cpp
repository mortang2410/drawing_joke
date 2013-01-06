#include "mydialogs.h"


ModifyShapeDialog::ModifyShapeDialog(QWidget *parent)
    :QDialog(parent)
{
    setupUi(this);

}
void ModifyShapeDialog::setItem(MyShape * item){
    myitem = item;
    mypen = myitem->getPen();
    mybrush = myitem->getBrush();
    chooseNewLineColorButton->updateColor(mypen.color());
    chooseNewBrushColorButton->updateColor(mybrush.color());
}

void ModifyShapeDialog::collectResult(){
    mypen.setWidth(linewidthbox->value());
    mypen.setColor(chooseNewLineColorButton->color());
    mybrush.setColor(chooseNewBrushColorButton->color());
}



void ModifyShapeDialog::chooseLineColor(){

}
void ModifyShapeDialog::chooseBrushColor(){

}


ExportDialog::ExportDialog(QWidget *parent)
    :QDialog(parent)
{
    setupUi(this);
    QStringList listOfTypes;
    foreach (QByteArray name, QImageWriter::supportedImageFormats())
        listOfTypes << name.data();
    listOfTypes << "SVG";
    fileTypeBox->insertItems(0,listOfTypes);
    connect(browseButton,SIGNAL(pressed()),this,SLOT(chooseFile()));
    connect(this,SIGNAL(accepted()),this,SLOT(saveFile()));
}


int ExportDialog::getX() const
{
    return spinBoxX->value();
}

int ExportDialog::getY() const
{
    return spinBoxY->value();
}

QString ExportDialog::getFileType() const
{
    return fileTypeBox->currentText();
}

void ExportDialog::setScene(QGraphicsScene *Scene)
{
    scene_ = Scene;    
    QRectF rect(scene_->sceneRect());
    spinBoxX->setValue((int) rect.width());
    spinBoxY->setValue((int) rect.height());    
}

void ExportDialog::chooseFile()
{
    QString name = QFileDialog::getSaveFileName(this);
    if ( !name.isNull()) fileNameLine->setText(name);
    QFileInfo info(name);
    int i = fileTypeBox->findText(info.suffix(),Qt::MatchFixedString);
    if (i!= -1 ) fileTypeBox->setCurrentIndex(i);
}

void ExportDialog::saveFile()
{    
    if ( (scene_ == 0) || fileNameLine->text().isNull()) return;
    bool a=false;
    QRectF rect(scene_->sceneRect());
    if (fileTypeBox->currentText() != "SVG"){
        QImage image(spinBoxX->value(),spinBoxY->value()
                     ,QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::white);
        QPainter painter(&image);
        scene_->render(&painter,QRectF(0,0,rect.width(),rect.height()));
        a = image.save(fileNameLine->text()
                           ,fileTypeBox->currentText().toLatin1().data());
    }
    else{
        QSvgGenerator image;
        image.setSize(QSize((int) rect.width(),(int) rect.height()));
        QFile file(fileNameLine->text());
        a = file.open(QIODevice::WriteOnly);
        image.setOutputDevice(&file);
        QPainter painter(&image);
        scene_->render(&painter,QRectF(0,0,rect.width(),rect.height()));
    }
    if (!a) QMessageBox::critical(0,tr("Error exporting"),tr("Could not export to file. Maybe the file can't be opened."));
}

PreferencesDialog::PreferencesDialog(QWidget *parent)
    :QDialog(parent)
{
    setupUi(this);
    setWindowTitle(tr("Preferences"));
    lang=EN;
    experimental = false;
    connect(defaultSettingsButton,SIGNAL(clicked()),
            this,SLOT(defaultSettings()));
}

void PreferencesDialog::defaultSettings(bool youSure)
{
    if (!youSure) {
        int ret = QMessageBox::question(0,tr("Are you sure?"),tr("All settings will be reverted to their default values. Is this what you want?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if ( ret == QMessageBox::No) return;
    }
    enButton->setChecked(true);
    experimentalBox->setChecked(false);
    emit settingsChanged();
}

