#include <iostream>
#include <alarmevent.h>
#include <alarmchild.h>

using namespace std;

AlarmChild::AlarmChild(QWidget* parent/*=0*/)
    : QTreeWidget(parent), MdiChild(MdiChild::Alarm), m_menu(NULL)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(friendlyTitle());

    setStyleSheet("QTreeView{background-color:rgb(0,0,32); font:bold;font-size:16px}");

    clear();
    setHeaderLabels(QStringList() << QString::fromLocal8Bit("日期")
                    << QString::fromLocal8Bit("时间")
                    << QString::fromLocal8Bit("状态")
                    << QString::fromLocal8Bit("厂站")
                    << QString::fromLocal8Bit("描述"));
    setHeaderHidden(true);
    setRootIsDecorated(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);//允许用户菜单

    m_timer = new QTimer(this);
    m_timer->setInterval(600);
    m_timer->start();

    connect(m_timer, SIGNAL(timeout()), SLOT(updateSlot()));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(contextMenuSlot(const QPoint&)));
}

/// 调整列宽
void AlarmChild::fitColumns()
{
    for(int i=0; i<columnCount(); i++)
        resizeColumnToContents(i);
}

void AlarmChild::customEvent(QEvent* event)
{
    if (event->type() == QEvent::User)
    {
        const AlarmData& data = ((AlarmEvent*)event)->data();
        addNewAlarmData(data);
    }
}

void AlarmChild::addNewAlarmData(const AlarmData& data)
{
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, data.date().toString("yyyy-MM-dd"));
    item->setData(0, Qt::UserRole, qVariantFromValue(data));
    item->setText(1, data.date().toString(" hh:mm:ss "));
    item->setText(2, " * ");
    item->setText(3, data.station() + " ");
    item->setText(4, data.text());
    insertTopLevelItem(0, item);

    QTreeWidgetItem* bitem = itemBelow(item);
    if (bitem)
    {
        AlarmData data2 = bitem->data(0, Qt::UserRole).value<AlarmData>();
        if (data2.date().date() == data.date().date())
            bitem->setText(0, "");
    }

    fitColumns();
}

void AlarmChild::contextMenuSlot(const QPoint& pos)
{
    if (m_menu == NULL)
    {
        m_menu = new QMenu(this);
        m_menu->addAction(QIcon(),
                          QString::fromLocal8Bit("确认"),
                          this,
                          SLOT(affirmSlot()));
        m_menu->addSeparator();
        m_menu->addAction(QIcon(),
                          QString::fromLocal8Bit("删除..."),
                          this,
                          SLOT(removeSlot()));
    }

    Q_ASSERT(m_menu);
    m_menu->popup( mapToGlobal(pos));
}

void AlarmChild::updateSlot()
{
    static QColor turnColors[] = { QColor(255, 0, 0),
                                   QColor(128, 0, 0),
                                   QColor(64, 0, 0),
                                   QColor(32, 0, 0),
                                   QColor(64, 0, 0),
                                   QColor(128, 0, 0)
                                 };

    static int blink = -1;
    blink++;
    blink = blink%(sizeof(turnColors)/sizeof(QColor));

    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        QTreeWidgetItem* item = *it;
        AlarmData data = item->data(0, Qt::UserRole).value<AlarmData>();
        if (data.state() == AlarmData::WARNING)
        {
            item->setText(2, "*");
            item->setForeground(0, QBrush(QColor(192, 0, 0)));
            item->setForeground(1, QBrush(QColor(192, 0, 0)));
            item->setForeground(2, QBrush(turnColors[blink]));
            item->setForeground(3, QBrush(QColor(192, 0, 0)));
            item->setForeground(4, QBrush(QColor(192, 0, 0)));
        }
        else if (data.state() == AlarmData::OVERLIMIT1)
        {
            item->setText(2, "*");
            item->setForeground(0, QBrush(QColor(128, 0, 0)));
            item->setForeground(1, QBrush(QColor(128, 0, 0)));
            item->setForeground(2, QBrush(turnColors[blink]));
            item->setForeground(3, QBrush(QColor(128, 0, 0)));
            item->setForeground(4, QBrush(QColor(128, 0, 0)));
        }
        else if (data.state() == AlarmData::OVERLIMIT2)
        {
            item->setText(2, "*");
            item->setForeground(0, QBrush(QColor(225, 0, 0)));
            item->setForeground(1, QBrush(QColor(225, 0, 0)));
            item->setForeground(2, QBrush(turnColors[blink]));
            item->setForeground(3, QBrush(QColor(225, 0, 0)));
            item->setForeground(4, QBrush(QColor(225, 0, 0)));
        }
        else if (data.state() == AlarmData::REVERT)
        {
            item->setText(2, "*");
            item->setForeground(0, QBrush(QColor(0, 255, 0)));
            item->setForeground(1, QBrush(QColor(0, 255, 0)));
            item->setForeground(2, QBrush(turnColors[blink]));
            item->setForeground(3, QBrush(QColor(0, 255, 0)));
            item->setForeground(4, QBrush(QColor(0, 255, 0)));
        }
        else if (data.state() == AlarmData::AFFIRM)
        {
            item->setText(2, "");
            item->setForeground(0, QBrush(QColor(255, 255, 0)));
            item->setForeground(1, QBrush(QColor(255, 255, 0)));
            item->setForeground(2, QBrush(QColor(255, 255, 0)));
            item->setForeground(3, QBrush(QColor(255, 255, 0)));
            item->setForeground(4, QBrush(QColor(255, 255, 0)));
        }

        ++it;
    }
}

void AlarmChild::affirmSlot()
{
}

void AlarmChild::removeSlot()
{
}

