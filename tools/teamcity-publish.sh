#!/bin/bash
# TODO: We need a VERSION string somewhere in the code for release.
#
# - This script only curtails the current structure of the binary assets
#   provided in EtherTerm.  For proper packaging, EtherTerm should probably
#   be 'etherterm', and install as /usr/local/bin/etherterm, and "discover"
#   its assets as /usr/local/share/etherterm.
#
# - We only publish "beta" packages for now, essentially:
#   - create tgz file in form EtherTerm-2015.02.22-master-deadbeef-flavor.tar.gz
#   - extracts to a subfolder as ./EtherTerm-2015.02.22-master-deadbeef/
#   - has right executable and read/write permissions.
#
set -ex
[ -z $TEMP ] && TEMP=/tmp

FLAVOR="unknown-$(uname -m)"
case ${TEAMCITY_BUILDCONF_NAME} in
	*Debian*)
		FLAVOR=debian-$(uname -m)
		;;
esac

NAME_PACKAGE="EtherTerm-${FLAVOR}-$(date +%Y.%m.%d)-${TEAMCITY_BUILD_BRANCH}-$(git rev-parse --short HEAD)"
NAME_SUBFOLDER="EtherTerm-$(date +%Y.%m.%d)-${TEAMCITY_BUILD_BRANCH}"
PATH_PROJECT_ROOT=$(cd `dirname $0`/..; pwd)
PATH_INSTALL_ROOT=$(mktemp -d "${TEMP}/${NAME_PACKAGE}-XXXX")
PATH_INSTALL_FOLDER=${PATH_INSTALL_ROOT}/${NAME_SUBFOLDER}
PATH_PUBLISH_FOLDER=/var/www/teamcity-master.pexpect.org/repo/etherterm
PATH_PACKAGE_FILE=${PATH_PUBLISH_FOLDER}/${NAME_PACKAGE}.tgz
PATH_PACKAGE_HASH=${PATH_PUBLISH_FOLDER}/${NAME_PACKAGE}.sha256

mkdir -pm 755 ${PATH_INSTALL_FOLDER}
cp ${PATH_PROJECT_ROOT}/EtherTerm ${PATH_INSTALL_FOLDER}/
cp -r ${PATH_PROJECT_ROOT}/assets ${PATH_INSTALL_FOLDER}/
chmod 755 ${PATH_INSTALL_FOLDER}/EtherTerm ${PATH_INSTALL_FOLDER}/assets
chmod 644 ${PATH_INSTALL_FOLDER}/assets/*
tar -C ${PATH_INSTALL_ROOT} -czvf ${PATH_PACKAGE_FILE} ${NAME_SUBFOLDER}
echo rm -rf ${PATH_INSTALL_ROOT}
echo "sha256=$(shasum -a 256 ${PATH_PACKAGE_FILE} | awk '{ print $1 }') $(basename ${PATH_PACKAGE_FILE})" \
	| tee $PATH_PACKAGE_HASH

if [ "${TEAMCITY_BUILD_BRANCH}" == "master" ]; then
	# update '-current' symlink
	cd $PATH_PUBLISH_FOLDER
	rm -f EtherTerm-${FLAVOR}-current.tgz EtherTerm-${FLAVOR}-current.sha256
	ln -s ${NAME_PACKAGE}.tgz EtherTerm-${FLAVOR}-current.tgz
	# we must write a new .sha256 to match the filename, we cannot symlink it
	echo "sha256=$(shasum -a 256 ${PATH_PACKAGE_FILE} | awk '{ print $1 }') EtherTerm-${FLAVOR}-current.tgz" | tee EtherTerm-${FLAVOR}-current.sha256
fi
