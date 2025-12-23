#!/bin/bash
set -e

PKG=opt_select_ncurses
ARCH=amd64

# ==============================
# Version handling
# ==============================
if [ -z "$1" ]; then
    echo "Usage: $0 <version>"
    echo "Example: $0 1.0-3"
    exit 1
fi

VER="$1"

echo "Compiling project..."
make -C ../

echo "Building $PKG version $VER..."

# Clean previous build
rm -rf pkg
mkdir -p pkg/DEBIAN

###############################################
# Update version inside control file
###############################################
sed "s/^Version:.*/Version: $VER/" control > pkg/DEBIAN/control

###############################################
# Install maintainer scripts
###############################################
install -m755 postinst pkg/DEBIAN/postinst
install -m755 prerm   pkg/DEBIAN/prerm

###############################################
# Install binary
###############################################
mkdir -p pkg/usr/bin
install -m755 ../opt_select_ncurses pkg/usr/bin/opt_select_ncurses

###############################################
# Install static library
###############################################
mkdir -p pkg/usr/lib
install -m644 ../libopt_select_ncurses.a pkg/usr/lib/libopt_select_ncurses.a

###############################################
# Install header
###############################################
mkdir -p pkg/usr/include/opt_select_ncurses
install -m644 ../inc/opt_select_ncurses_lib.h pkg/usr/include/opt_select_ncurses/

###############################################
# Install full source tree
###############################################
mkdir -p pkg/usr/share/opt_select_ncurses
cp -r ../src ../inc ../scripts ../Makefile ../README.md ../options.txt ../libopt_select_ncurses.a \
    pkg/usr/share/opt_select_ncurses/

###############################################
# Install PATH + cdvi + acd logic
###############################################
mkdir -p pkg/etc/profile.d
install -m644 opt_select_ncurses.sh pkg/etc/profile.d/opt_select_ncurses.sh

###############################################
# Build .deb
###############################################
OUTFILE="../${PKG}_${VER}_${ARCH}.deb"
dpkg-deb --build pkg "$OUTFILE"

echo ""
echo "===================================================="
echo "  Package created: $OUTFILE"
echo "===================================================="
echo ""
