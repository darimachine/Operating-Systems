# Сортирайте /etc/passwd лексикографски по поле UserID.
cat /etc/passwd | sort -k3 -t ":"
