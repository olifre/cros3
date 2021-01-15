#!/bin/bash -x

rpmdev-setuptree

tar cjf cros3-0.1.tar.bz2 -C source_dir/src .
cp *.tar.bz2 ~/rpmbuild/SOURCES/
cp source_dir/*.spec .

yum install -y gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools
yum-builddep -y cros3.spec

rpmbuild -ba cros3.spec

cd ~
find
ls -la
