# Mempool for a real-time application

## How to run it (on Ubuntu/Debian)

```shell
sudo apt update
sudo apt install libfmt-dev
git clone https://github.com/SzymonZos/Mempool.git
cd Mempool
cmake -B build
cmake --build build
./build/test/mempool_tests
```
