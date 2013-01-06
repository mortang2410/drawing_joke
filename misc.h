#ifndef MISC_H
#define MISC_H
#include "smallfunctions.h"
class mylabel : public QLabel
{
public:
    mylabel(): QLabel() { setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed)); setMinimumSize(200,50);  }

};

//a button, when pushed, lets you choose a color
//it stores the color in mycolor
class MyChooseColorButton : public QPushButton
{
    Q_OBJECT
public:
    MyChooseColorButton(QWidget * parent=0);    
signals:
    void colorChanged(bool UsingSlider=false);
public slots:
    void chooseColor();        
    void slideChanged();
    //use this to update the color without emitting signals    
    void updateColor(QColor col);
    void updateColor(QColor col, qreal alpha);
    QColor color() {return mycolor;}
private:
    QMenu * theMenu;
    QWidget * sliderBigWidget;
    QHBoxLayout * sliderLayout;
    QLabel * sliderLabel;
    QSlider * alphaSlider;
    QWidgetAction * sliderAction;
    QAction * chooseColorAction;
    QColor mycolor;
    qreal alphaValue;
    bool updateMode;
};

//convenience functions
namespace Misc
{
    enum {iconsize=40};
    QPixmap createColorSquarePixmap(const QColor & color=Qt::white, const int & size=iconsize);
    QIcon createColorSquareIcon(const QColor & color=Qt::white, const int & size=iconsize);
    template<class from,class to>
    QSet<to> convertSet(QSet<from> set){
        QSet<to *> kq;
        foreach (from * pointer, set)
            if (to * newpointer = dynamic_cast<to *>(pointer))
                kq << newpointer;
        return kq;
    }    
};

#endif // MISC_H
