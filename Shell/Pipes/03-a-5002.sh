# Изведете само имената на хората с второ име по-дълго от 6 (>6) символа според /etc/passwd
cat /etc/passwd | cut -d ":" -f 5 |cut -d ',' -f 1 | awk 'length($2)>6 {print}'
