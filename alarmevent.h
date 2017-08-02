/*! \file alarmevent.h
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

#pragma once

#include <QtGui>

/*! 报警处理模式
**
** 报警事项都有其专有的状态。一般地一条报警事项都有报警和报警解除两种状态。比如开关
** 的断开、闭合，保护信号的动作、复归，进程的运行、故障，通道的运行、停止等，对一些
** 模拟量则有越限、恢复，而有些越限又细分为一级越限和二级越限，还有其他一些报警事项
** 则属于单状态事项，即不存在报警解除的概念，如一些操作事项。报警处理模式用于定义针
** 对点类型或不同点属性信息的报警处理方法。
*/
class AlarmMode
{
public:
    /*! 报警模式 */
    enum MODE
    {
        ACCIDENT, ///< 事故
        PERSIST,  ///< 持续
        NOPERSIST, ///< 非持续
        DELAY, ///< 延时
    };

    AlarmMode(int = NOPERSIST, int = 10);

    int mode() const;
    void setMode(int);

    int delay() const;
    void setDelay(int);

    static const AlarmMode* almMode(int m);

private:
    int m_mode; ///< 模式名
    int m_delay; ///< 延时时间
};

/*! 点属性信息
**
** 点属性除值外均为只读。不论何种类型的点，均可通过厂站+点名(station+ptname)
** 唯一标识，该标识符称之为attrKey。
*/
class PointAttr
{
public:
    enum TYPE { ANA, PROT, BRK, RTU, SYS, };

    PointAttr();
    PointAttr(const QString&,
              const QString&,
              const QString&,
              int,
              int = AlarmMode::DELAY,
              const QVariant& = QVariant());

    QString attrKey() const;
    QString group() const;
    QString ptname() const;
    int type() const;
    const AlarmMode* almMode() const;
    QVariant value() const;
    void setValue(const QVariant&);

private:
    QString     m_key;
    QString     m_group;
    QString     m_ptname;
    int         m_type;   //TYPE
    int         m_almode; //AlarmMode::MODE
    QVariant    m_val;
};

/*! 报警数据
**
** 报警数据描述一条报警的信息。我们假定在同一时刻（毫秒级）同一个属性点只能有
** 一条报警，因此attrkey + stamp 可唯一确定一条报警信息。
*/
class AlarmData
{
public:
    ///当前报警状态
    enum STATE
    {
        WARNING    = 0x0001,
        OVERLIMIT1 = 0x0002,
        OVERLIMIT2 = 0x0004,
        REVERT     = 0x0008,

        AFFIRM     = 0x0010,
        ACCAFFIRM  = 0x0020, // 仅对事故，确认+复归
    };

    AlarmData();
    AlarmData(const QString&,
              const QString&,
              const QString&,
              unsigned int,
              int = 0,
              int = -1,
              int = WARNING,
              const QString& = QString::null);

    QString attrKey() const;
    const PointAttr* pointAttr() const;
    static const PointAttr* pointAttr(const QString&);

    const AlarmMode* almMode() const;

    unsigned int second() const;
    int msec() const;
    QDateTime dateTime() const;

    QString text() const;
    QString group() const;

    int state() const;
    void setState(int);

    QString remark() const;
    void setRemark(const QString&);

    /// 模拟事件
    static QMap<QString, PointAttr*> attrMap();
    static AlarmData* createAlarmData();

private:
    static QMap<QString, PointAttr*> ATTRMAP;

    QString       m_key;
    unsigned int  m_sec;
    int           m_msec;
    QString       m_group;
    QString       m_text;
    int           m_almode;
    int           m_state;
    QString       m_remark;
};

Q_DECLARE_METATYPE(AlarmData);
Q_DECLARE_METATYPE(AlarmData*);

class AlarmEvent : public QEvent
{
public:
    enum TYPE
    {
        NEW = QEvent::User + 1000,  
        UPDATE, 
        REMOVE, 
    };

    AlarmEvent(const AlarmData& d, Type = (Type)NEW);
    ~AlarmEvent();

    AlarmData& data();
    const AlarmData& data() const;

private:
    AlarmData m_data;
};

