#Копирайте най-малкия файл от тези, намиращи се в /etc, в home директорията си.
# Variant 1
find /etc -type f -exec wc -c {} \; 2> null | sort -nbr | tail -1 | cut -d " " -f 2 | xargs -I {} cp {} ~/
# ИЛИ
cp $(find /etc/ -type f -exec wc -c {} \; 2>null | sort -bnr | tail -1 | cut -d " " -f2) ~/
#Variant 2
find /etc -type f -printf "%s %p\n" 2> null | sort -bnr | tail -1 | cut -d " " -f2 | xargs -I {} cp {} ~
# ИЛИ
cp $(find /etc/ -type f -printf "%s %p\n" 2> null | sort -nbr | tail -1 | cut -d " " -f2) ~
