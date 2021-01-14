#!/bin/bash -x

mkdir build
cp -ra source_dir/src/* build/
cp source_dir/*.spec build/
cd build

yum install -y gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools
yum-builddep -y cros3.spec

rpmdev-setuptree
rpmbuild -ba cros3.spec

cd ..
find
ls -la
