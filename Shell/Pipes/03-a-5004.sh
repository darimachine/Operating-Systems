# Изведете целите редове от /etc/passwd за хората от 03-a-5003
cat /etc/passwd | awk -F ":" '{gsub(","," ",$5); split($5,name," "); if(length(name[2]) < 8) print}'
