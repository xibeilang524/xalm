#include <displaysetting.h>

DisplaySetting* DisplaySetting::instance()
{
    static DisplaySetting* hinst = NULL;
    if (hinst == NULL)
        hinst = new DisplaySetting();

    return hinst;
}

DisplaySetting::DisplaySetting()
{
    settings = new QSettings("dfdz", "xalm");
}

void DisplaySetting::setValue(const QString& key, const QVariant& value)
{
    settings->setValue(key, value);
    settings->sync();
}

QVariant DisplaySetting::value(const QString& key, const QVariant& defaultValue/*=QVariant()*/) const
{
    return settings->value(key, defaultValue);
}

void DisplaySetting::remove(const QString& key)
{
    settings->remove(key);
    settings->sync();
}

QStringList DisplaySetting::allKeys() const
{
    return settings->allKeys();
}

bool DisplaySetting::contains(const QString& key) const
{
    return settings->contains(key);
}

