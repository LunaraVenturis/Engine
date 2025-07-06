if [ -d "./Vendor" ]; then
    cd ./Vendor
    rm -rf ./glm ./SDL ./SDL_ttf
    git submodule update --init --recursive
else
    mkdir ./Vendor
    cd ./Vendor
    git submodule update --init --recursive
fi