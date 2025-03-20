#Колко файлове в /bin са 'shell script'-oве? (Колко файлове в дадена директория са ASCII text?)
find /bin/ -type f -exec file {} ';'| grep "shell script" | wc -l
#(Колко файлове в дадена директория са ASCII text?)
find /bin/ -type f -exec file {} ';'| grep "ASCII text" | wc -l

