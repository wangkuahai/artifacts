#!/bin/bash

# https://ezprompt.net/ can generate custom bashrc 

check=$(grep git_branch ~/.bashrc)
if [ -z "$check" ]; then 
    cp ~/.bashrc .bashrc.save
    cat git_branch >> ~/.bashrc
fi
source ~/.bashrc
