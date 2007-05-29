#!/bin/sh

PACKAGE_DIRECTORY=Cyphesis.app
CONTENT_DIRECTORY=${PACKAGE_DIRECTORY}/Contents
PLIST_FILE=${CONTENT_DIRECTORY}/Info.plist

# Create the basic package hierarchy
mkdir -p ${CONTENT_DIRECTORY}

# Add a minimal package list
# echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" > ${PLIST_FILE}
# echo "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" >> ${PLIST_FILE}
# echo "<plist version=\"1.0\">" >> ${PLIST_FILE}
# echo "<dict>" >> ${PLIST_FILE}
# echo "  <key>CFBundleExecutable</key>" >> ${PLIST_FILE}
# echo "  <string>cyphesis</string>" >> ${PLIST_FILE}
# echo "  <key>CFBundleIdentifier</key>" >> ${PLIST_FILE}
# echo "  <string>org.worldforge.cyphesis</string>" >> ${PLIST_FILE}
# echo "  <key>CFBundleName</key>" >> ${PLIST_FILE}
# echo "  <string>BARRG</string>" >> ${PLIST_FILE}
# echo "  <key>LSBackgroundOnly</key>" >> ${PLIST_FILE}
# echo "  <boolean>1</boolean>" >> ${PLIST_FILE}
# echo "</dict>" >> ${PLIST_FILE}
# echo "</plist>" >> ${PLIST_FILE}
cp scripts/Info.plist ${PLIST_FILE}

# ./configure --bindir=`pwd`/${CONTENT_DIRECTORY}/MacOS --datadir=`pwd`/${CONTENT_DIRECTORY}/Resources --sysconfdir=`pwd`/${CONTENT_DIRECTORY}/Resources/etc --localstatedir=/var
