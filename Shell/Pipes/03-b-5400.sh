#Отпечатайте всички редове на файла /etc/passwd, които не съдържат символния низ 'ов'.
cat /etc/passwd | egrep -v "ов"
