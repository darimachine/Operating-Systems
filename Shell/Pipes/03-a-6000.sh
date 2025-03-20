
# Копирайте <РЕПО>/exercises/data/emp.data във вашата home директория.
cp /srv/fmi-os/exercises/data/emp.data ~/

# Посредством awk, използвайки копирания файл за входнни данни, изведете:

# - общия брой редове
awk 'END {print NR}' emp.data

# - третия ред
awk 'NR == 3 {print}' emp.data

# - последното поле от всеки ред
awk '{print $3}' emp.data

# - последното поле на последния ред
awk 'END {print $NF}' emp.data

#- всеки ред, който има повече от 4 полета
awk '{if(NF>4) print $0}' emp.data

#- всеки ред, чието последно поле е по-голямо от 4
awk '$NF>4 {print}' emp.data

# - общия брой полета във всички редове
awk '{print NF}' emp.data

# - броя редове, в които се среща низът Beth
awk 'BEGIN {i=0} {if(match($0,/Beth/)) i++} END {print i}' emp.data
#OR
awk '/Beth/' emp.data  | awk 'END {print NR}'

# - най-голямото трето поле и редът, който го съдържа
awk 'BEGIN {max=0; line=" "} {if($3>max) {max = $3;line = $0}} END {print line}' emp.data

# - всеки ред, който има поне едно поле
awk 'NF >= 1 {print}' emp.data

# - всеки ред, който има повече от 17 знака
awk 'length($0) > 17 {print}' emp.data
#OR
awk '{if(length($0) > 17) print}' emp.data
# - броя на полетата във всеки ред и самият ред
awk '{print NF "-> " $0}' emp.data
#OR
# - първите две полета от всеки ред, с разменени места
awk '{print $2,$1}' emp.data

# - всеки ред така, че първите две полета да са с разменени места
awk '{printf $2 "\t" $1} {for(i=3;i<=NF;i++) {print "\t" $i}}' emp.data
#OR
awk '{temp = $1; $1=$2; $2 = temp} {print}' emp.data

# - всеки ред така, че на мястото на първото поле да има номер на реда
awk '{printf NR; {for(i=2;i<=NF;i++){printf "\t" $i}; printf "\n"}}' emp.data
#OR
awk '{print NR,$2,$3}' emp.data
#THE BEST
awk '{$1 = NR} {print}' emp.data
# - всеки ред без второто поле
awk '{$2 = ""} {print}' emp.data

# - за всеки ред, сумата от второ и трето поле
awk '{$2 = $2 + $3; $3=""} {print}' emp.data

# - сумата на второ и трето поле от всеки ред
awk 'BEGIN {sum=0} {sum+=$2+$3} END {print sum}' emp.data
