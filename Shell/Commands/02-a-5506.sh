# Изтрийте файловете в home директорията си по-нови от practice/01/f3 (подайте на rm опция -i за да може да изберете само тези които искате да изтриете).

find . -anewer practice/01/f3 -type f -exec rm -i {} \; # more optimal
# OR
rm -i $(find . -anewer practice/01/f3 -type f)
