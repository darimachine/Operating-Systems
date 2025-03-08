# Отпечатайте потребителските имена и техните home директории от /etc/passwd.
cat /etc/passwd | cut -d ":" -f 1,6 | tr ':' ' '
