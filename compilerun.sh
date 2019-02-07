#!/bin/bash
echo "Kompilowanie gry"
gcc -Wall gra1.c -o gra1
echo "Kompilowanie serwera"
gcc -Wall warcabys4.c -o warcabys
echo "Kompilowanie klienta"
gcc -Wall warcabyk4.c -o warcabyk

echo "Usuwanie kolejki komunikatow"
ipcrm -Q 3
echo "Starting tmux"
tmux new-session -d -s "Warcaby" "./warcabys"
tmux set -g mouse on
sleep 1
tmux split-window -h "./warcabyk"
tmux split-window -v "./warcabyk"
tmux split-window -v -t 0 "./warcabyk"
tmux split-window -v -t 0 "./warcabyk"
tmux attach-session -d -t "Warcaby"
ipcrm -M 5
