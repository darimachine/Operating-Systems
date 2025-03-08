#Запазете само потребителските имена от /etc/passwd във файл users във вашата home директория.
cat /etc/passwd | cut -d ":" -f 1 > users
