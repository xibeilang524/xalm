/*! \file alarmevent.h
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

#pragma once

#include <QtGui>

/*! ��������ģʽ
**
** ��������ģʽ���ڶ�����Ե����ͻ�ͬ��������Ϣ�ı�����������
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

    /// ģ���¼�
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
