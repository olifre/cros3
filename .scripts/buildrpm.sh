#!/bin/bash

mkdir build
cp -ra spurce_dir/src/* build/
cp *.spec build/
cd build

yum install -y gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools
yum-builddep -y cros3.spec

rpmdev-setuptree
rpmbuild -ba hello-world.spec

cd ..
find
ls -la
