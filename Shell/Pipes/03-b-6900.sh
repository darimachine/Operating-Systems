# Намерете имената на 10-те файла във вашата home директория, чието съдържание е
# редактирано най-скоро. На първо място трябва да бъде най-скоро редактираният
# файл. Намерете 10-те най-скоро достъпени файлове. (hint: Unix time)
find ~ -type f -printf "%T@:%f\n" | sort -t ':' -k1 -nr | head | cut -d ':' -f2
#AND
find ~ -type f -printf "%A@:%f\n" | sort -t ':' -k1 -nr | head | cut -d ':' -f2
