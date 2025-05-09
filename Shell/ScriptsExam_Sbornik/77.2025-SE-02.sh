# not tested

# check for env variable
# if its not set: export SVC_DIR=services
if [[ -z ${SVC_DIR} ]]; then
  echo "env file doesnt exist"
  exit 1
fi 

if [[ ${#} -lt 1 ]]; then
  echo "you must have atleast 1 argument"
  exit 2
fi
comm=${1}
nameOfProcess=$(mktemp)
ps -eo comm=,pid= >> ${nameOfProcess}
nameOfServices=$(mktemp)

#cat ${nameOfProcess}

while IFS= read -r -d '' file; do # bonus uslovie
    if [[ -z ${file} ]]; then
        continue
    fi
    name=$(cat "${file}" | grep -E "name:" | tr -s ' ' | cut -d ':' -f2 | cut -c 2-)
    pidfile=$(cat "${file}" | grep -E "pidfile:" | cut -d ':' -f2 | cut -c 2-)
    outfile=$(cat ${file} | grep -E "outfile:" | cut -d ':' -f2 | cut -c 2-)
    echo "Name: ${name} PidFile: ${pidfile} OutFile: ${outfile}"
    echo "Command: ${comm}"
    if [[ ${comm} == "cleanup" ]]; then
        #echo "Name: ${name}"
        processPid=$(cat "${pidfile}" | cut -d ' ' -f2)
        
        if ! grep -q -E "${pidd}" "${nameOfProcess}"; then
            rm -r ${pidfile}
        fi
        continue
    fi
    if [[ ${comm} == "running" ]]; then
        processPid=$(cat "${pidfile}" | cut -d ' ' -f2)
        
        if grep -q -E "${processPid}" "${nameOfProcess}"; then
            echo "${name}" >> ${nameOfServices}
        fi
        continue
    fi
    
    if [[ ${comm} == "stop" ]]; then
    
          if [[ "${2}" == "${name}" ]]; then
                processPid=$(cat "${pidfile}" | cut -d ' ' -f2)
                echo "Sigterm: ${mypid}"
                if grep -q -E "^${pidfile}" "${nameOfProcess}"; then # ako raboti ubiva
                     kill "${mypid}" # sigterm kill ubiva
                fi
          fi
          continue
    fi
    if [[ ${comm} == "start" ]]; then
          if [[ ${name} != ${2} ]]; then
              #echo "Razlichno"
              continue
          fi
          if grep -q -E "^${pidfile}" "${nameOfProcess}"; then
              continue # service veche rabodi ne pravi nishto
          fi
          command=$(cat "${file}" | grep -E "comm:" | cut -d ':' -f2 | cut -c 2-)
          eval ${command} &>${outfile} &
          echo "Pid: ${!}" > ${pidfile}
          continue
    fi
    
    echo "Files: ${file}"

done< <(find ${SVC_DIR} -type f -print0) # works for special chars as well for bonus

if [[ ${comm} == "running" ]]; then
    cat ${nameOfServices} | sort
fi
rm ${nameOfServices}
rm ${nameOfProcess}
