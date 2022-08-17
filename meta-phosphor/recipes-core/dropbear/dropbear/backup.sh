#!/bin/bash
EvntFILE=/etc/keystore/dropbear_rsa_host_key
sshFile=/etc/dropbear/dropbear_rsa_host_key

if [ -f $EvntFILE ] && [ -f $sshFile ]
then
	if ! /usr/bin/diff -q $EvntFILE $sshFile
	then
		cp -a $sshFile $EvntFILE
		echo "Notification: host_key has been copied!"
	fi
else
	if [ -f $EvntFILE ]
	then
		cp -a $EvntFILE $sshFile
	elif [ -f $sshFile ]
	then
		cp -a $sshFile $EvntFILE
	fi
fi
