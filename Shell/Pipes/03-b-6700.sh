# Отпечатайте правата (permissions) и имената на всички файлове, до които имате
# read достъп, намиращи се в директорията /tmp. (hint: 'man find', вижте -readable)
find /tmp -readable -printf "%M -> %p\n" 2>/dev/null
#OR
