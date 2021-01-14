#!/bin/bash

mkdir -p pkgs
mv *-debpkg/*.deb pkgs/
apt -y update
ERRCODE=0
apt -y install ./pkgs/* || ERRCODE=$?
find /var/lib/dkms/ -iname "make.log" -exec grep -H . {} \;
exit ${ERRCODE}
