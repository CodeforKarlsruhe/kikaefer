#!/bin/bash

sshcnf="$HOME/.ssh/config"
cnfdr="$HOME/.ssh/conf.d/"


read -r -p "init a config dir for ssh in $cnfdr? [y/N]" ans

if [ "$ans" == "y" ] ; then
mkdir -p "$cnfdr"
chmod 700 "$cnfdr"

cp ssh.unitv2.conf "$cnfdr"
# check if config file and conf.d Include exists, create file or add directive, if necessary
if [ ! -s "$sshcnf" ] # not: file exists and has a size greater than zero
then
    echo 'Include ~/ssh/conf.d/*.conf' >> "$sshcnf"
else
    if [ -z "$(grep 'Include ~/\.ssh/conf\.d/\*\.conf' "$sshcnf")" ]
    then
        echo 'adding Include ~/.ssh/conf.d/*.conf to' "$sshconf"
        sed -i -e '1iInclude ~/.ssh/conf.d/*.conf' "$sshcnf"
    fi
fi

