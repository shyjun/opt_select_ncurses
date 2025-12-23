#!/bin/bash
set -e

# ==========================================
#  Configuration
# ==========================================

MAIN_REPO_DIR="$(pwd)"            # Your main opt_select_ncurses repo
APT_REPO_DIR="$HOME/WORK/opt_select_ncurses-apt"   # Update this path if needed

PKG_VERSION="$1"

if [ -z "$PKG_VERSION" ]; then
    echo "Usage: ./tools/release.sh <version>"
    echo "Example: ./tools/release.sh 1.0-3"
    exit 1
fi

DEB_NAME="opt_select_ncurses_${PKG_VERSION}_amd64.deb"

echo "======================================"
echo " Building opt_select_ncurses $PKG_VERSION"
echo "======================================"

# ==========================================
#  Step 1 — Clean previous builds
# ==========================================
echo "[1/6] Cleaning old .deb files..."
rm -f "$MAIN_REPO_DIR"/*.deb

# ==========================================
#  Step 2 — Build the new .deb
# ==========================================
echo "[2/6] Building .deb using debian/build_deb.sh..."

cd "$MAIN_REPO_DIR/debian"
./build_deb.sh "$PKG_VERSION"

cd "$MAIN_REPO_DIR"

if [ ! -f "$DEB_NAME" ]; then
    echo "❌ ERROR: Build failed — .deb not found!"
    exit 1
fi

echo "✓ .deb built: $DEB_NAME"

# ==========================================
#  Step 3 — Copy .deb to APT repo
# ==========================================
echo "[3/6] Copying .deb to APT repository..."

cp -fv "$DEB_NAME" "$APT_REPO_DIR/apt/opt_select_ncurses_${PKG_VERSION}_amd64.deb"

# ==========================================
#  Step 4 — Regenerate Packages + Release
# ==========================================
echo "[4/6] Updating APT index files..."
cd "$APT_REPO_DIR/apt"
rm -rfv Packages Packages.gz Release
dpkg-scanpackages . /dev/null > Packages
gzip -kf Packages

cat > Release <<EOF
Origin: opt-select-ncurses
Label: opt-select-ncurses
Suite: stable
Codename: stable
Architectures: amd64
Components: main
Description: APT repository for opt-select-ncurses
EOF

echo "✓ APT index updated"

# ==========================================
#  Step 5 — Commit + push
# ==========================================
echo "[5/6] Committing APT repo updates..."

cd "$APT_REPO_DIR"
git add apt/
git commit -m "Release version $PKG_VERSION"
git push

echo "✓ Pushed to GitHub pages"

# ==========================================
#  DONE
# ==========================================
echo ""
echo "======================================"
echo " Release $PKG_VERSION published!"
echo "======================================"
echo ""
echo "Users can now install/update with:"
echo ""
echo "  sudo apt update"
echo "  sudo apt install opt-select-ncurses"
echo ""
