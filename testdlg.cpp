/*! \file testdlg.h
**
** \autor chuixj@live.com
** \date 2017/07/19
*/

#include <testdlg.h>

TestDlg::TestDlg(QWidget * parent/*= 0*/, Qt::WindowFlags f/*=0*/) : QDialog(parent, f)
{
    setupUi(this);
    setWindowTitle(QString::fromLocal8Bit("信号模拟器"));

    ptype_comboBox->setEnabled(false);
    ptype_comboBox->clear();
    ptype_comboBox->addItem(QString::fromLocal8Bit("模拟量"), PointAttr::ANA);
    ptype_comboBox->addItem(QString::fromLocal8Bit("保护信号"), PointAttr::PROT);
    ptype_comboBox->addItem(QString::fromLocal8Bit("开关/刀闸"), PointAttr::BRK);
    ptype_comboBox->addItem(QString::fromLocal8Bit("RTU/设备"), PointAttr::RTU);
    ptype_comboBox->addItem(QString::fromLocal8Bit("系统"), PointAttr::SYS);

    pmode_comboBox->setEnabled(false);
    pmode_comboBox->clear();
    pmode_comboBox->addItem(QString::fromLocal8Bit("事故"), AlarmMode::ACCIDENT);
    pmode_comboBox->addItem(QString::fromLocal8Bit("持续"), AlarmMode::PERSIST);
    pmode_comboBox->addItem(QString::fromLocal8Bit("非持续"), AlarmMode::NOPERSIST);
    pmode_comboBox->addItem(QString::fromLocal8Bit("延时"), AlarmMode::DELAY);

    emode_comboBox->clear();
    emode_comboBox->addItem(QString::fromLocal8Bit("继承自点"), -1);
    emode_comboBox->addItem(QString::fromLocal8Bit("事故"), AlarmMode::ACCIDENT);
    emode_comboBox->addItem(QString::fromLocal8Bit("持续"), AlarmMode::PERSIST);
    emode_comboBox->addItem(QString::fromLocal8Bit("非持续"), AlarmMode::NOPERSIST);
    emode_comboBox->addItem(QString::fromLocal8Bit("延时"), AlarmMode::DELAY);

    init();

    connect(pgroup_comboBox, SIGNAL(currentIndexChanged(const QString&)), SLOT(groupChanged()));
    connect(pname_comboBox, SIGNAL(currentIndexChanged(const QString&)), SLOT(nameChanged()));

    connect(oky_btn, SIGNAL(clicked()), SLOT(accept()));
    connect(cancel_btn, SIGNAL(clicked()), SLOT(reject()));
    connect(apply_btn, SIGNAL(clicked()), SLOT(apply()));
}

// 设置组
void TestDlg::init()
{
    ATTRMAP = AlarmData::attrMap();

    QSet<QString> grpset;
    QMap<QString, PointAttr*>::const_iterator it = ATTRMAP.constBegin();
    while(it != ATTRMAP.constEnd()) 
    {
        grpset << it.value()->group();
	++it;
    }

    pgroup_comboBox->clear();
    pgroup_comboBox->addItems(grpset.values());
    pgroup_comboBox->setCurrentIndex(pgroup_comboBox->count() > 0 ? 0 : -1);
}

void TestDlg::groupChanged()
{
    QString group = pgroup_comboBox->currentText();

    pname_comboBox->clear();
    QMap<QString, PointAttr*>::const_iterator it = ATTRMAP.constBegin();
    while(it != ATTRMAP.constEnd()) 
    {
        PointAttr* attr = it.value();
        if (group == attr->group())
	    pname_comboBox->addItem(attr->ptname());

	++it;
    }
    pname_comboBox->setCurrentIndex(pname_comboBox->count() > 0 ? 0 : -1);
}

void TestDlg::nameChanged()
{
    QString group = pgroup_comboBox->currentText();
    QString pname = pname_comboBox->currentText();

    QMap<QString, PointAttr*>::const_iterator it = ATTRMAP.constBegin();
    while(it != ATTRMAP.constEnd()) 
    {
        PointAttr* attr = it.value();
        if (group == attr->group() && pname == attr->ptname())
	{
	    int idx = ptype_comboBox->findData(attr->type());
	    ptype_comboBox->setCurrentIndex(idx);

	    idx = pmode_comboBox->findData(attr->almMode()->mode());
	    pmode_comboBox->setCurrentIndex(idx);

	    pvalue_comboBox->clear();
	    pvalue_comboBox->setEditable(attr->type() == PointAttr::ANA);
	    if (attr->type() == PointAttr::ANA)
	    {
		pvalue_comboBox->addItem(QString().setNum(attr->value().toFloat()));
		pvalue_comboBox->setCurrentIndex(0);
	    }
	    else 
	    {
	        if (attr->type() == PointAttr::PROT)
		{
		    pvalue_comboBox->addItem(QString::fromLocal8Bit("动作"), 1);
		    pvalue_comboBox->addItem(QString::fromLocal8Bit("复归"), 0);
		}
		else if (attr->type() == PointAttr::BRK)
		{
		    pvalue_comboBox->addItem(QString::fromLocal8Bit("闭合"), 1);
		    pvalue_comboBox->addItem(QString::fromLocal8Bit("断开"), 0);
	        }
		else if (attr->type() == PointAttr::RTU)
		{
		    pvalue_comboBox->addItem(QString::fromLocal8Bit("运行"), 1);
		    pvalue_comboBox->addItem(QString::fromLocal8Bit("停止"), 0);
	        }
		else if (attr->type() == PointAttr::SYS)
		{
		    pvalue_comboBox->addItem(QString::fromLocal8Bit("正常"), 1);
		    pvalue_comboBox->addItem(QString::fromLocal8Bit("故障"), 0);
	        }

	        int idx = pvalue_comboBox->findData(attr->value().toInt());
		pvalue_comboBox->setCurrentIndex(idx);
	    }

	    break;
	}

	++it;
    }
}

void TestDlg::apply()
{
    QString group = pgroup_comboBox->currentText();
    QString pname = pname_comboBox->currentText();

    PointAttr* attr = NULL;
    QMap<QString, PointAttr*>::const_iterator it = ATTRMAP.constBegin();
    while(it != ATTRMAP.constEnd()) 
    {
        PointAttr* tmpattr = it.value();
        if (group == tmpattr->group() && pname == tmpattr->ptname())
	{
	    attr = tmpattr;
	    break;
	}

	++it;
    }
    if (attr == NULL) return;

    int state = AlarmData::WARNING;
    QString text = QString::null;
    int mode = emode_comboBox->itemData(emode_comboBox->currentIndex()).toInt();
    if (mode == -1) 
        mode = attr->almMode()->mode();

    if (attr->type() == PointAttr::ANA)
    {
        float v = pvalue_comboBox->currentText().toFloat();
	if (v > 200.f)
	{
	    state = AlarmData::OVERLIMIT2;
	    if (mode == AlarmMode::ACCIDENT)
	        text = attr->ptname() + QString::fromLocal8Bit(" 遥测事故, 值:");
	    else
	        text = attr->ptname() + QString::fromLocal8Bit(" 二级越限, 值:");
	}
	else if (v > 150.f)
	{
	    state = AlarmData::OVERLIMIT1;
	    text = attr->ptname() + QString::fromLocal8Bit(" 一级越限, 值:");
	}
	else 
	{
	    state = AlarmData::REVERT;
	    text = attr->ptname() + QString::fromLocal8Bit(" 越限恢复, 值:");
	}

	text += QString().setNum(v);
    }
    else if (attr->type() == PointAttr::BRK)
    {
        int v = pvalue_comboBox->itemData(pvalue_comboBox->currentIndex()).toInt();
	if (v == 0)
	{
	    state = AlarmData::WARNING;
	    if (mode == AlarmMode::ACCIDENT)
	       text = attr->ptname() + QString::fromLocal8Bit(" 事故跳闸");
	    else 
	       text = attr->ptname() + QString::fromLocal8Bit(" 断开");
	}
	else 
	{
	    state = AlarmData::REVERT;
	    text = attr->ptname() + QString::fromLocal8Bit(" 闭合");
	}
    }
    else if (attr->type() == PointAttr::PROT)
    {
        int v = pvalue_comboBox->itemData(pvalue_comboBox->currentIndex()).toInt();
	if (v == 0)
	{
	    state = AlarmData::REVERT;
	    text = attr->ptname() + QString::fromLocal8Bit(" 复归");
	}
	else
	{
	    state = AlarmData::WARNING;
	    if (mode == AlarmMode::ACCIDENT)
	        text = attr->ptname() + QString::fromLocal8Bit(" 事故");
	    else
	        text = attr->ptname() + QString::fromLocal8Bit(" 动作");
	}
    }
    else if (attr->type() == PointAttr::RTU)
    {
        int v = pvalue_comboBox->itemData(pvalue_comboBox->currentIndex()).toInt();
	if (v == 0)
	{
	    state = AlarmData::WARNING;
	    if (mode == AlarmMode::ACCIDENT)
	        text = attr->ptname() + QString::fromLocal8Bit(" 事故");
	    else
	        text = attr->ptname() + QString::fromLocal8Bit(" 停止");
	}
	else
	{
	    state = AlarmData::REVERT;
	    text = attr->ptname() + QString::fromLocal8Bit(" 运行");
	}
    }
    else if (attr->type() == PointAttr::SYS)
    {
        int v = pvalue_comboBox->itemData(pvalue_comboBox->currentIndex()).toInt();
        if (v == 0)
        {
            state = AlarmData::WARNING;
	    if (mode == AlarmMode::ACCIDENT)
	        text = attr->ptname() + QString::fromLocal8Bit(" 事故");
	    else
	        text = attr->ptname() + QString::fromLocal8Bit(" 停止");
        }
        else
        {
            state = AlarmData::REVERT;
            text = attr->ptname() + QString::fromLocal8Bit(" 运行");
        }   
    }   

    AlarmData data(attr->attrKey(), 
                   attr->group(), 
		   text, 
		   QDateTime::currentDateTime().toTime_t(),
		   rand()%1000, 
		   emode_comboBox->itemData(emode_comboBox->currentIndex()).toInt(), 
		   state); 
    QCoreApplication::postEvent(parent(), new AlarmEvent(data, (QEvent::Type)AlarmEvent::NEW));
}

void TestDlg::accept()
{
    apply();
    QDialog::accept();
}

