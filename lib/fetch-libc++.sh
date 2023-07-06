#!/usr/bin/env bash

GCC_VERSION=$1

if [ $GCC_VERSION = "12.2.0" ]; then
  GCC_SHA="f879b8763de38b34ab904d1ed8733e9accf60f4f9ccab78462bb525767691420"
fi

# Name of the pre-created compiled directories.
ZIP_FILE=libtock-libc++-$GCC_VERSION.zip

# List of mirrors we support.
MIRRORS=(\
  "https://www.cs.virginia.edu/~bjc8c/archive/tock"\
  "https://alpha.mirror.svc.schuermann.io/files"\
)

let FOUND=0

# Try from each mirror until we successfully download a .zip file.
for MIRROR in ${MIRRORS[@]}; do
  URL=$MIRROR/$ZIP_FILE
  echo "Fetching libc++ from ${MIRROR}..."
  echo "  Fetching ${URL}..."
  wget -q "$URL" && (echo "$GCC_SHA $ZIP_FILE" | sha256sum -c)
  if [ $? -ne 0 ]; then
    echo "  WARNING: Fetching libc++ from mirror $MIRROR failed!" >&2
  else
    let FOUND=1
    break
  fi
done

if [[ $FOUND -ne 0 ]]; then
  unzip $ZIP_FILE
else
  echo "ERROR: Unable to find tock-libc++"
  exit -1
fi
