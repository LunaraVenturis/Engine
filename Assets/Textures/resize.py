#!/usr/bin/env python3

from PIL import Image
import argparse
import os


def resize_small_pngs(directory, width, height, recursive=False):
    """
    Resize all PNG files in `directory` to 32x32 if their size is less than 32x32.

    Parameters:
    - directory (str): Path to the folder containing PNG files.
    - recursive (bool): If True, also search in subdirectories.
    """
    for root, _, files in os.walk(directory):
        for filename in files:
            if filename.lower().endswith(".png"):
                filepath = os.path.join(root, filename)
                try:
                    with Image.open(filepath) as img:
                        w, h = img.size
                        if w < width or h < height:
                            resized = img.resize((width, height), Image.NEAREST)
                            resized.save(filepath)
                            print(f"Resized: {filepath} ({w}x{h} -> {width}x{height})")
                except Exception as e:
                    print(f"Error processing {filepath}: {e}")
        if not recursive:
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Resize small PNG images to a target size."
    )
    parser.add_argument(
        "directory", default=".", help="Path to the folder containing PNG files."
    )
    parser.add_argument(
        "-r", "--recursive", action="store_true", help="Include subdirectories."
    )
    parser.add_argument(
        "--width", type=int, default=32, help="Target width (default: 32)."
    )
    parser.add_argument(
        "--height", type=int, default=32, help="Target height (default: 32)."
    )
    args = parser.parse_args()

    resize_small_pngs(args.directory, args.width, args.height, args.recursive)

    print("Done.")

    exit(0)
