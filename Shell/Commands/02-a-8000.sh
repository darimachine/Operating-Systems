# Копирайте всички файлове от /etc, които могат да се четат от всички, в
# директория myetc в home директорията ви. Направете такава, ако нямате.
mkdir ~/myetc;
find /etc/ -perm -444 -exec cp -r {} myetc/ \; # better more optimal copies one by one
# OR
cp -r $(find /etc/ -perm -444) ~/myetc #doesnt copy one by one problem with large files and file names containg spaces

