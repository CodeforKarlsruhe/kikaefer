#!/bin/bash

sshpriv="$HOME/.ssh/id_rsa"
sshpub="$HOME/.ssh/id_rsa.pub"
unitv2ip="10.254.239.1"

usage(){
    echo "$0 "
    echo "init a unitv2 device"
    echo "init ssh key and config, too, if necessary"
    
}

if [ ! -f "$sshpriv" ] 
then
    read -r -p "no key $sshpriv found, create? [y/N]"
    if [ "$ans" == "y" ] 
    then
        ssh-keygen -f "$sshpriv" -b 4096 -C "${USER}@${HOSTNAME}_$(date -I)"
    fi
fi

sc="ssh m5stack@$unitv2ip"

$sc true
success=$?

if [[ "$success" != 0 ]]
then
    echo "installing ssh key for m5stack@$unitv2ip, default password is: 12345678"
    ssh-copy-id -o PreferredAuthentications=password -o PubkeyAuthentication=no -i "$sshpriv" m5stack@$unitv2ip
fi

# fails due to /dev/null issues
#scp unitv2_ondeviceinit.sh "m5stack@${unitv2ip}:"
#$sc sudo unitv2_ondeviceinit.sh


