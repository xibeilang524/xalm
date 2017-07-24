#include <mdichild.h>

MdiChild::MdiChild(int type /*=Alarm*/) : mditype(type)
{
}

int MdiChild::mdiType() const
{
    return mditype;
}

QString MdiChild::friendlyTitle() const
{
    QString title;
    switch(mditype)
    {
    case Event:
        title = QString::fromLocal8Bit("实时事项");
        break;
    case Text:
        title = QString::fromLocal8Bit("文本编辑");
        break;
    case Alarm:
    default:
        title = QString::fromLocal8Bit("实时告警");
        break;
    }

    return title;
}
