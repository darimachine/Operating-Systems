#Изведете статистика за най-често срещаните символи в трите файла.
cat file{1,2,3} | grep -o . | sort | uniq -c | sort -nr
