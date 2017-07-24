#pragma once

#include <ui_wtdlg.h>

class WtDlg : public QDialog, Ui::WtDlg
{
    Q_OBJECT
public:
    WtDlg(QWidget* parent = 0);
    int wType() const;
};
