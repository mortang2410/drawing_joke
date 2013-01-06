#ifndef MYWIDGET_H
#define MYWIDGET_H
#include "renderarea.h"
class MyCentralWidget : public QMdiArea
{
    Q_OBJECT
public:
    explicit MyCentralWidget(QWidget *parent = 0);
private:

signals:
    void changedsize();
public slots:
    
};


#endif // MYWIDGET_H
