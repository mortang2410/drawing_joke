#include "misc.h"



MyChooseColorButton::MyChooseColorButton(QWidget *parent)
    :QPushButton(parent)
{
    updateMode = true;
    chooseColorAction = new QAction(tr("Another color..."),this);    
    alphaSlider = new QSlider();
    alphaSlider->setRange(0,100);
    alphaSlider->setOrientation(Qt::Horizontal);    
    sliderLabel=new QLabel(QObject::tr("Opacity"));
    sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(sliderLabel);
    sliderLayout->addWidget(alphaSlider);
    sliderBigWidget = new QWidget;
    sliderBigWidget->setLayout(sliderLayout);
    sliderAction = new QWidgetAction(this);
    sliderAction->setDefaultWidget(sliderBigWidget);


    theMenu = new QMenu(this);
    theMenu->addAction(chooseColorAction);
    theMenu->addAction(sliderAction);
    setMenu(theMenu);

    alphaValue = 0.3;
    alphaSlider->setValue(alphaValue*100);
    updateColor(Qt::white);
    connect(alphaSlider,SIGNAL(valueChanged(int)),this,SLOT(slideChanged()));
    connect(chooseColorAction,SIGNAL(triggered()),this,SLOT(chooseColor()));
    updateMode = false;
}

void MyChooseColorButton::chooseColor() {
    updateMode = false;
    QColor newcolor = QColorDialog::getColor();
    if (newcolor.isValid()) {        
        updateColor(newcolor,alphaValue);
        emit colorChanged(false);
    }    
}
void MyChooseColorButton::updateColor(QColor col)
{
    updateColor(col,col.alphaF());
}
void MyChooseColorButton::updateColor(QColor col, qreal alpha)
{
    updateMode = true;
    mycolor = col;
    alphaValue = alpha;
    mycolor.setAlphaF(alphaValue);
    alphaSlider->setValue(alpha*100);
    setIcon(Misc::createColorSquareIcon(mycolor));
    updateMode = false;
}

void MyChooseColorButton::slideChanged()
{
    if (updateMode == true) return;
    alphaValue = alphaSlider->value()/100.0;
    mycolor.setAlphaF(alphaValue);
    setIcon(Misc::createColorSquareIcon(mycolor));
    emit colorChanged(true);
}


QPixmap Misc::createColorSquarePixmap(const QColor &color, const int &size)
{
    QPixmap pixmap(size,size);
    pixmap.fill(color);
    return pixmap;
}
QIcon Misc::createColorSquareIcon(const QColor &color, const int &size)
{
    return QIcon(createColorSquarePixmap(color,size));
}



