#!/bin/zsh
ZSH_CUSTOM=~/.oh-my-zsh/custom

cp --backup=numbered ~/.zshrc ~/.zshrc-copy

# zsh-autosuggestions
git clone git@github.com:zsh-users/zsh-autosuggestions.git $ZSH_CUSTOM/plugins/zsh-autosuggestions
sed -i -E 's/(plugins[[:space:]]*=[[:space:]]*\()/\0zsh-autosuggestions /' ~/.zshrc

# wget http://mimosa-pudica.net/src/incr-0.2.zsh
# mkdir $ZSH_CUSTOM/plugins/incr
# mv incr-0.2.zsh $ZSH_CUSTOM/plugins/incr/incr.plugin.zsh
# sed -i -E 's/(plugins[[:space:]]*=[[:space:]]*\()/\0incr /' ~/.zshrc
