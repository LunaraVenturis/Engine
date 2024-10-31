"""
Python script to setup Vulkan development environment,
and install required libraries if they are not already installed.
Written by Krasto
"""

import os
import sys
import subprocess
import re
from SetupPython import PythonConfiguration as PythonRequirements

# Make sure everything we need for the setup is installed
required_packages = ["urllib3", "zipfile", "shlex","shutil"]
PythonRequirements.Validate(required_packages)

import Utils

from colorama import *

init()

from SetupVulkan import VulkanConfiguration as VulkanRequirements

VulkanRequirements.Validate("../Vendor/")

path = str(os.environ.get("VULKAN_SDK")).strip()
filedata = ""

path = re.sub(r"\\","/",path)

print(f"{Fore.BLUE}Replacing VulkanSDK path in CMakeLists.txt{Style.RESET_ALL}")
with open('../CMakeLists.txt', 'r') as file :
    found = False
    for line in file:
        if(found):
            filedata += "set(GLFW_VULKAN_INCLUDE_DIR    \"" + path.strip() + "\")\n\n"
            found = False
        else:
            filedata += line
        if(line.strip().startswith("## __Vulkan")):
            found = True

with open('../CMakeLists.txt', 'w') as file :
    file.write(filedata)

# Done
print(f"{Fore.GREEN}Done!{Style.RESET_ALL}")