# Изведете всички обикновени файлове в директорията /tmp които са от вашата група, които имат write права за достъп за група или за останалите(o=w)
find /tmp -type f -group students -perm /g=w,o=w
or
find /tmp -type f -group students -perm /022
or
find /tmp -type f -group $(id -g) -perm /g=w,o=w



