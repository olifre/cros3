#!/bin/bash

set -x
set -e
set -o pipefail

lintian --version

export TOP_DIR=${GITHUB_WORKSPACE}
export CCACHE_DIR=${TOP_DIR}/.ccache
export WORKING_DIR=${TOP_DIR}/debian/output
export SRC_DIR_NAME=source_dir

mkdir -p ${WORKING_DIR}
cp -ra ${TOP_DIR}/${SRC_DIR_NAME} ${WORKING_DIR}

# Enter source package dir
cd ${WORKING_DIR}/${SRC_DIR_NAME}


cd ${TOP_DIR}
ls -la
find
