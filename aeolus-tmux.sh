#!/bin/bash
if ! tmux has-session -t aeolus; then
	tmux new -ds aeolus -n atop 'sudo atop'
	tmux neww -t aeolus -an aeolus 'aeolus -t'
	tmux neww -t aeolus -an jamb 'jamb'
	tmux neww -t aeolus -an aconnect 'watch aconnect -l'
fi
tmux attach -t aeolus
