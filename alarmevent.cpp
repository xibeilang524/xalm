#include <iostream>
#include <alarmevent.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////
// PointAttr
PointAttr::PointAttr()
{
}

PointAttr::PointAttr(const QString& st, 
                     const QString& pt, 
		     unsigned int type, 
		     int mode, 
		     const QVariant& val /*=QVariant()*/)
{
    this->station = st;
    this->ptname = pt;
    this->type = type;
    this->almode = mode;
    this->val = val;
}

/////////////////////////////////////////////////////////////////////////
// AlarmData 
AlarmData::AlarmData() : m_stamp(0), m_st(WARNING), m_remark(QString::null)
{
}

AlarmData::AlarmData(unsigned int stamp, 
                     const QString& station, 
		     const QString& text, 
		     unsigned int st/*= WARNING*/, 
		     const QString& remark/*= QString::null*/)
{
    m_stamp = stamp;
    m_station = station;
    m_text = text;
    m_st =  st;
    m_remark = remark;
}

unsigned int AlarmData::stamp() const
{
    return m_stamp;
}

QString AlarmData::station() const
{
    return m_station;
}

QString AlarmData::text() const
{
    return m_text;
}

unsigned int AlarmData::state() const
{
    return m_st;
}

unsigned int AlarmData::setState(unsigned int st)
{
    unsigned int oldst = m_st;
    if (st != m_st)
        m_st = st;

    return oldst;
}

QString AlarmData::remark() const
{
    return m_remark;
}

QString  AlarmData::setRemark(const QString& rem)
{
    QString oldrem = m_remark;
    if (oldrem != rem)
        m_remark = rem;

   return oldrem; 
}

QDateTime AlarmData::date() const
{
    QDateTime dt;
    dt.setTime_t(m_stamp);
    return dt;
}

/// ģ���¼�
AlarmData* AlarmData::createAlarmData()
{
    // init points
    static QList<PointAttr*> ptlist;
    if (ptlist.size() == 0)
    {
        PointAttr* ptattr = new PointAttr(QString::fromLocal8Bit("��ޣվ"),
	                                  QString::fromLocal8Bit("#2����35kV��302����"), 
					  PointAttr::BRK, 1, 0);
	ptlist << ptattr;

	ptattr = new PointAttr(QString::fromLocal8Bit("��ޣվ"),
			       QString::fromLocal8Bit("10kV211-2��ͨ��բ"),
			       PointAttr::BRK, 1, 0);
	ptlist << ptattr;

	ptattr = new PointAttr(QString::fromLocal8Bit("��ޣվ"),
			       QString::fromLocal8Bit("10kV��ѹ�����ߵ���δ����"),
			       PointAttr::PROT, 1, 0);
	ptlist << ptattr;

	ptattr = new PointAttr(QString::fromLocal8Bit("ïԴ220kVվ"),
			       QString::fromLocal8Bit("��ï��222���ظ�ѹ���й�"),
			       PointAttr::ANA, 1, 168.24);
	ptlist << ptattr;

	ptattr = new PointAttr(QString::fromLocal8Bit("ïԴ220kVվ"),
			       QString::fromLocal8Bit("¼��ʵʱͨ��"),
			       PointAttr::RTU, 1, 1);
	ptlist << ptattr;

	ptattr = new PointAttr(QString::fromLocal8Bit("ϵͳ�¼�"),
			       QString::fromLocal8Bit("ϵͳͨѶ�������"),
			       PointAttr::SYS, 1, 1);
	ptlist << ptattr;
    }

    // ����һ������
    srand(time(NULL));
    int rd = rand() % ptlist.size();
    PointAttr* ptattr = ptlist[rd];

    AlarmData* event = new AlarmData;
    event->m_stamp = QDateTime::currentDateTime().toTime_t();
    event->m_station = ptattr->station;
    if (ptattr->type == PointAttr::ANA)
    {
        float oldval = ptattr->val.toFloat();
        float newval = ptattr->val.toFloat();
	while( (oldval>200.f && newval>200.f) ||  // ȷ�����������ͬ��Խ��״̬
               (oldval<150.f && newval<150.f) ||
	       (oldval<200.f && oldval>150.f && newval<200.f && newval>150.f))
	{
	    ptattr->val = ptattr->val.toFloat() + (rand()%2==0 ? 1.01f * (rand() % 50) : -1.01f * (rand() % 50));
	    newval = ptattr->val.toFloat();
	}
	
	if(ptattr->val.toFloat() > 200.f)
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" ����Խ��, ֵ:") + 
	                    QString().setNum(ptattr->val.toFloat());
	    event->m_st = OVERLIMIT2;
	}
	else if(ptattr->val.toFloat() > 150.f)
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" һ��Խ��, ֵ:") + 
	                    QString().setNum(ptattr->val.toFloat());
	    event->m_st = OVERLIMIT1;
	}
	else 
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" Խ�޻ָ�, ֵ:") + 
	                    QString().setNum(ptattr->val.toFloat());
	    event->m_st = REVERT;
	}
    }
    else if (ptattr->type == PointAttr::BRK)
    {
        ptattr->val = ptattr->val.toInt() == 0 ? 1:0; //��ת
	if(ptattr->val.toInt() == 0)
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" �Ͽ�");
            event->m_st = WARNING;
	}
	else 
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" �պ�");
	    event->m_st = REVERT;
	}
    }
    else if (ptattr->type == PointAttr::PROT)
    {
        ptattr->val = ptattr->val.toInt() == 0 ? 1:0;
	if(ptattr->val.toInt() == 0)
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" ����");
	    event->m_st = REVERT;
	}
	else 
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" ����");
            event->m_st = WARNING;
	}
    }
    else if (ptattr->type == PointAttr::RTU)
    {
        ptattr->val = ptattr->val.toInt() == 0 ? 1:0;
	if(ptattr->val.toInt() == 0)
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" ֹͣ");
            event->m_st = WARNING;
	}
	else 
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" ����");
	    event->m_st = REVERT;
	}
    }
    else if (ptattr->type == PointAttr::SYS)
    {
        ptattr->val = ptattr->val.toInt() == 0 ? 1:0;
	if(ptattr->val.toInt() == 0)
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" ����");
            event->m_st = WARNING;
	}
	else 
	{
            event->m_text = ptattr->ptname + QString::fromLocal8Bit(" ����");
	    event->m_st = REVERT;
	}
    }

    return event;
}

//////////////////////////////////////////////////////////////////////////
// AlarmEvent
AlarmEvent::AlarmEvent(const AlarmData& d, Type type/*= QEvent::User*/) : QEvent(type)
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
