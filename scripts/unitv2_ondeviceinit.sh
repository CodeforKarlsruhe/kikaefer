#!/bin/zsh

if [ ! -f "/root/.ssh/authorized_keys" ] ; then
    sudo mkdir -p /root/.ssh/
    sudo chmod 700 /root/.ssh/
    sudo cp ~m5stack/.ssh/authorized_keys /root/.ssh/authorized_keys
fi

sudo chown -R root:root /root # not avahi, otherwise key-based login fails

#chmod a+rw /dev/null # permission denied for user
sudo rm /dev/null
sudo mknod /dev/null c 1 3
sudo chmod 666 /dev/null

if [ -z "$(grep '^m5stack' /etc/sudoers)" ] ; then
    sed -i -e '$a\\nm5stack ALL=(ALL) NOPASSWD: ALL' /etc/sudoers 
fi
