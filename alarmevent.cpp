/* \file alarmevent.cpp
**
** �����õĽṹ���塣
** ÿһ����������AlarmData����Ӧ���Լ��Ĳ���������ϢPointAttr����ÿ��PointAttr
** ����һ����Ӧ�ı�������ģʽAlarmMode��
** ��ϵͳҲ������һ��Я�����û�������AlarmData���¼�AlarmEvent��ͨ��postEvent��
** sendEvent�����û����ݷ��͵����ն���
**
** \author chuixj@live.com
** \date 2017/07/12
*/

#include <iostream>
#include <QUuid>
#include <alarmevent.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////
//AlarmMode

/*! ���캯��
**
** \param m ģʽ��\sa MODE
** \param d ��ʱʱ��, ����ģʽΪDELAYʱ��Ч
*/
AlarmMode::AlarmMode(int m/*=NOPERSIST*/, int d/*=10*/)
{
    m_mode = m;
    m_delay = d;
}

/// ģʽ
int AlarmMode::mode() const
{
    return m_mode;
}

/*! ����ģʽ
**
** \param m ģʽ��\sa MODE
**/
void AlarmMode::setMode(int m)
{
    m_mode = m;
}

/// ��ʱʱ��
int AlarmMode::delay() const
{
    return m_delay;
}

/*! ������ʱʱ��
**
** \param d ��ʱʱ��, ��
*/
void AlarmMode::setDelay(int d)
{
    m_delay = d;
}

/// ����ģʽ
const AlarmMode* AlarmMode::almMode(int m)
{
    static QMap<int, AlarmMode*> ammap; // ����ģʽ�ֵ�
    if (ammap.size() == 0)
    {
        ammap.insert(AlarmMode::ACCIDENT, new AlarmMode(AlarmMode::ACCIDENT, 0));
        ammap.insert(AlarmMode::PERSIST, new AlarmMode(AlarmMode::PERSIST, 0));
        ammap.insert(AlarmMode::NOPERSIST, new AlarmMode(AlarmMode::NOPERSIST, 0));
        ammap.insert(AlarmMode::DELAY, new AlarmMode(AlarmMode::DELAY, 10));
    }

    return (ammap.contains(m) ? ammap[m] : NULL);
}

/////////////////////////////////////////////////////////////////////////
// PointAttr

/// Ĭ�Ϲ���
PointAttr::PointAttr()
{
}

/*! ����
**
** \param key ��Ψһ��ʶ��
** \param grp ������ͨ���ǳ�վ��
** \param pt ����
** \param t ������, \sa TYPE
** \param m ģʽ, \sa AlarmMode::MODE
** \param v ��ǰֵ, ���������
*/
PointAttr::PointAttr(const QString& key,
                     const QString& grp,
                     const QString& pt,
                     int t,
                     int m /*=AlarmMode::NOPERSIST*/,
                     const QVariant& val/*=QVariant()*/)
{
    m_key = key;
    m_group = grp;
    m_ptname = pt;
    m_type = t;
    m_almode = m;
    m_val = val;
}

/// ���ʶ��
QString PointAttr::attrKey() const
{
    return m_key;
}

/// ����
QString PointAttr::group() const
{
    return m_group;
}

/// ����
QString PointAttr::ptname() const
{
    return m_ptname;
}

/// ����
int PointAttr::type() const
{
    return m_type;
}

const AlarmMode* PointAttr::almMode() const
{
    return AlarmMode::almMode(m_almode);
}

///ֵ
QVariant PointAttr::value() const
{
    return m_val;
}

/// ����ֵ
void PointAttr::setValue(const QVariant& val)
{
    m_val = val;
}

/////////////////////////////////////////////////////////////////////////
// AlarmData
QMap<QString, PointAttr*> AlarmData::ATTRMAP;

/// Ĭ�Ϲ���
AlarmData::AlarmData()
{
}

/*�����캯��
**
** \param key �����Ӧ�ĵ�����Ψһ��ʶ��
** \param grp ��������
** \param txt ��������
** \param tm ʱ�䣬������
** \param ms ʱ�䣬����
** \param mode ����ģʽ, \sa AlarmMode::MODE, -1=�̳е�ı���ģʽ
** \param sts ����״̬
** \param rem ��������
*/
AlarmData::AlarmData(const QString& key,
                     const QString& grp,
                     const QString& txt,
                     unsigned int tm,
                     int ms /* = 0*/,
                     int mode /*= -1*/,
                     int sts/*= WARNING*/,
                     const QString& rem/*= QString::null*/)
{
    m_key = key;
    m_sec = tm;
    m_msec = ms;
    m_text = txt;
    m_almode = mode;
    m_state =  sts;
    m_group = grp;
    m_remark = rem;
}

/// �����Ա�ʶ��
QString AlarmData::attrKey() const
{
    return m_key;
}

///��ö�Ӧ�ĵ�������Ϣ
const PointAttr* AlarmData::pointAttr() const
{
    return AlarmData::pointAttr(m_key);
}

///��ö�Ӧ�ĵ�������Ϣ
const PointAttr* AlarmData::pointAttr(const QString& key)
{
    return (ATTRMAP.contains(key) ? ATTRMAP[key] : NULL);
}

QMap<QString, PointAttr*> AlarmData::attrMap()
{
    return ATTRMAP;
}

/// ʱ��,������
unsigned int AlarmData::second() const
{
    return m_sec;
}

/// ʱ��,����
int AlarmData::msec() const
{
    return m_msec;
}

/// ת����ǰʱ��ΪQDateTime
QDateTime AlarmData::dateTime() const
{
    QDateTime dt;
    dt.setTime_t(m_sec);
    return dt.addMSecs(m_msec);
}

/// ����������һ��Ϊ��վ��
QString AlarmData::group() const
{
    return m_group;
}

/// ��������
QString AlarmData::text() const
{
    return m_text;
}

/// ״̬
int AlarmData::state() const
{
    return m_state;
}

/*! ���õ�ǰ����״̬
**
** \param sts ״ֵ̬, \sa STATE
*/
void AlarmData::setState(int sts)
{
    m_state = sts;
}

/// ��������
QString AlarmData::remark() const
{
    return m_remark;
}

/// ���ø�������
void AlarmData::setRemark(const QString& rem)
{
    m_remark = rem;
}

/*! ��ȡ����ģʽ
**
** �ú���ͨ��һ�²���ȥ��ȡ��������ģʽ��
** (1)���ȳ��Ի�ȡ�澯������ָ����ģʽ����ʧ��
** (2)��һ�����Ի�ȡ�ø澯����Ӧ��ı���ģʽ����ʧ��
** (3)���ر�������AlarmMode::DELAY��
*/
const AlarmMode* AlarmData::almMode() const
{
    const AlarmMode* am = AlarmMode::almMode(m_almode);
    if (am == NULL)
    {
        const PointAttr* attr = pointAttr();
        if (attr != NULL && attr->almMode() != NULL)
            am = attr->almMode();
        else
            am = AlarmMode::almMode(AlarmMode::DELAY);
    }

    Q_ASSERT(am!=NULL);

    return am;
}

/// ģ���¼�
AlarmData* AlarmData::createAlarmData()
{
    // init points
    static QList<PointAttr*> ptlist;
    if (ptlist.size() == 0)
    {
        PointAttr* ptattr = new PointAttr(QUuid::createUuid().toString(), //key
                                          QString::fromLocal8Bit("��ޣվ"), //group
                                          QString::fromLocal8Bit("#2����35kV��302����"), // point
                                          PointAttr::BRK, // type
                                          AlarmMode::PERSIST, // mode
                                          1); // value

        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("��ޣվ"),
                               QString::fromLocal8Bit("10kV211-2��ͨ��բ"),
                               PointAttr::BRK,
                               AlarmMode::PERSIST,
                               1);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("��ޣվ"),
                               QString::fromLocal8Bit("10kV��ѹ�����ߵ���δ����"),
                               PointAttr::PROT,
                               AlarmMode::DELAY,
                               0);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("��ޣվ"),
                               QString::fromLocal8Bit("220kV��԰��ѹ��󱸵���"),
                               PointAttr::ANA,
                               AlarmMode::PERSIST,
                               112.4);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("ïԴ220kVվ"),
                               QString::fromLocal8Bit("��ï��222���ظ�ѹ���й�"),
                               PointAttr::ANA,
                               AlarmMode::NOPERSIST,
                               168.24);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("ïԴ220kVվ"),
                               QString::fromLocal8Bit("¼��ʵʱͨ��"),
                               PointAttr::RTU,
                               AlarmMode::PERSIST,
                               1);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("ϵͳ�¼�"),
                               QString::fromLocal8Bit("ϵͳͨѶ�������"),
                               PointAttr::SYS,
                               AlarmMode::DELAY,
                               1);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);
    }

    // ����һ������
    srand(time(NULL));
    PointAttr* ptattr = ptlist[rand() % ptlist.size()];

    AlarmData* event = new AlarmData(ptattr->attrKey(),
                                     ptattr->group(),
                                     "", // text
                                     QDateTime::currentDateTime().toTime_t(),
                                     0,
                                     -1, // �̳е�ı���ģʽ
                                     WARNING);

    static int cnt = 50;
    if (cnt > 0) // ǰ50����������Ϊ�ǵ�ǰ����
    {
	cnt--;
        event->m_sec -= 8640*cnt;
    }

    static int ms = 0; //  ����һ����ͬ�ĺ���ֵ
    int newms = rand()%1000;
    while (ms == newms)
    {
        srand(newms);
        newms = rand()%1000;
    }
    ms = newms;
    event->m_msec = ms;

    if (ptattr->type() == PointAttr::ANA)
    {
        float oldval = ptattr->value().toFloat();
        float newval = oldval;
        while( (oldval>200.f && newval>200.f) ||  // ȷ�����������ͬ��Խ��״̬
                (oldval<150.f && newval<150.f) ||
                (oldval<200.f && oldval>150.f && newval<200.f && newval>150.f))
        {
            newval = newval + (rand()%2==0 ? 1.01f * (rand() % 50) : -1.01f * (rand() % 50));
        }

        ptattr->setValue(newval);
        if(newval > 200.f)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" ����Խ��, ֵ:") +
                            QString().setNum(ptattr->value().toFloat());
            event->setState(OVERLIMIT2);
        }
        else if(newval > 150.f)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" һ��Խ��, ֵ:") +
                            QString().setNum(ptattr->value().toFloat());
            event->setState(OVERLIMIT1);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" Խ�޻ָ�, ֵ:") +
                            QString().setNum(ptattr->value().toFloat());
            event->setState(REVERT);
        }
    }
    else if (ptattr->type() == PointAttr::BRK)
    {
        ptattr->setValue(ptattr->value().toInt() == 0 ? 1:0); //��ת
        if(ptattr->value().toInt() == 0)
        {
            // ģ���¹�: ����, �Ͽ�
            if ( (rand()%100) == 0)
                event->m_almode = AlarmMode::ACCIDENT;

            const AlarmMode* am = event->almMode();
            if (am->mode() == AlarmMode::ACCIDENT)
                event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" �¹���բ");
            else
                event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" �Ͽ�");

            event->setState(WARNING);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" �պ�");
            event->setState(REVERT);
        }
    }
    else if (ptattr->type() == PointAttr::PROT)
    {
        ptattr->setValue(ptattr->value().toInt() == 0 ? 1:0);
        if(ptattr->value().toInt() == 0)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" ����");
            event->setState(REVERT);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" ����");
            event->setState(WARNING);
        }
    }
    else if (ptattr->type() == PointAttr::RTU)
    {
        ptattr->setValue(ptattr->value().toInt() == 0 ? 1:0);
        if(ptattr->value().toInt() == 0)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" ֹͣ");
            event->setState(WARNING);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" ����");
            event->setState(REVERT);
        }
    }
    else if (ptattr->type() == PointAttr::SYS)
    {
        ptattr->setValue(ptattr->value().toInt() == 0 ? 1:0);
        if(ptattr->value().toInt() == 0)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" ����");
            event->setState(WARNING);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" ����");
            event->setState(REVERT);
        }
    }

    return event;
}

//////////////////////////////////////////////////////////////////////////
// AlarmEvent
AlarmEvent::AlarmEvent(const AlarmData& d, Type type/*=APPEND*/) : QEvent(type)
{
    m_data = d;
}

AlarmEvent::~AlarmEvent()
{
}

AlarmData& AlarmEvent::data()
{
    return m_data;
}

const AlarmData& AlarmEvent::data() const
{
    return m_data;
}

