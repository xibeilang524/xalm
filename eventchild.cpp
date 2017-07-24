#include <eventchild.h>

EventChild::EventChild(QWidget* parent/*=0*/) : QTreeWidget(parent), MdiChild(MdiChild::Event)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(friendlyTitle());
}
