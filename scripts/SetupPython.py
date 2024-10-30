import sys
import subprocess
import importlib.util as importlib_util
import importlib

class PythonConfiguration:
    @classmethod
    def Validate(cls,packageList):

        if not cls.__ValidatePackage("colorama"):
            return
        if not cls.__ValidatePackage("requests"):
            return

        cls.colors = importlib.import_module("colorama","*")
        cls.colors.init()
        __import__("requests") 

        if not cls.__ValidatePython():
            return # cannot validate further
        if not cls.__ValidatePackage("conan"):
            return


        print("Needed Packages:")
        packageList = ["requests"] + packageList

        for packageName in packageList:
            print("     " + packageName)
            if not cls.__ValidatePackage(packageName):
                return # cannot validate further

    @classmethod
    def __ValidatePython(cls, versionMajor = 3, versionMinor = 3):
        if sys.version is not None:
            print(f"{cls.colors.Fore.BLUE}Python version {0:d}.{1:d}.{2:d} detected.{cls.colors.Style.RESET_ALL}".format( \
                sys.version_info.major, sys.version_info.minor, sys.version_info.micro))
            if sys.version_info.major < versionMajor or (sys.version_info.major == versionMajor and sys.version_info.minor < versionMinor):
                print(f"{cls.colors.Fore.YELLOW}Python version too low, expected version {0:d}.{1:d} or higher.{cls.colors.Style.RESET_ALL}".format( \
                    versionMajor, versionMinor))
                return False
            return True

    @classmethod
    def __ValidatePackage(cls, packageName):
        if importlib_util.find_spec(packageName) is None:
            return cls.__InstallPackage(packageName)
        return True

    @classmethod
    def __InstallPackage(cls, packageName):
        print(f"Installing {packageName} module...")
        subprocess.check_call(['python', '-m', 'pip', 'install', packageName])

        return cls.__ValidatePackage(packageName)

if __name__ == "__main__":
    PythonConfiguration.Validate()
