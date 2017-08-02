#include <iostream>
#include <alarmevent.h>
#include <remarkdlg.h>
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
                    << QString::fromLocal8Bit("组别")
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

/// 获取顶层窗口
QObject* AlarmChild::mainwin() const
{
    QObject* mw = parent();
    while(mw && qstrcmp(mw->metaObject()->className(), "MainWin"))
        mw = mw->parent();

    Q_ASSERT(mw);
    return mw;
}

/* 根据attrkey和时间定位指定的报警条目
**
** \param key 待查找的条目标识符
** \param dt 待查找的条目时间, 若无效则忽略该参数
** \return 第一个符号条件的条目或NULL
*/
QTreeWidgetItem* AlarmChild::findItem(const QString& key, const QDateTime& dt/*=QDateTime()*/)
{
    QTreeWidgetItem* item = NULL;
    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        AlarmData mydata = (*it)->data(0, Qt::UserRole).value<AlarmData>();
        if (mydata.attrKey() == key)
        {
	    if (!dt.isValid() || dt == mydata.dateTime())
	    {
                item = *it; // found
                break;
	    }
        }

        ++it;
    }

    return item;
}

/// 快捷菜单
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
        m_menu->addSeparator();
        m_menu->addAction(QIcon("images/remark.png"),
                          QString::fromLocal8Bit("标记..."),
                          this,
                          SLOT(remarkSlot()));
    }

    Q_ASSERT(m_menu);
    m_menu->popup( mapToGlobal(pos));
}

///定时器更新
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
            item->setForeground(0, QBrush(QColor(255, 0, 0)));
            item->setForeground(1, QBrush(QColor(255, 0, 0)));
            item->setForeground(2, QBrush(turnColors[blink]));
            item->setForeground(3, QBrush(QColor(255, 0, 0)));
            item->setForeground(4, QBrush(QColor(225, 0, 0)));
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
            item->setForeground(0, QBrush(QColor(192, 0, 0)));
            item->setForeground(1, QBrush(QColor(192, 0, 0)));
            item->setForeground(2, QBrush(turnColors[blink]));
            item->setForeground(3, QBrush(QColor(192, 0, 0)));
            item->setForeground(4, QBrush(QColor(192, 0, 0)));
        }
        else if (data.state() == AlarmData::REVERT)
        {
            item->setText(2, "*");

            // 事故特殊处理
            const AlarmMode* am = data.almMode();
            if (am->mode() == AlarmMode::ACCIDENT)
            {
                item->setForeground(0, QBrush(Qt::white));
                item->setForeground(1, QBrush(Qt::white));
                item->setForeground(2, QBrush(turnColors[blink]));
                item->setForeground(3, QBrush(Qt::white));
                item->setForeground(4, QBrush(Qt::white));
            }
            else
            {
                item->setForeground(0, QBrush(QColor(0, 255, 0)));
                item->setForeground(1, QBrush(QColor(0, 255, 0)));
                item->setForeground(2, QBrush(turnColors[blink]));
                item->setForeground(3, QBrush(QColor(0, 255, 0)));
                item->setForeground(4, QBrush(QColor(0, 255, 0)));
            }
        }
        else if (data.state() == AlarmData::AFFIRM)
        {
            item->setText(2, "");

            // 事故特殊处理
            const AlarmMode* am = data.almMode();
            if (am->mode() == AlarmMode::ACCIDENT)
            {
                item->setForeground(0, QBrush(Qt::magenta));
                item->setForeground(1, QBrush(Qt::magenta));
                item->setForeground(2, QBrush(Qt::magenta));
                item->setForeground(3, QBrush(Qt::magenta));
                item->setForeground(4, QBrush(Qt::magenta));
            }
            else
            {
                item->setForeground(0, QBrush(QColor(255, 255, 0)));
                item->setForeground(1, QBrush(QColor(255, 255, 0)));
                item->setForeground(2, QBrush(QColor(255, 255, 0)));
                item->setForeground(3, QBrush(QColor(255, 255, 0)));
                item->setForeground(4, QBrush(QColor(255, 255, 0)));
            }
        }
        else if (data.state() == AlarmData::ACCAFFIRM)
        {
            item->setText(2, "");

            // 事故特殊处理
            const AlarmMode* am = data.almMode();
            if (am->mode() == AlarmMode::ACCIDENT)
            {
                item->setForeground(0, QBrush(Qt::white));
                item->setForeground(1, QBrush(Qt::white));
                item->setForeground(2, QBrush(Qt::white));
                item->setForeground(3, QBrush(Qt::white));
                item->setForeground(4, QBrush(Qt::white));
            }
            else
            {
                item->setForeground(0, QBrush(QColor(255, 255, 0)));
                item->setForeground(1, QBrush(QColor(255, 255, 0)));
                item->setForeground(2, QBrush(QColor(255, 255, 0)));
                item->setForeground(3, QBrush(QColor(255, 255, 0)));
                item->setForeground(4, QBrush(QColor(255, 255, 0)));
            }
        }

        ++it;

        // 处理延时告警，超时删除
        QTime tm = item->data(0, Qt::UserRole + 1).toTime();
        if (tm.isValid() && QTime::currentTime() > tm)
            removeAlarmItem(data);
    }
}

/// 来自mainwin的用户自定义事件
void AlarmChild::customEvent(QEvent* event)
{
    AlarmEvent* ae = dynamic_cast<AlarmEvent*>(event);
    if (ae == NULL) return;

    switch(event->type())
    {
    case AlarmEvent::NEW:
        newAlarmItem(ae->data());
        break;
    case AlarmEvent::UPDATE:
        updateAlarmItem(ae->data());
        break;
    case AlarmEvent::REMOVE:
        removeAlarmItem(ae->data());
        break;
    default:
        break;
    }
}

///处理一条新告警
void AlarmChild::newAlarmItem(const AlarmData& data)
{
    const AlarmMode* am = data.almMode();

    if (am->mode() == AlarmMode::ACCIDENT || 
        data.state() == AlarmData::AFFIRM ||
        data.state() == AlarmData::ACCAFFIRM)
    {
        appendAlarmItem(data);
    }
    else if (data.state() == AlarmData::REVERT)
    {
        QTreeWidgetItem* item = findItem(data.attrKey());
	if (item == NULL)
            appendAlarmItem(data);
	else 
	{
	    AlarmData olddata = item->data(0, Qt::UserRole).value<AlarmData>();
	    const AlarmMode* oldam = olddata.almMode();
	    if (olddata.state() == AlarmData::AFFIRM)
	    {
                if (oldam->mode() == AlarmMode::ACCIDENT)//事故处理
		{
		    olddata.setState(AlarmData::ACCAFFIRM);
		    item->setData(0, Qt::UserRole, qVariantFromValue(olddata));
		    //appendAlarmItem(data);
		}
                else 
		    removeAlarmItem(olddata);
	    }
	    else 
	    {
                if (oldam->mode() == AlarmMode::ACCIDENT)//事故处理
		{
		    olddata.setState(AlarmData::REVERT);
		    item->setData(0, Qt::UserRole, qVariantFromValue(olddata));
		}
		else 
		    removeAlarmItem(olddata);

		if (oldam->mode() != AlarmMode::NOPERSIST && oldam->mode() != AlarmMode::DELAY)
		    appendAlarmItem(data);
	    }
	}
    }
    else //WARNING || OVERLIMIT1 || OVERLIMIT2 
    {
        QTreeWidgetItem* item = findItem(data.attrKey());
	if (item != NULL)
	{
	    AlarmData olddata = item->data(0, Qt::UserRole).value<AlarmData>();
	    const AlarmMode* oldam = olddata.almMode();
            if (oldam->mode() != AlarmMode::ACCIDENT)//仅非事故处理
	        removeAlarmItem(olddata);
	}

	appendAlarmItem(data);
    }
}

///新加一条新告警
void AlarmChild::appendAlarmItem(const AlarmData& data)
{
    const AlarmMode* am = data.almMode();

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, data.dateTime().toString("yyyy-MM-dd"));
    item->setData(0, Qt::UserRole, qVariantFromValue(data));
    if (am->mode() == AlarmMode::DELAY)
        item->setData(0, Qt::UserRole+1, QTime::currentTime().addSecs(am->delay()));

    item->setText(1, data.dateTime().toString(" hh:mm:ss.zzz "));
    item->setText(2, data.state() == AlarmData::AFFIRM ? " * ": "   ");
    item->setText(3, data.group() + " ");
    item->setText(4, data.text());

    if (!data.remark().isEmpty())
        item->setIcon(4, QIcon("images/remark.png"));

    insertTopLevelItem(0, item);

    // 更新年月日列
    QTreeWidgetItem* bitem = itemBelow(item);
    if (bitem)
    {
        AlarmData data2 = bitem->data(0, Qt::UserRole).value<AlarmData>();
        if (data2.dateTime().date() == data.dateTime().date())
            bitem->setText(0, "");
    }

    fitColumns();
}

///删除一条已存在告警
void AlarmChild::removeAlarmItem(const AlarmData& data)
{
    QTreeWidgetItem* item = findItem(data.attrKey(), data.dateTime());
    if (item == NULL) return;

    AlarmData olddata = item->data(0, Qt::UserRole).value<AlarmData>();
    const AlarmMode* am = olddata.almMode();

    // 删除非事故或确认过的已复归事故条目
    if ( am->mode() != AlarmMode::ACCIDENT ||
            (am->mode() == AlarmMode::ACCIDENT && data.state() == AlarmData::ACCAFFIRM))
    {
        QDateTime adt, bdt;
        QTreeWidgetItem* aitem = itemAbove(item);
        if (aitem)
        {
            AlarmData tmpdata = aitem->data(0, Qt::UserRole).value<AlarmData>();
            adt = tmpdata.dateTime();
        }

        QTreeWidgetItem* bitem = itemBelow(item);
        if (bitem)
        {
            AlarmData tmpdata = bitem->data(0, Qt::UserRole).value<AlarmData>();
            bdt = tmpdata.dateTime();
        }

        // 更新年月日列
        if (adt.date() != bdt.date() && bitem!=NULL)
            bitem->setText(0, bdt.toString("yyyy-MM-dd"));

        delete item;
    }
}

///更新一条告警状态, affirm/remark
void AlarmChild::updateAlarmItem(const AlarmData& newdata)
{
    QTreeWidgetItem* item = findItem(newdata.attrKey(), newdata.dateTime());
    if (item == NULL) return;

    AlarmData olddata = item->data(0, Qt::UserRole).value<AlarmData>();
    const AlarmMode* am = olddata.almMode();

cout << "aaaaaaaaaaa" << endl;
    //  处理标记 
    if (olddata.remark() != newdata.remark())
    {
        olddata.setRemark(newdata.remark());
	item->setData(0, Qt::UserRole, qVariantFromValue(newdata));
	if (!olddata.remark().isEmpty())
            item->setIcon(4, QIcon("images/remark.png"));
    }

    // 处理确认
    if (newdata.state() == AlarmData::AFFIRM || newdata.state() == AlarmData::ACCAFFIRM)
    {
        if (am->mode() == AlarmMode::ACCIDENT)//事故
        {
            olddata.setState(newdata.state()); //确认 
            item->setData(0, Qt::UserRole, qVariantFromValue(olddata)); //仅更新状态
        }
        else if (olddata.state() == AlarmData::REVERT) //删除
	{
            removeAlarmItem(olddata);
	}
        else // 更新
            item->setData(0, Qt::UserRole, qVariantFromValue(newdata));
    }
}

/// 确认告警
void AlarmChild::affirmSlot()
{
    QTreeWidgetItem* item = currentItem();
    if (item == NULL) return;

    AlarmData data = item->data(0, Qt::UserRole).value<AlarmData>();
    const AlarmMode* am = data.almMode();
    if (data.state() == AlarmData::AFFIRM || data.state() == AlarmData::ACCAFFIRM)
        return;

    if(am->mode() == AlarmMode::ACCIDENT && data.state() == AlarmData::REVERT)
        data.setState(AlarmData::ACCAFFIRM); // 复归事故确认
    else
        data.setState(AlarmData::AFFIRM); // 确认

    switch(am->mode())
    {
    case AlarmMode::ACCIDENT:
    case AlarmMode::PERSIST:
        QCoreApplication::postEvent(mainwin(), new AlarmEvent(data, (QEvent::Type)AlarmEvent::UPDATE));
        break;
    case AlarmMode::NOPERSIST:
    case AlarmMode::DELAY:
    default:
        QCoreApplication::postEvent(mainwin(), new AlarmEvent(data, (QEvent::Type)AlarmEvent::REMOVE));
        break;
    }
}

// 手工删除一条报警
void AlarmChild::removeSlot()
{
    QTreeWidgetItem* item = currentItem();
    if (item == NULL) return;

    AlarmData data = item->data(0, Qt::UserRole).value<AlarmData>();
    const AlarmMode* am = data.almMode();
    switch(am->mode())
    {
    case AlarmMode::ACCIDENT:
    {
        if (data.state() == AlarmData::ACCAFFIRM) //只有确认后才可删除
            QCoreApplication::postEvent(mainwin(), new AlarmEvent(data, (QEvent::Type)AlarmEvent::REMOVE));
    }
    break;
    case AlarmMode::PERSIST:
    case AlarmMode::NOPERSIST:
    case AlarmMode::DELAY:
    default:
        QCoreApplication::postEvent(mainwin(), new AlarmEvent(data, (QEvent::Type)AlarmEvent::REMOVE));
        break;
    }
}

///设置标记
void AlarmChild::remarkSlot()
{
    QTreeWidgetItem* item = currentItem();
    if (item == NULL) return;

    AlarmData data = item->data(0, Qt::UserRole).value<AlarmData>();

    RemarkDlg* dlg = new RemarkDlg(this);
    dlg->setRemark(data.remark());
    if (dlg->exec() != QDialog::Accepted) return;

    // 对话框长时间操作后，为避免要设置标注的条目别删除，这里重新定位条目
    item = findItem(data.attrKey(), data.dateTime());
    if (item == NULL)
    {
        QMessageBox::warning(this,
                             QString::fromLocal8Bit("提示"),
                             QString::fromLocal8Bit("该告警已删除!\n") + data.text());
        return;
    }

    data.setRemark(dlg->remark());
    QCoreApplication::postEvent(mainwin(), new AlarmEvent(data, (QEvent::Type)AlarmEvent::UPDATE));
}

