#Направете списък с директориите на вашата файлова система, до които нямате достъп. Понеже файловата система може да е много голяма, търсете до 3 нива на дълбочина.
find -maxdepth 3 -type d 2>&1 >/dev/null | cut -d : -f2 | tr -d ' ' | cut -c 6- | rev | cut -c 4- | rev
#OR
find -maxdepth 3 -type d ! -perm -u=r -printf '%f\n' 2>/dev/null

