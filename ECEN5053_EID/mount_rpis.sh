#!/bin/sh

sudo sshfs -o allow_other pi@192.168.1.16:/ /mnt/server-pi
sudo sshfs -o allow_other pi@192.168.1.17:/ /mnt/client-pi
