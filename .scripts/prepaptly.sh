#!/bin/bash

PKGDIR=pkgs

mkdir -p ${PKGDIR}
mv *-debpkg/*.deb ${PKGDIR}

aptly repo create -distribution ${RELEASE} -component main ${PROJECT_NAME}
aptly repo add ${CI_PROJECT_NAME} ${PKGDIR}
aptly repo show -with-packages ${CI_PROJECT_NAME}
