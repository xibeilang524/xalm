#pragma once

#include <QtGui>

class DisplaySetting
{
public:
    static DisplaySetting* instance();

    void setValue(const QString& key, const QVariant& value);
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void remove(const QString& key);

    QStringList	allKeys() const;
    bool contains(const QString& key) const;

private:
    DisplaySetting();
    QSettings* settings;
};

