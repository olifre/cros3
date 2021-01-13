#!/bin/bash

PKGDIR=pkgs

mkdir -p ${PKGDIR}
mv *-debpkg/*.deb ${PKGDIR}

aptly repo create -distribution ${RELEASE} -component main ${PROJECT_NAME}
aptly repo add ${PROJECT_NAME} ${PKGDIR}
aptly repo show -with-packages ${PROJECT_NAME}

find
ls -la

cat /etc/aptly/index.html.template
