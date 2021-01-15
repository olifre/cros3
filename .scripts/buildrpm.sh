#!/bin/bash -x

yum install -y gcc rpm-build rpm-devel rpmlint make bash diffutils patch rpmdevtools
rpmdev-setuptree

mv source_dir/src cros3-0.1
tar cjf cros3-0.1.tar.bz2 cros3-0.1
cp *.tar.bz2 ~/rpmbuild/SOURCES/
cp source_dir/*.spec .

yum-builddep -y cros3.spec

rpmbuild -ba cros3.spec

cd ~
find
ls -la
