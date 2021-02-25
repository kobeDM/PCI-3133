#! /bin/sh
EXE_1="nohup /home/msgc/src/PCI-3133/runPCI-3133_read 1 -10 10"
EXE_2="nohup /home/msgc/src/PCI-3133/runPCI-3133_read 2 0 5"
EXE_3="nohup sudo /usr/local/bin/python3 /home/msgc/bin/getCAEN1471_v3.py"
echo $EXE_1
#echo $EXE_2
#echo $EXE_3
${EXE_1} &
#${EXE_2} &
#${EXE_3} &
#gnome-terminal --title "PCI-3133 board 1" -x ${EXE_1} 
#gnome-terminal  --title "PCI-3133 board 2" -x ${EXE_2}
