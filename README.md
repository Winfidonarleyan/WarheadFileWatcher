# WarheadFileWatcher

## Build requirements

1. Ubuntu

```sh
sudo apt-get install git clang cmake make gcc g++
```

2. Manjaro Linux

```sh
sudo pacman -S git clang cmake make
```

## Build process

```sh
git clone https://github.com/Winfidonarleyan/WarheadFileWatcher
cd WarheadFileWatcher
mkdir build
cd build
cmake ../ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=../../wfw
make -j 4 && make install
cd ..
```

A new folder should appear next to the repository `wfw`

## Create config file (optional)
1. Move to `.../wfw`
2. Copy dist config
* Location in github - **[WarheadFileWatcher.conf.dist](https://github.com/Winfidonarleyan/WarheadFileWatcher/blob/master/src/app/WarheadFileWatcher.conf.dist)**
* Location in code `.../WarheadFileWatcher/src/app/WarheadFileWatcher.conf.dist`
3. Replace file name to `WarheadFileWatcher.conf`
4. Change options if need

## Run server
1. Move to `.../wfw/bin`
2. `./WarheadFileWatcher`
