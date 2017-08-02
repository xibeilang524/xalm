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
    setHeaderLabels(QStringList() << QString::fromLocal8Bit("����")
                    << QString::fromLocal8Bit("ʱ��")
                    << QString::fromLocal8Bit("״̬")
                    << QString::fromLocal8Bit("���")
                    << QString::fromLocal8Bit("����"));
    setHeaderHidden(true);
    setRootIsDecorated(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);//�����û��˵�

    m_timer = new QTimer(this);
    m_timer->setInterval(600);
    m_timer->start();

    connect(m_timer, SIGNAL(timeout()), SLOT(updateSlot()));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(contextMenuSlot(const QPoint&)));
}

/// �����п�
void AlarmChild::fitColumns()
{
    for(int i=0; i<columnCount(); i++)
        resizeColumnToContents(i);
}

/// ��ȡ���㴰��
QObject* AlarmChild::mainwin() const
{
    QObject* mw = parent();
    while(mw && qstrcmp(mw->metaObject()->className(), "MainWin"))
        mw = mw->parent();

    Q_ASSERT(mw);
    return mw;
}

/* ����attrkey��ʱ�䶨λָ���ı�����Ŀ
**
** \param key �����ҵ���Ŀ��ʶ��
** \param dt �����ҵ���Ŀʱ��, ����Ч����Ըò���
** \return ��һ��������������Ŀ��NULL
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

/// ��ݲ˵�
void AlarmChild::contextMenuSlot(const QPoint& pos)
{
    if (m_menu == NULL)
    {
        m_menu = new QMenu(this);
        m_menu->addAction(QIcon(),
                          QString::fromLocal8Bit("ȷ��"),
                          this,
                          SLOT(affirmSlot()));
        m_menu->addSeparator();
        m_menu->addAction(QIcon(),
                          QString::fromLocal8Bit("ɾ��..."),
                          this,
                          SLOT(removeSlot()));
        m_menu->addSeparator();
        m_menu->addAction(QIcon("images/remark.png"),
                          QString::fromLocal8Bit("���..."),
                          this,
                          SLOT(remarkSlot()));
    }

    Q_ASSERT(m_menu);
    m_menu->popup( mapToGlobal(pos));
}

///��ʱ������
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

            // �¹����⴦��
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

            // �¹����⴦��
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

            // �¹����⴦��
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

        // ������ʱ�澯����ʱɾ��
        QTime tm = item->data(0, Qt::UserRole + 1).toTime();
        if (tm.isValid() && QTime::currentTime() > tm)
            removeAlarmItem(data);
    }
}

/// ����mainwin���û��Զ����¼�
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

///����һ���¸澯
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
                if (oldam->mode() == AlarmMode::ACCIDENT)//�¹ʴ���
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
                if (oldam->mode() == AlarmMode::ACCIDENT)//�¹ʴ���
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
            if (oldam->mode() != AlarmMode::ACCIDENT)//�����¹ʴ���
	        removeAlarmItem(olddata);
	}

	appendAlarmItem(data);
    }
}

///�¼�һ���¸澯
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

    // ������������
    QTreeWidgetItem* bitem = itemBelow(item);
    if (bitem)
    {
        AlarmData data2 = bitem->data(0, Qt::UserRole).value<AlarmData>();
        if (data2.dateTime().date() == data.dateTime().date())
            bitem->setText(0, "");
    }

    fitColumns();
}

///ɾ��һ���Ѵ��ڸ澯
void AlarmChild::removeAlarmItem(const AlarmData& data)
{
    QTreeWidgetItem* item = findItem(data.attrKey(), data.dateTime());
    if (item == NULL) return;

    AlarmData olddata = item->data(0, Qt::UserRole).value<AlarmData>();
    const AlarmMode* am = olddata.almMode();

    // ɾ�����¹ʻ�ȷ�Ϲ����Ѹ����¹���Ŀ
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

        // ������������
        if (adt.date() != bdt.date() && bitem!=NULL)
            bitem->setText(0, bdt.toString("yyyy-MM-dd"));

        delete item;
    }
}

///����һ���澯״̬, affirm/remark
void AlarmChild::updateAlarmItem(const AlarmData& newdata)
{
    QTreeWidgetItem* item = findItem(newdata.attrKey(), newdata.dateTime());
    if (item == NULL) return;

    AlarmData olddata = item->data(0, Qt::UserRole).value<AlarmData>();
    const AlarmMode* am = olddata.almMode();

cout << "aaaaaaaaaaa" << endl;
    //  ������ 
    if (olddata.remark() != newdata.remark())
    {
        olddata.setRemark(newdata.remark());
	item->setData(0, Qt::UserRole, qVariantFromValue(newdata));
	if (!olddata.remark().isEmpty())
            item->setIcon(4, QIcon("images/remark.png"));
    }

    // ����ȷ��
    if (newdata.state() == AlarmData::AFFIRM || newdata.state() == AlarmData::ACCAFFIRM)
    {
        if (am->mode() == AlarmMode::ACCIDENT)//�¹�
        {
            olddata.setState(newdata.state()); //ȷ�� 
            item->setData(0, Qt::UserRole, qVariantFromValue(olddata)); //������״̬
        }
        else if (olddata.state() == AlarmData::REVERT) //ɾ��
	{
            removeAlarmItem(olddata);
	}
        else // ����
            item->setData(0, Qt::UserRole, qVariantFromValue(newdata));
    }
}

/// ȷ�ϸ澯
void AlarmChild::affirmSlot()
{
    QTreeWidgetItem* item = currentItem();
    if (item == NULL) return;

    AlarmData data = item->data(0, Qt::UserRole).value<AlarmData>();
    const AlarmMode* am = data.almMode();
    if (data.state() == AlarmData::AFFIRM || data.state() == AlarmData::ACCAFFIRM)
        return;

    if(am->mode() == AlarmMode::ACCIDENT && data.state() == AlarmData::REVERT)
        data.setState(AlarmData::ACCAFFIRM); // �����¹�ȷ��
    else
        data.setState(AlarmData::AFFIRM); // ȷ��

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

// �ֹ�ɾ��һ������
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
        if (data.state() == AlarmData::ACCAFFIRM) //ֻ��ȷ�Ϻ�ſ�ɾ��
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

///���ñ��
void AlarmChild::remarkSlot()
{
    QTreeWidgetItem* item = currentItem();
    if (item == NULL) return;

    AlarmData data = item->data(0, Qt::UserRole).value<AlarmData>();

    RemarkDlg* dlg = new RemarkDlg(this);
    dlg->setRemark(data.remark());
    if (dlg->exec() != QDialog::Accepted) return;

    // �Ի���ʱ�������Ϊ����Ҫ���ñ�ע����Ŀ��ɾ�����������¶�λ��Ŀ
    item = findItem(data.attrKey(), data.dateTime());
    if (item == NULL)
    {
        QMessageBox::warning(this,
                             QString::fromLocal8Bit("��ʾ"),
                             QString::fromLocal8Bit("�ø澯��ɾ��!\n") + data.text());
        return;
    }

    data.setRemark(dlg->remark());
    QCoreApplication::postEvent(mainwin(), new AlarmEvent(data, (QEvent::Type)AlarmEvent::UPDATE));
}

