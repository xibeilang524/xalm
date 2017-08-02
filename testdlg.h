/*! \file testdlg.h
**
** \autor chuixj@live.com
** \date 2017/07/19
*/

#pragma once

#include <QtGui>
#include <alarmevent.h>
#include <ui_testdlg.h>

class TestDlg : public QDialog, Ui::TestDlg
{
    Q_OBJECT
public:
    TestDlg(QWidget * parent = 0, Qt::WindowFlags f = 0);
    void init();

private slots:
    void groupChanged();
    void nameChanged();

    void apply();
    void accept();

private:
    QMap<QString, PointAttr*> ATTRMAP;
};

