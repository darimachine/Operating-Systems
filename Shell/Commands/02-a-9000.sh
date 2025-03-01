# от предната задача: когато вече сте получили myetc с файлове, архивирайте
# всички от тях, които започват с 'c' в архив, който се казва c_start.tar

# изтрийте директорията myetc и цялото и съдържание

# изтрийте архива c_start.tar

#KOPIRAME PURVITE 10 REDA SHTOTO POSLE GURMI ZA PAMET
find /etc -perm -444 | head | xargs -I {} cp -r {} ~/myetc
##VARIAN 1-------------------------------------------------------------
find myetc/ -name "c*" | xargs -I {} tar -rf c_start.tar {}
#VARIANT 2-------------------------------------------------------
find myetc/ -name "c*" -exec tar -rf c_start2.tar {} +
#VARIANT 3--------------------------------------------------
tar -cf c_start.tar $(find myetc/ -name "c*")


#list archive
tar -tf c_start.tar

#delete
rm -rf myetc

#delete tar
rm -i c_start.tar
