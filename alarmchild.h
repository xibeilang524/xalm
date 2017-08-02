#pragma once

#include <QtGui>
#include <mdichild.h>
#include <alarmevent.h>

class AlarmChild : public QTreeWidget, public MdiChild
{
    Q_OBJECT
public:
    AlarmChild(QWidget* parent = 0);

    void newAlarmItem(const AlarmData&);

    void appendAlarmItem(const AlarmData&);
    void removeAlarmItem(const AlarmData&);
    void updateAlarmItem(const AlarmData&);

protected:
    virtual void customEvent(QEvent* event);

private slots:
    void contextMenuSlot(const QPoint&);
    void updateSlot();
    void affirmSlot();
    void removeSlot();
    void remarkSlot();

private:
    void fitColumns();
    QObject* mainwin() const;
    QTreeWidgetItem* findItem(const QString&, const QDateTime& = QDateTime());

    QTimer* m_timer;
    QMenu*  m_menu;
};
