#include "mycentralwidget.h"


MyCentralWidget::MyCentralWidget(QWidget *parent) :
    QMdiArea(parent)

{
    setViewMode(QMdiArea::TabbedView);
    setTabsMovable(true);
    setTabsClosable(true);
    setDocumentMode(true);
}

