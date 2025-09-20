import os
import sys
import argparse
import subprocess
import json
import re
import requests
from tqdm import tqdm
import shutil
from pathlib import Path
import platform

class VulkanConfigurator:
    @classmethod
    def GetLatestVersion(cls, platform):
        # Run the curl command to get the latest Vulkan SDK version in JSON format
        query_result = subprocess.run(
            ["curl", "-s", "https://vulkan.lunarg.com/sdk/latest.json"],  # '-s' makes curl silent
            stdout=subprocess.PIPE,  # Capture stdout
            stderr=subprocess.PIPE   # Capture stderr in case of an error
        )

        # Check if the curl command succeeded
        if query_result.returncode != 0:
            raise Exception(f"Failed to get latest version of Vulkan SDK. Error: {query_result.stderr.decode()}")

        # Decode the response to a string
        jsondata = query_result.stdout.decode()

        # Parse the JSON data
        try:
            data = json.loads(jsondata)
            
            if platform == "windows":
                latest_version = data["windows"]
            elif platform == "linux":
                latest_version = data["linux"]
            elif platform == "mac":
                latest_version = data["mac"]
            else:
                raise Exception(f"Unsupported platform: {platform}")
            return latest_version
        except json.JSONDecodeError:
            raise Exception("Failed to decode JSON response from Vulkan SDK API.")
    @classmethod 
    def GetCurrentVersion(cls, install_dir):
        vulkan_folders = []
        print("-------------------------")
        print(f"Searching for Vulkan SDK in {os.path.abspath(install_dir)}")
        print("-------------------------")
        # Walk through the directory structure
        for root, dirs, files in os.walk(os.path.abspath(install_dir)):
            # Check for the 'vulkan' folder in the current directory
            if "vulkan" in os.path.basename(root).lower():
                # Find subfolders that resemble Vulkan version numbers (e.g., 1.2.189)
                version_pattern = re.compile(r"(^\d+\.\d+\.\d+)(.\d+)$")  # Simple version pattern like 1.2.189
                for subfolder in dirs:
                    if version_pattern.match(subfolder):
                        print (f"Found Vulkan SDK version: {subfolder}")
                        vulkan_folders.append(os.path.join(root, subfolder))

        if vulkan_folders:
            version = vulkan_folders[-1].replace("\\", '/').split('/')[-1]
            path = vulkan_folders[-1]

            vulkan_info_meta = {
                "version": version,
                "path": path
            }

            return vulkan_info_meta
        else:
            return None
    @classmethod
    def Download(cls, install_dir, platform, version):
        extension = ""
        if platform == "windows":
            extension = ".exe"
        elif platform == "linux":
            extension = ".tar.xz"
        elif platform == "mac":
            extension = ".zip"
        else:
            raise Exception(f"Unsupported platform: {platform}")
        url = f"https://sdk.lunarg.com/sdk/download/{version}/{platform}/vulkan_sdk{extension}"

        print(f"Downloading Vulkan SDK {version} for {platform} to {install_dir}/vulkan/{version}...")

        dest_path = install_dir + "/" + f"vulkan_sdk{extension}"

        os.makedirs(os.path.dirname(dest_path), exist_ok=True)

        response = requests.get(url, stream=True)
        response.raise_for_status()

        total = int(response.headers.get('content-length', 0))
        with open(dest_path, "wb") as file, tqdm(
            desc="⬇ Downloading",
            total=total,
            unit='B',
            unit_scale=True,
            unit_divisor=1024,
        ) as bar:
            for chunk in response.iter_content(chunk_size=8192):
                file.write(chunk)
                bar.update(len(chunk))
        return install_dir + "/" + f"vulkan_sdk{extension}"

    @classmethod
    def Extract(cls,file_path, dir):
        print(f"Extracting Vulkan SDK {file_path} to {dir}...")
        subprocess.run(["tar", "-xf", file_path, "-C", dir])
        os.remove(file_path)

    @classmethod 
    def Install(cls, install_dir, platform, version):
        downloaded_file = cls.Download(install_dir, platform, version)

        if platform != "windows":
            if not os.path.exists(install_dir + "/vulkan"):
                os.makedirs(install_dir + "/vulkan")
            cls.Extract(downloaded_file, install_dir + "/vulkan")
            dirs = [d for d in os.listdir(os.path.join(install_dir)) if os.path.isdir(d)]
            if version in dirs:
                print(f"There seems to be a subfolder with the same name as the version, {version}. Moving it to the root folder.")
                all_entries = os.listdir(install_dir + "/" + version)
                os.rmdir(install_dir + "/" + version)
        
        if platform == "linux":
            cls.HandleLinuxInstall(install_dir + "/vulkan", version)
        elif platform == "mac":
            cls.HandleMacInstall(install_dir + "/" + version, version)
        elif platform == "windows":
            cls.HandleWindowsInstall(install_dir + "/" + version, version)
        else:
            raise Exception(f"Unsupported platform: {platform}")

    @classmethod
    def GetCurrentDistro(cls):
        info = platform.freedesktop_os_release()
        return info.get("NAME")
    @classmethod
    def AppendVulkanExports(cls,version, install_dir):
        shell = os.environ.get("SHELL", "")
        if "zsh" in shell:
            shell_rc = Path.home() / ".zshrc"
        elif "bash" in shell:
            shell_rc = Path.home() / ".bashrc"
        else:
            shell_rc = Path.home() / ".profile"  # Fallback

        vulkan_path = f"{install_dir}/{version}/x86_64"
        vulkan_path = Path(vulkan_path).absolute()
        export_block = f"""
    # >>> Vulkan SDK exports >>>
    export VULKAN_SDK={vulkan_path}
    export PATH=$VULKAN_SDK/bin:$PATH
    export LD_LIBRARY_PATH=$VULKAN_SDK/lib${{LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}}
    export VK_ADD_LAYER_PATH=$VULKAN_SDK/share/vulkan/explicit_layer.d
    # <<< Vulkan SDK exports <<<
    """.strip()

        # Check if already present
        if shell_rc.exists() and export_block.splitlines()[1] in shell_rc.read_text():
            print(f"✔️  Vulkan SDK export block already exists in {shell_rc}")
        else:
            with open(shell_rc, "a") as f:
                f.write("\n\n" + export_block + "\n")
            print(f"✅ Added Vulkan SDK exports to {shell_rc}")

        print("ℹ️  Restart your terminal or run `source ~/.bashrc` (or relevant shell) to apply.")


    @classmethod
    def PrintSystemPrerequisites(cls,distro):
        if "UBUNTU" in distro:
            print("You need to install the following packages if the script fails and run the script again:")
            print("sudo apt install xz-utils \
             sudo apt-get install libglm-dev cmake libxcb-dri3-0 libxcb-present0 libpciaccess0 \
             libpng-dev libxcb-keysyms1-dev libxcb-dri3-dev libx11-dev g++ gcc \
             libwayland-dev libxrandr-dev libxcb-randr0-dev libxcb-ewmh-dev \
             git python-is-python3 bison libx11-xcb-dev liblz4-dev libzstd-dev \
             ocaml-core ninja-build pkg-config libxml2-dev wayland-protocols python3-jsonschema \
             clang-format qtbase5-dev qt6-base-dev")
            print("sudo apt install libxcb-xinput0 libxcb-xinerama0 libxcb-cursor-dev")

            return
        elif "FEDORA" in distro:
            print("You need to install the following packages if the script fails and run the script again:")
            print("sudo dnf install xz")
            print("sudo dnf install @development-tools glm-devel cmake libpng-devel wayland-devel \
                    libpciaccess-devel libX11-devel libXpresent libxcb xcb-util libxcb-devel libXrandr-devel \
                    xcb-util-keysyms-devel xcb-util-wm-devel python3 git lz4-devel libzstd-devel python3-distutils-extra qt \
                    gcc-g++ wayland-protocols-devel ninja-build python3-jsonschema qt5-qtbase-devel qt6-qtbase-devel")
            print("sudo dnf install xinput libXinerama xcb-util-cursor")
            return
        elif "ARCH" in distro:
            print("You need to install the following packages if the script fails and run the script again:")
            print("sudo pacman -S xz")
            print("sudo pacman -S base-devel glm cmake libpng wayland libpciaccess libx11 libxpresent \
                    libxcb xcb-util libxrandr xcb-util-keysyms xcb-util-wm python git lz4 zstd python-distlib qt5-base \
                    wayland-protocols ninja")
            print("sudo pacman -S libxcb libxinerama xcb-util-cursor")

    @classmethod
    def HandleLinuxInstall(cls, directory, version):
        distro = cls.GetCurrentDistro().upper()
        print(f"Detected distro: {distro}")
        cls.PrintSystemPrerequisites(distro)
        cls.AppendVulkanExports(version, directory)


    @classmethod
    def Update(cls, install_dir, platform, oldPath , oldVersion, newVersion):
        pass

    @classmethod
    def ValidatePlatform(cls,platform):
        if platform not in ["windows","linux","mac"]:
            raise Exception(f"Unsupported platform: {platform}")

    @classmethod
    def Validate(cls, install_dir, platform):
        cls.ValidatePlatform(platform)

        current_sdk = cls.GetCurrentVersion(install_dir)
        latest_version = cls.GetLatestVersion(platform)
        if not current_sdk:
            print("Vulkan SDK is not installed.")
            print(f"Do you want to install it? (latest version: {latest_version})")
            if input("Y/N: ").lower() == "y":
                cls.Install(install_dir, platform,latest_version)
        else:
            print("Vulkan SDK is already installed.")
            print("Current version is:", current_sdk["version"])
            print("Checking for updates...")
            if current_sdk["version"] != latest_version:
                print("Vulkan SDK update available.")
                print("Do you want to update it?")
                if input("Y/N: ").lower() == "y":
                    print("Updating Vulkan SDK to {}...", latest_version)
                    cls.Update(install_dir,current_sdk["path"], platform,current_sdk["version"],latest_version)
            else:
                print("Vulkan SDK is up to date.")

def main():

    current_platform = sys.platform
    if current_platform == "win32":
        current_platform = "windows"
    elif current_platform == "linux":
        current_platform = "linux"
    elif current_platform == "darwin":
        current_platform = "mac"
    else:
        raise Exception(f"Unsupported platform: {current_platform}")

    parser = argparse.ArgumentParser(description="A script to setup Vulkan SDK.")

    subparsers = parser.add_subparsers(dest='command')

    check_latest_parser = subparsers.add_parser('checklatest', help='Check latest available version of Vulkan SDK.')
    check_latest_parser.add_argument('platform', choices=['windows', 'linux', 'mac'],default=current_platform,nargs='?', help='Platform to check the Vulkan SDK version for.')
    
    check_version_parser = subparsers.add_parser('check', help='Check current version of Vulkan SDK.')
    check_version_parser.add_argument('install_dir', default=".",nargs='?' ,help='Path to the Vulkan SDK installation directory.')

    validate_parser = subparsers.add_parser('validate', help='Validate if Vulkan SDK is installed and up to date.')
    validate_parser.add_argument('install_dir', default=".",nargs='?' ,help='Path to the Vulkan SDK installation directory.')
    validate_parser.add_argument('platform', choices=['windows', 'linux', 'mac'],default=current_platform,nargs='?', help='Platform to check the Vulkan SDK version for.')

    args = parser.parse_args()

    if args.command == 'checklatest':
        print("Checking latest version of Vulkan SDK...")
        latest_version = VulkanConfigurator.GetLatestVersion(args.platform)
        print(f"Latest version of Vulkan SDK is: {latest_version}")
    elif args.command == 'check':
        print("Checking current version of Vulkan SDK...")
        current_sdk_version = VulkanConfigurator.GetCurrentVersion(args.install_dir)
        if not current_sdk_version:
            print("Vulkan SDK is not installed.")
            sys.exit(1)
        print(f"Current version of Vulkan SDK is: {current_sdk_version['version']} at {current_sdk_version['path']}")
    elif args.command == 'validate':
        print("Validating Vulkan SDK...")
        VulkanConfigurator.Validate(args.install_dir, args.platform)
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == "__main__":
    main()