#pragma once

#include <QtGui>
#include <mdichild.h>
#include <alarmevent.h>

class AlarmChild : public QTreeWidget, public MdiChild
{
    Q_OBJECT
public:
    AlarmChild(QWidget* parent = 0);
    void addNewAlarmData(const AlarmData&);

protected:
    virtual void customEvent(QEvent* event);

private slots:
    void contextMenuSlot(const QPoint&);
    void updateSlot();
    void affirmSlot();
    void removeSlot();

private:
    void fitColumns();

    QTimer* m_timer;
    QMenu*  m_menu;
};
