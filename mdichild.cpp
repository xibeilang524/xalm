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
        title = QString::fromLocal8Bit("ʵʱ����");
        break;
    case Text:
        title = QString::fromLocal8Bit("�ı��༭");
        break;
    case Alarm:
    default:
        title = QString::fromLocal8Bit("ʵʱ�澯");
        break;
    }

    return title;
}
