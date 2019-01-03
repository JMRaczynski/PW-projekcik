#!/bin/bash
echo "Kompilowanie serwera"
gcc -Wall warcabys.c gra.c -o warcabys
echo "Kompilowanie klienta"
gcc -Wall warcabyk.c -o warcabyk

echo "Usuwanie kolejki komunikatow"
ipcrm -q 0
echo "Starting tmux"
tmux new-session -d -s "Warcaby" "./warcabys"
tmux set -g mouse on
sleep 1
tmux split-window -h "./warcabyk"
tmux split-window -v "./warcabyk"
tmux split-window -v -t 0 "./warcabyk"
tmux split-window -v -t 0 "./warcabyk"
tmux attach-session -d -t "Warcaby"
