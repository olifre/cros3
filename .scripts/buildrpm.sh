#!/bin/bash -x

tar czf cros3-0.1.tar.bz2 -C source_dir/src .

yum install -y gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools
yum-builddep -y cros3.spec

rpmdev-setuptree
rpmbuild -ba cros3.spec

cd ..
find
ls -la
