#include "settings.h"
#include <QApplication>
#include <QObject>
#include <QSettings>

using namespace MiscHelper;

namespace {
QSettings *g_settings = NULL;
QSettings *getSettings()
{
    if (!g_settings) {
        g_settings =
            new QSettings(qApp->organizationName(), qApp->applicationName());

        QObject::connect(qApp, &QApplication::destroyed, [=]() {
            if (g_settings) {
                delete g_settings;
                g_settings = NULL;
            }
        });
    }

    return g_settings;
}

QString groupName(Settings::GroupType group)
{
    QString name;
    switch (group) {
        case Settings::E_GROUP_APP:
            name = "Application";
            break;
        case Settings::E_GROUP_PROJECT:
            name = "Project";
            break;
        case Settings::E_GROUP_DEVICE:
            name = "Device";
            break;
        case Settings::E_GROUP_GUI:
            name = "Gui";
            break;
        case Settings::E_GROUP_COMM:
            name = "Communication";
            break;
        default:
            break;
    }

    return name;
}

QString keyName(Settings::KeyType key)
{
    QString name;
    switch (key) {
        case Settings::E_KEY_GEOMETRY:
            name = "Geometry";
            break;
        case Settings::E_KEY_STATE:
            name = "State";
            break;
        case Settings::E_KEY_DIR:
            name = "Dir";
            break;
        default:
            break;
    }

    return name;
}

}  // namespace

QString Settings::KeyName(const QString &user_name, Settings::GroupType group)
{
    QString key_name;
    if (group == Settings::E_GROUP_NONE) {
        key_name = user_name;
    } else {
        key_name = QString("%1/%2").arg(groupName(group)).arg(user_name);
    }

    return key_name;
}

QString Settings::KeyName(Settings::KeyType key,
                          Settings::GroupType group,
                          const QString &prefix,
                          const QString &suffix)
{
    QString key_name = keyName(key);
    if (!prefix.isEmpty()) {
        key_name.prepend(prefix);
    }

    if (!suffix.isEmpty()) {
        key_name.append(suffix);
    }

    if (group != Settings::E_GROUP_NONE) {
        key_name = QString("%1/%2").arg(groupName(group)).arg(key_name);
    }

    return key_name;
}

bool Settings::Exists(const QString &key_name)
{
    bool res = getSettings()->contains(key_name);
    return res;
}

void Settings::Remove(const QString &key_name)
{
    getSettings()->remove(key_name);
    getSettings()->sync();
}

QVariant Settings::Value(const QString &key_name, const QVariant &default_value)
{
    QVariant value = getSettings()->value(key_name, default_value);
    return value;
}

void Settings::SetValue(const QString &key_name, const QVariant &value)
{
    getSettings()->setValue(key_name, value);
    getSettings()->sync();
}

void Settings::SetIniMode(const QString &path, const char *codec)
{
    if (g_settings) {
        delete g_settings;
        g_settings = NULL;
    }

    g_settings = new QSettings(path, QSettings::IniFormat);
    g_settings->setIniCodec(codec);
}
