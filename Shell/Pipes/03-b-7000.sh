# да приемем, че файловете, които съдържат C код, завършват на `.c` или `.h`.
# Колко на брой са те в директорията `/usr/include`?
# Колко реда C код има в тези файлове?
find /usr/include -name "*.c" -o -name "*.h" -type f | wc -l

find /usr/include -name "*.c" -o -name "*.h" -type f -exec wc -l {} ';'

#Сумата им
find /usr/include -name "*.c" -o -name "*.h" -type f -exec wc -l {} ';' | cut -d ' ' -f1 | awk '{sum+=$1} END {print sum}'
#Средния брой редове във всички файлове

find /usr/include -name "*.c" -o -name "*.h" -type f -exec wc -l {} ';' | cut -d ' ' -f1 | awk '{sum+=$1; count++} END {print sum/count}'