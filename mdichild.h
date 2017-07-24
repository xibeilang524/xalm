#pragma once

#include <QtGui>

class MdiChild
{
public:
    enum MDITYPE { Alarm, Event, Text, };

    MdiChild(int type = Alarm);
    int mdiType() const;
    virtual QString friendlyTitle() const;

private:
    int mditype;
};

