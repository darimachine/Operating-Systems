# Изведете всички обикновени ("regular") файлове, които само преките поддиректории на /etc съдържат
find /etc -type f -mindepth 2 -maxdepth 2
