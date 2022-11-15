#!/bin/bash

# https://ezprompt.net/ can generate custom bashrc 

BashSrc=$1

if [[ $# < 1 ]];then 
    echo "Usage: BashSrc!"
    exit 
fi 

check=$(grep git_branch ~/.bashrc)
if [ -z "$check" ]; then 
    echo "Save ~/.bashrc to ~/.bashrc.save"
    cp ~/.bashrc ~/.bashrc.save
    cat $BashSrc >> ~/.bashrc
fi 
echo "Need to do: source ~/.bashrc"
