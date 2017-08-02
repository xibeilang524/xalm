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
** �����������ר�е�״̬��һ���һ����������б����ͱ����������״̬�����翪��
** �ĶϿ����պϣ������źŵĶ��������飬���̵����С����ϣ�ͨ�������С�ֹͣ�ȣ���һЩ
** ģ��������Խ�ޡ��ָ�������ЩԽ����ϸ��Ϊһ��Խ�޺Ͷ���Խ�ޣ���������һЩ��������
** �����ڵ�״̬����������ڱ�������ĸ����һЩ���������������ģʽ���ڶ�����
** �Ե����ͻ�ͬ��������Ϣ�ı�����������
*/
class AlarmMode
{
public:
    /*! ����ģʽ */
    enum MODE
    {
        ACCIDENT, ///< �¹�
        PERSIST,  ///< ����
        NOPERSIST, ///< �ǳ���
        DELAY, ///< ��ʱ
    };

    AlarmMode(int = NOPERSIST, int = 10);

    int mode() const;
    void setMode(int);

    int delay() const;
    void setDelay(int);

    static const AlarmMode* almMode(int m);

private:
    int m_mode; ///< ģʽ��
    int m_delay; ///< ��ʱʱ��
};

/*! ��������Ϣ
**
** �����Գ�ֵ���Ϊֻ�������ۺ������͵ĵ㣬����ͨ����վ+����(station+ptname)
** Ψһ��ʶ���ñ�ʶ����֮ΪattrKey��
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

/*! ��������
**
** ������������һ����������Ϣ�����Ǽٶ���ͬһʱ�̣����뼶��ͬһ�����Ե�ֻ����
** һ�����������attrkey + stamp ��Ψһȷ��һ��������Ϣ��
*/
class AlarmData
{
public:
    ///��ǰ����״̬
    enum STATE
    {
        WARNING    = 0x0001,
        OVERLIMIT1 = 0x0002,
        OVERLIMIT2 = 0x0004,
        REVERT     = 0x0008,

        AFFIRM     = 0x0010,
        ACCAFFIRM  = 0x0020, // �����¹ʣ�ȷ��+����
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

    /// ģ���¼�
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

