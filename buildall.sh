rm -rf ./build
make -C ./leveldb/ clean

mkdir build
mkdir build/tests
make -C ./varint/ all
make -C ./leveldb/ all
make -C ./src/ all
