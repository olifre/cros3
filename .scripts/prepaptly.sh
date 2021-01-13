#!/bin/bash

PKGDIR=pkgs

mkdir -p ${PKGDIR}
mv *-debpkg/*.deb ${PKGDIR}

aptly repo create -distribution ${RELEASE} -component main ${PROJECT_NAME}
aptly repo add ${PROJECT_NAME} ${PKGDIR}
aptly repo show -with-packages ${PROJECT_NAME}

ARCHITECTURES=$(aptly repo show -with-packages ${PROJECT_NAME} | \
		awk 'BEGIN {FS="_"} /^Packages:/ {x=NR} (x && NR>x) {print $3}' | \
		egrep -v '^$' | sort -u | tr '\n' ','); \

echo $ARCHITECTURES

aptly publish repo \
		-skip-signing \
		${ARCHITECTURES:+ -architectures=${ARCHITECTURES}} \
		${PROJECT_NAME}

find ~/.aptly

tar czf debrepo.tar.gz -C ~/.aptly/public .
