#!/bin/bash

set -e

APP_NAME="RenderDoc"
VERSION="1.38"
DOWNLOAD_URL="https://renderdoc.org/stable/1.38/renderdoc_1.38.tar.gz"
INSTALL_DIR="$HOME/Downloads/renderdoc_$VERSION"
EXEC_PATH="$INSTALL_DIR/bin/qrenderdoc"
DESKTOP_FILE="$HOME/.local/share/applications/renderdoc.desktop"
SYMLINK="/usr/local/bin/renderdoc"

echo "Downloading RenderDoc $VERSION..."

mkdir -p "$INSTALL_DIR"
cd "$HOME/Downloads"

# Download the tarball if not already downloaded
if [ ! -f "renderdoc_${VERSION}.tar.gz" ]; then
    wget "$DOWNLOAD_URL" -O "renderdoc_${VERSION}.tar.gz"
else
    echo "Tarball already downloaded."
fi

# Extract if folder doesn't exist
if [ ! -d "$INSTALL_DIR" ]; then
    tar -xzf "renderdoc_${VERSION}.tar.gz" -C "$HOME/Downloads"
else
    echo "RenderDoc already extracted."
fi

# Verify executable
if [ ! -x "$EXEC_PATH" ]; then
    echo "Error: Executable not found or not executable at $EXEC_PATH"
    exit 1
fi

echo "Creating .desktop launcher..."

mkdir -p ~/.local/share/applications

cat > "$DESKTOP_FILE" <<EOF
[Desktop Entry]
Name=$APP_NAME
Comment=RenderDoc Vulkan debugger
Exec=$EXEC_PATH
Icon=utilities-system-monitor
Type=Application
Categories=Development;
Terminal=false
EOF

chmod +x "$DESKTOP_FILE"

echo "Creating terminal shortcut..."

if [ ! -e "$SYMLINK" ]; then
    sudo ln -s "$EXEC_PATH" "$SYMLINK"
else
    echo "Symlink already exists at $SYMLINK"
fi

echo "Updating desktop database..."
update-desktop-database ~/.local/share/applications

echo "✅ RenderDoc $VERSION installed and launcher created!"
echo "You can launch it from the app menu or by typing 'renderdoc' in the terminal."
