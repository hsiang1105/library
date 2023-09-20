#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVariant>

namespace MiscHelper {
namespace Settings {

enum GroupType
{
    E_GROUP_NONE,
    E_GROUP_APP,
    E_GROUP_PROJECT,
    E_GROUP_DEVICE,
    E_GROUP_GUI,
    E_GROUP_COMM
};

enum KeyType
{
    E_KEY_GEOMETRY,
    E_KEY_STATE,
    E_KEY_DIR
};

QString KeyName(const QString &user_name, GroupType group = E_GROUP_NONE);
QString KeyName(KeyType key,
                GroupType group,
                const QString &prefix = "",
                const QString &suffix = "");

bool Exists(const QString &key_name);
void Remove(const QString &key_name);
QVariant Value(const QString &key_name,
               const QVariant &default_value = QVariant());

void SetValue(const QString &key_name, const QVariant &value);
void SetIniMode(const QString &path, const char *codec = "UTF-8");

}  // namespace Settings
}  // namespace MiscHelper

#endif  // SETTINGS_H
