/* \file alarmevent.cpp
**
** 报警用的结构定义。
** 每一条报警数据AlarmData都对应着自己的参数属性信息PointAttr，而每个PointAttr
** 均有一个对应的报警处理模式AlarmMode。
** 本系统也定义了一个携带有用户化数据AlarmData的事件AlarmEvent，通过postEvent或
** sendEvent将该用户数据发送到接收对象。
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

/*! 构造函数
**
** \param m 模式，\sa MODE
** \param d 延时时间, 仅当模式为DELAY时有效
*/
AlarmMode::AlarmMode(int m/*=NOPERSIST*/, int d/*=10*/)
{
    m_mode = m;
    m_delay = d;
}

/// 模式
int AlarmMode::mode() const
{
    return m_mode;
}

/*! 设置模式
**
** \param m 模式，\sa MODE
**/
void AlarmMode::setMode(int m)
{
    m_mode = m;
}

/// 延时时间
int AlarmMode::delay() const
{
    return m_delay;
}

/*! 设置延时时间
**
** \param d 延时时间, 秒
*/
void AlarmMode::setDelay(int d)
{
    m_delay = d;
}

/// 报警模式
const AlarmMode* AlarmMode::almMode(int m)
{
    static QMap<int, AlarmMode*> ammap; // 报警模式字典
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

/// 默认构造
PointAttr::PointAttr()
{
}

/*! 构造
**
** \param key 点唯一标识符
** \param grp 组名，通常是厂站名
** \param pt 点名
** \param t 点类型, \sa TYPE
** \param m 模式, \sa AlarmMode::MODE
** \param v 当前值, 浮点或整型
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

/// 点标识符
QString PointAttr::attrKey() const
{
    return m_key;
}

/// 组名
QString PointAttr::group() const
{
    return m_group;
}

/// 点名
QString PointAttr::ptname() const
{
    return m_ptname;
}

/// 类型
int PointAttr::type() const
{
    return m_type;
}

const AlarmMode* PointAttr::almMode() const
{
    return AlarmMode::almMode(m_almode);
}

///值
QVariant PointAttr::value() const
{
    return m_val;
}

/// 设置值
void PointAttr::setValue(const QVariant& val)
{
    m_val = val;
}

/////////////////////////////////////////////////////////////////////////
// AlarmData
QMap<QString, PointAttr*> AlarmData::ATTRMAP;

/// 默认构造
AlarmData::AlarmData()
{
}

/*！构造函数
**
** \param key 事项对应的点属性唯一标识符
** \param grp 事项组名
** \param txt 事项描述
** \param tm 时间，绝对秒
** \param ms 时间，毫秒
** \param mode 报警模式, \sa AlarmMode::MODE, -1=继承点的报警模式
** \param sts 事项状态
** \param rem 附加描述
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

/// 点属性标识符
QString AlarmData::attrKey() const
{
    return m_key;
}

///获得对应的点属性信息
const PointAttr* AlarmData::pointAttr() const
{
    return AlarmData::pointAttr(m_key);
}

///获得对应的点属性信息
const PointAttr* AlarmData::pointAttr(const QString& key)
{
    return (ATTRMAP.contains(key) ? ATTRMAP[key] : NULL);
}

QMap<QString, PointAttr*> AlarmData::attrMap()
{
    return ATTRMAP;
}

/// 时间,绝对秒
unsigned int AlarmData::second() const
{
    return m_sec;
}

/// 时间,毫秒
int AlarmData::msec() const
{
    return m_msec;
}

/// 转换当前时间为QDateTime
QDateTime AlarmData::dateTime() const
{
    QDateTime dt;
    dt.setTime_t(m_sec);
    return dt.addMSecs(m_msec);
}

/// 报警组名，一般为厂站名
QString AlarmData::group() const
{
    return m_group;
}

/// 报警描述
QString AlarmData::text() const
{
    return m_text;
}

/// 状态
int AlarmData::state() const
{
    return m_state;
}

/*! 设置当前报警状态
**
** \param sts 状态值, \sa STATE
*/
void AlarmData::setState(int sts)
{
    m_state = sts;
}

/// 附加描述
QString AlarmData::remark() const
{
    return m_remark;
}

/// 设置附加描述
void AlarmData::setRemark(const QString& rem)
{
    m_remark = rem;
}

/*! 获取报警模式
**
** 该函数通过一下步骤去获取报警处理模式：
** (1)首先尝试获取告警事项所指定的模式，若失败
** (2)进一步尝试获取该告警所对应点的报警模式，若失败
** (3)返回报警处理AlarmMode::DELAY。
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

/// 模拟事件
AlarmData* AlarmData::createAlarmData()
{
    // init points
    static QList<PointAttr*> ptlist;
    if (ptlist.size() == 0)
    {
        PointAttr* ptattr = new PointAttr(QUuid::createUuid().toString(), //key
                                          QString::fromLocal8Bit("苜蓿站"), //group
                                          QString::fromLocal8Bit("#2主变35kV侧302开关"), // point
                                          PointAttr::BRK, // type
                                          AlarmMode::PERSIST, // mode
                                          1); // value

        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("苜蓿站"),
                               QString::fromLocal8Bit("10kV211-2普通刀闸"),
                               PointAttr::BRK,
                               AlarmMode::PERSIST,
                               1);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("苜蓿站"),
                               QString::fromLocal8Bit("10kV低压备用线弹簧未储能"),
                               PointAttr::PROT,
                               AlarmMode::DELAY,
                               0);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("苜蓿站"),
                               QString::fromLocal8Bit("220kV常园高压侧后备电流"),
                               PointAttr::ANA,
                               AlarmMode::PERSIST,
                               112.4);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("茂源220kV站"),
                               QString::fromLocal8Bit("金茂线222开关高压侧有功"),
                               PointAttr::ANA,
                               AlarmMode::NOPERSIST,
                               168.24);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("茂源220kV站"),
                               QString::fromLocal8Bit("录波实时通道"),
                               PointAttr::RTU,
                               AlarmMode::PERSIST,
                               1);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);

        ptattr = new PointAttr(QUuid::createUuid().toString(),
                               QString::fromLocal8Bit("系统事件"),
                               QString::fromLocal8Bit("系统通讯服务进程"),
                               PointAttr::SYS,
                               AlarmMode::DELAY,
                               1);
        ptlist << ptattr;
        ATTRMAP.insert(ptattr->attrKey(), ptattr);
    }

    // 生成一个报警
    srand(time(NULL));
    PointAttr* ptattr = ptlist[rand() % ptlist.size()];

    AlarmData* event = new AlarmData(ptattr->attrKey(),
                                     ptattr->group(),
                                     "", // text
                                     QDateTime::currentDateTime().toTime_t(),
                                     0,
                                     -1, // 继承点的报警模式
                                     WARNING);

    static int cnt = 50;
    if (cnt > 0) // 前50个事项日期为非当前日期
    {
	cnt--;
        event->m_sec -= 8640*cnt;
    }

    static int ms = 0; //  生成一个不同的毫秒值
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
        while( (oldval>200.f && newval>200.f) ||  // 确保不会出现相同的越限状态
                (oldval<150.f && newval<150.f) ||
                (oldval<200.f && oldval>150.f && newval<200.f && newval>150.f))
        {
            newval = newval + (rand()%2==0 ? 1.01f * (rand() % 50) : -1.01f * (rand() % 50));
        }

        ptattr->setValue(newval);
        if(newval > 200.f)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 二级越限, 值:") +
                            QString().setNum(ptattr->value().toFloat());
            event->setState(OVERLIMIT2);
        }
        else if(newval > 150.f)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 一级越限, 值:") +
                            QString().setNum(ptattr->value().toFloat());
            event->setState(OVERLIMIT1);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 越限恢复, 值:") +
                            QString().setNum(ptattr->value().toFloat());
            event->setState(REVERT);
        }
    }
    else if (ptattr->type() == PointAttr::BRK)
    {
        ptattr->setValue(ptattr->value().toInt() == 0 ? 1:0); //反转
        if(ptattr->value().toInt() == 0)
        {
            // 模拟事故: 开关, 断开
            if ( (rand()%100) == 0)
                event->m_almode = AlarmMode::ACCIDENT;

            const AlarmMode* am = event->almMode();
            if (am->mode() == AlarmMode::ACCIDENT)
                event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 事故跳闸");
            else
                event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 断开");

            event->setState(WARNING);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 闭合");
            event->setState(REVERT);
        }
    }
    else if (ptattr->type() == PointAttr::PROT)
    {
        ptattr->setValue(ptattr->value().toInt() == 0 ? 1:0);
        if(ptattr->value().toInt() == 0)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 复归");
            event->setState(REVERT);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 动作");
            event->setState(WARNING);
        }
    }
    else if (ptattr->type() == PointAttr::RTU)
    {
        ptattr->setValue(ptattr->value().toInt() == 0 ? 1:0);
        if(ptattr->value().toInt() == 0)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 停止");
            event->setState(WARNING);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 运行");
            event->setState(REVERT);
        }
    }
    else if (ptattr->type() == PointAttr::SYS)
    {
        ptattr->setValue(ptattr->value().toInt() == 0 ? 1:0);
        if(ptattr->value().toInt() == 0)
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 故障");
            event->setState(WARNING);
        }
        else
        {
            event->m_text = ptattr->ptname() + QString::fromLocal8Bit(" 正常");
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

