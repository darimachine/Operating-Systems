# Прочетете текстов файл file1 и направете всички главни букви малки като
# запишете резултата във file2.
cat file1 | tr '[:upper:]' '[:lower:]' > file2
