#!/bin/bash

sudo apt-get update
sudo apt-get upgrade

sudo apt-get install zsh

sh -c "$(wget -O- https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
