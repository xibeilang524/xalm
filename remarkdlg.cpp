/*! \file remarkdlg.cpp
**
** …Ë÷√◊¢ Õ
**
** \author chuixj@live.com
** \date 2017/07/17
*/

#include <remarkdlg.h>

RemarkDlg::RemarkDlg(QWidget* parent/* = 0*/, Qt::WindowFlags f/* = 0*/)
    : QDialog(parent, f)
{
    setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString::fromLocal8Bit("…Ë÷√◊¢ Õ..."));

    user_comboBox->clear();
    user_comboBox->setEditable(true);
    remark_plainTextEdit->clear();
    date_Label->setText("");

    connect(oky_btn, SIGNAL(clicked()), SLOT(accept()));
    connect(cancel_btn, SIGNAL(clicked()), SLOT(reject()));
}

void RemarkDlg::setRemark(const QString& rem)
{
    QStringList fs = rem.split("::");
    if (fs.count()>0)
    {
        int idx = user_comboBox->findText(fs[0]);
        while(idx < 0)
        {
            user_comboBox->addItem(fs[0]);
            idx = user_comboBox->findText(fs[0]);
        }
        user_comboBox->setCurrentIndex(idx);
    }

    if (fs.count() > 1)
    {
        date_Label->setText(fs[1]);
    }

    if (fs.count() > 2)
        remark_plainTextEdit->setPlainText(fs[2]);
}

QString RemarkDlg::remark() const
{
    QString user = user_comboBox->currentText().simplified();
    QString text = remark_plainTextEdit->toPlainText().simplified();
    QDateTime date = QDateTime::currentDateTime();

    if (text.isEmpty()) return QString::null;

    return (user + "::" + date.toString("yyyy-MM-dd hh:mm:ss") + "::" + text);
}

