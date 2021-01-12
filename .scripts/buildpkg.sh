#!/bin/bash

export TOP_DIR=${GITHUB_WORKSPACE}
export CCACHE_DIR=${TOP_DIR}/.ccache
export WORKING_DIR=${TOP_DIR}/debian/output

mkdir -p ${WORKING_DIR}
cp -ra ${TOP_DIR}/source_dir ${WORKING_DIR}

# Enter source package dir
cd ${WORKING_DIR}/source_dir

# Add deb-src entries
sed -n '/^deb\s/s//deb-src /p' /etc/apt/sources.list > /etc/apt/sources.list.d/deb-src.list

apt-get update && eatmydata apt-get install --no-install-recommends -y \
     aptitude \
     devscripts \
     ccache \
     equivs \
     build-essential

eatmydata install-build-deps.sh .

# Generate ccache links
dpkg-reconfigure ccache
PATH="/usr/lib/ccache/:${PATH}"

# Reset ccache stats
ccache -z

 # Create build user and fix permissions
useradd buildci
chown -R buildci. ${WORKING_DIR} ${CCACHE_DIR}

# Define buildlog filename
BUILD_LOGFILE_SOURCE=$(dpkg-parsechangelog -S Source)
BUILD_LOGFILE_VERSION=$(dpkg-parsechangelog -S Version)
BUILD_LOGFILE_VERSION=${BUILD_LOGFILE_VERSION#*:}
BUILD_LOGFILE_ARCH=$(dpkg --print-architecture)
BUILD_LOGFILE="${WORKING_DIR}/${BUILD_LOGFILE_SOURCE}_${BUILD_LOGFILE_VERSION}_${BUILD_LOGFILE_ARCH}.build"

# Build package as user buildci
su buildci -c "eatmydata dpkg-buildpackage ${DB_BUILD_PARAM}" |& OUTPUT_FILENAME=${BUILD_LOGFILE} filter-output

# Restore PWD to ${WORKING_DIR}
cd ${WORKING_DIR}
rm -rf ${WORKING_DIR}/${SOURCE_DIR}

# Revert ownership for CCACHE_DIR
chown -R $(id -nu). ${CCACHE_DIR}

# Print ccache stats on job log
ccache -s

ls -la
find
