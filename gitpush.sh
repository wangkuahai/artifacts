#!/bin/bash

message="default commit"

if (($# > 0)); then
	message=$1
	echo "get message: " $message
fi

git add . 
git commit -m $message
git push

