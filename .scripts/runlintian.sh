#!/bin/bash

set -x
set -e
set -o pipefail

export WORKING_DIR=Linux-${RELEASE}-debpkg

mkdir -p lintian

lintian --version

if lintian --fail-on error --allow-root > /dev/null ; then
  if echo "${CI_LINTIAN_FAIL_WARNING}" | grep -qE '^(1|yes|true)$'; then
    CI_LINTIAN_FAIL_ARG='--fail-on error --fail-on warning'
  else
    CI_LINTIAN_FAIL_ARG='--fail-on error'
  fi
else
  CI_LINTIAN_FAIL_ARG=''
fi

lintian	--suppress-tags "${CI_LINTIAN_SUPPRESS_TAGS}" \
	--display-info --pedantic ${CI_LINTIAN_FAIL_ARG} \
	--allow-root ${WORKING_DIR}/*.changes | tee lintian.output || ECODE=$?

if echo "${CI_LINTIAN_FAIL_WARNING}" | grep -qE '^(1|yes|true)$'; then
  grep -q '^W: ' lintian.output && ECODE=3
fi

lintian2junit.py --lintian-file lintian.output > lintian/lintian.xml

lintian --suppress-tags "${CI_LINTIAN_SUPPRESS_TAGS}" \
                --display-info --pedantic --allow-root --exp-output format=html \
                ${WORKING_DIR}/*.changes > lintian/lintian.html || true

ls -la
find

exit ${ECODE-0}
