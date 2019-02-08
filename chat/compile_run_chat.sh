#!/bin/bash
echo "Kompilacja klienta"
gcc -Wall -pthread klient.c -o klient
echo "Usuwanie kolejki i fifo"
ipcrm -a
rm server_1234
echo "Starting tmux"
tmux new-session -d -s "Chat" "./serwer"
tmux set -g mouse on
sleep 0.3
tmux split-window -h "./klient"
sleep 0.3
tmux split-window -v "./klient"
sleep 0.3
tmux split-window -v -t 0 "./klient"
sleep 0.3
tmux split-window -v -t 0 "./klient"
tmux attach-session -d -t "Chat"
ipcrm -a
rm server_1234
