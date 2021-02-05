# Butler HTTP Client and Webserver

This repo will contain all coursework for CSEC-731

## Setup
Please read everything below before installing any software.
1) Setup your environment:
   * In order to ensure that you will be able to successfully build the binaries here you must install the following:
     * `make`
     * `cmake` **SEE BELOW**
     * `libssl-dev`
     * `libfmt-dev`
     * `gcc`
     * `build-essential`
2) Installing `cmake`:
   * I am using the latest version of `cmake` _3.19_ for this project. In order to install this version on Ubuntu 18 you must install it via snaps `sudo snap install cmake` or download a tar file and build it yourself. 
   * If you would like to build it yourself head over to this link (https://cmake.org/download/), download the Ubuntu tar file, unzip and untar the file, then `cd` into the cmake directory and run `./boostrap`. Once this is done running it will prompt you to run `make`.
3) To quickly get everything installed that you need you can run the `setup.sh` script contained in this repo. It will install everything that you need to build the project.

## Building and Executing

1) Once everything is installed execute the following commands:
```
mkdir build
cd build
cmake ..
make
./bin/client https://www.rit.edu
```

This will build the project and use the HTTP client to make a request to `https://www.rit.edu`

    