rm -rf ./build
make -C ./leveldb-1.7.0/ clean

mkdir build
mkdir build/tests
make -C ./varint/ all
make -C ./leveldb-1.7.0/ all
make -C ./src/ all