#!/usr/bin/env python3

import os
import subprocess

def update_vendor_submodules():
    vendor_dir = './Vendor'

    if os.path.isdir(vendor_dir):
        # If the directory exists, remove submodules
        print("📂 Vendor directory exists. Cleaning up submodules...")
        os.chdir(vendor_dir)
        for submodule in ['glm', 'SDL', 'SDL_ttf']:
            submodule_path = os.path.join(vendor_dir, submodule)
            if os.path.isdir(submodule_path):
                print(f"🚮 Removing submodule: {submodule}")
                subprocess.run(['rm', '-rf', submodule_path])
        # Update submodules
        subprocess.run(['git', 'submodule', 'update', '--init', '--recursive'])
    else:
        # If the directory does not exist, create it and initialize submodules
        print("📂 Vendor directory does not exist. Creating and initializing submodules...")
        os.makedirs(vendor_dir)
        os.chdir(vendor_dir)
        subprocess.run(['git', 'submodule', 'update', '--init', '--recursive'])

if __name__ == '__main__':
    update_vendor_submodules()
