#include <mdichild.h>
#include <wtdlg.h>

WtDlg::WtDlg(QWidget* parent/*=0*/) : QDialog(parent)
{
    setupUi(this);
    setWindowTitle(QString::fromLocal8Bit("新建窗口..."));

    listWidget->clear();
    QListWidgetItem* item = new QListWidgetItem(listWidget);
    item->setText(QString::fromLocal8Bit("实时告警窗"));
    item->setData(Qt::UserRole, MdiChild::Alarm);
    listWidget->setCurrentItem(item);

    item = new QListWidgetItem(listWidget);
    item->setText(QString::fromLocal8Bit("实时事件窗"));
    item->setData(Qt::UserRole, MdiChild::Event);

    item = new QListWidgetItem(listWidget);
    item->setText(QString::fromLocal8Bit("文本编辑窗"));
    item->setData(Qt::UserRole, MdiChild::Text);

    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(accept()));
    connect(oky_btn, SIGNAL(clicked()), SLOT(accept()));
    connect(cancel_btn, SIGNAL(clicked()), SLOT(reject()));
}

int WtDlg::wType() const
{
    QListWidgetItem* item = listWidget->currentItem();
    return (item ? item->data(Qt::UserRole).toInt() : -1);
}

