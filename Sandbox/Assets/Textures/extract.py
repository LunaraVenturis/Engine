#!/usr/bin/env python3

from PIL import Image
import os


def extract_sprites(input_file: str, rows: int, columns: int):
    """
    Extracts individual sprites from a sprite sheet and saves them as separate files.

    Each file is named with its row and column: <base_name>_r<row>_c<col>.<ext>

    Parameters:
        input_file (str): Path to the sprite sheet image.
        rows (int): Number of rows in the sprite sheet.
        columns (int): Number of columns in the sprite sheet.
    """
    # Load the image
    sprite_sheet = Image.open(input_file)
    sheet_width, sheet_height = sprite_sheet.size

    # Calculate individual sprite dimensions
    sprite_width = sheet_width // columns
    sprite_height = sheet_height // rows

    # Create output directory
    base_name, ext = os.path.splitext(os.path.basename(input_file))
    output_dir = f"{base_name}_sprites"
    os.makedirs(output_dir, exist_ok=True)

    # Extract and save sprites
    for row in range(rows):
        for col in range(columns):
            left = col * sprite_width
            upper = row * sprite_height
            right = left + sprite_width
            lower = upper + sprite_height

            sprite = sprite_sheet.crop((left, upper, right, lower))
            sprite_filename = os.path.join(
                output_dir, f"{base_name}_r{row}_c{col}{ext}"
            )
            sprite.save(sprite_filename)

    print(
        f"Extracted {rows * columns} sprites to '{output_dir}/' with row/column naming."
    )


if __name__ == "__main__":
    input_file = input("Enter the path to the sprite sheet image: ")
    rows = int(input("Enter the number of rows in the sprite sheet: "))
    columns = int(input("Enter the number of columns in the sprite sheet: "))
    extract_sprites(input_file, rows, columns)
