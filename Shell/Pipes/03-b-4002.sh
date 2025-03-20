#Във file3 (inplace) изтрийте всички "1"-ци.
cat file1 | tr -d '1' > temp | mv temp file1
sed -i 's:1::g' file1

