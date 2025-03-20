#Във file2 (inplace) подменете всички малки букви с главни.
cat file2 | tr '[a-z]' '[A-Z]' > temp | mv temp file2
