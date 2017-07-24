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
** 报警处理模式用于定义针对点类型或不同点属性信息的报警处理方法。
**
**
*/
class AlarmMode
{
public:
    enum MODE { };
};

class PointAttr
{
public:
    enum TYPE { ANA, PROT, BRK, RTU, SYS, };
    PointAttr();
    PointAttr(const QString&, 
              const QString&, 
              unsigned int, 
	      int, 
	      const QVariant& = QVariant());

private:
    QString         station;
    QString         ptname;
    unsigned int    type;  //TYPE 
    int             almode;
    QVariant        val;

    friend class AlarmData;
};

class AlarmData
{
public:
    enum STATE
    {
        WARNING    = 0x0001,
        OVERLIMIT1 = 0x0002,
        OVERLIMIT2 = 0x0004,
        REVERT     = 0x0008,

        AFFIRM     = 0x0010,
    };

    AlarmData();
    AlarmData(unsigned int, 
              const QString&,
	      const QString&, 
	      unsigned int = WARNING, 
	      const QString& = QString::null);

    unsigned int stamp() const;
    QDateTime date() const;
    QString station() const;
    QString text() const;

    unsigned int state() const;
    unsigned int setState(unsigned int st);

    QString remark() const;
    QString setRemark(const QString&);

    const PointAttr* pointAttr() const;

    /// 模拟事件
    static AlarmData* createAlarmData();

private:
    unsigned int  m_stamp;
    QString       m_station;
    QString       m_text;
    unsigned int  m_st;
    QString       m_remark;
};

Q_DECLARE_METATYPE(AlarmData);
Q_DECLARE_METATYPE(AlarmData*);

class AlarmEvent : public QEvent
{
public:
    AlarmEvent(const AlarmData& d, Type = QEvent::User);
    ~AlarmEvent();

    AlarmData& data();
    const AlarmData& data() const;

private:
    AlarmData m_data;    
};
