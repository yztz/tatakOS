#!/bin/bash

# RST=result.txt
# if [ -f $RST ];then
# 	rm $RST
# fi
# touch $RST
# 
# echo "If the CMD runs incorrectly, return value will put in $RST" > $RST
# echo -e "Else nothing will put in $RST\n" >> $RST
# echo "TEST START" >> $RST

./busybox cat ./busybox_cmd.txt | while read line
do
	# echo -e "\e[33;1m $line \e[0m"
	if [[ ${line: 0: 1} == "#" ]] ; then
		continue
	fi
	eval "./busybox $line"
	RTN=$?
	if [[ $RTN -ne 0 && $line != "false" ]] ;then
		echo -e "\e[31;1mtestcase busybox $line fail\e[0m"
		# echo "return: $RTN, cmd: $line" >> $RST
	else
		echo -e "\e[32;1mtestcase busybox $line success\e[0m"
	fi
done

# echo "TEST END" >> $RST
