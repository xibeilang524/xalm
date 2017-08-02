/*! \file remarkdlg.h
**
** ����ע��
**
** \author chuixj@live.com
** \date 2017/07/17
*/

#pragma once

#include <QtGui>
#include <ui_remarkdlg.h>

/// ����ע��
class RemarkDlg : public QDialog, Ui::RemarkDlg
{
    Q_OBJECT
public:
    RemarkDlg(QWidget* parent = 0, Qt::WindowFlags f = 0);

    void setRemark(const QString&);
    QString remark() const;
};

