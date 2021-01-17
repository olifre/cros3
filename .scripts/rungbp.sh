#!/bin/bash -x

export TOP_DIR=${GITHUB_WORKSPACE}
export CCACHE_DIR=${TOP_DIR}/.ccache
export WORKING_DIR=${TOP_DIR}/debian/output
export SRC_DIR_NAME=source_dir

mkdir -p ${WORKING_DIR}

pushd ${SRC_DIR_NAME} >/dev/null

gbp pull --ignore-branch --pristine-tar --track-missing

# Check if we can obtain the orig from the git branches
if ! gbp export-orig --tarball-dir=${WORKING_DIR}; then
  # Fallback using origtargz
  apt-get update
  origtargz -dt
  cp ../*orig.tar* ${WORKING_DIR}
  GBP_BUILDPACKAGE_ARGS="--git-overlay ${GBP_BUILDPACKAGE_ARGS}"
fi

# As of 2020-09-09, gbp doesn't have a simpler method to extract the
# debianized source package. Use --git-pbuilder=`/bin/true` for the moment:
# https://bugs.debian.org/969952
gbp buildpackage \
     --git-ignore-branch \
     --git-ignore-new \
     --git-no-create-orig \
     --git-export-dir=${WORKING_DIR} \
     --no-check-builddeps \
     --git-builder=/bin/true \
     --git-no-pbuilder \
     --git-no-hooks \
     --git-no-purge \
     ${GBP_BUILDPACKAGE_ARGS} |& filter-output

cd ${WORKING_DIR}
DEBIANIZED_SOURCE=$(find . -maxdepth 3 -wholename "*/debian/changelog" |\
                    awk -F '/debian' '{print $1}')
if [ ! "${DEBIANIZED_SOURCE}" ] ; then
  echo "Error: No valid debianized source tree found."
  exit 1
fi

#mv ${DEBIANIZED_SOURCE} ${SOURCE_DIR}
popd >/dev/null

ls -la
find
