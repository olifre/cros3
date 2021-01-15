#!/bin/bash -x

yum install -y gcc rpm-build rpm-devel rpmlint make bash coreutils diffutils patch rpmdevtools
rpmdev-setuptree

tar cjf cros3-0.1.tar.bz2 -C source_dir/src .
cp *.tar.bz2 ~/rpmbuild/SOURCES/
cp source_dir/*.spec .

yum-builddep -y cros3.spec

rpmbuild -ba cros3.spec

cd ~
find
ls -la
