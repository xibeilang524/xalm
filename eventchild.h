#pragma once

#include <QtGui>
#include <mdichild.h>

class EventChild : public QTreeWidget, public MdiChild
{
    Q_OBJECT
public:
    EventChild(QWidget* parent = 0);
};
