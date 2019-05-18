# CChat
A simple chat application written in C. It disables canonical mode and echo and provides simple typing control.

## Installing
There are three ways to install the application.
* Pre-built binary
* RPM package
* Build from source

## Building
In this section you can find out how to build the binaries and the RPMs.

### Binary
In order to compile the source code, you need to have `gcc` installed and then 
simply run `make` in the root directory of the repository. The resulting executable 
could then be found at `/build`.

### RPMs
The `cchat.spec`, that is used as a description for an RPM package can be found at `/rpm`.
You need to have that file placed as following `/path/to/rpmbuild/SPECS/cchat.spec`.

You also need to put a tarball with the source code at `/path/to/rpmbuild/SOURCES/cchat-<version>.tar.gz`, 
satisfying the version written in the `cchat.spec`.

After all of that is done you can run `rpmbuild -ba --target=<arch> /path/to/rpmbuild/SPECS/cchat.spec`, specifying the
desired architecture, which could be `i386` or `x86_64` for example.

You will find the RPMs at `/path/to/rpmbuild/RPMS`.

Note: you might need to install `gcc.i386` for 32-bit binaries.

Note: if you have rpmbuild directory at `~/rpmbuild`, you could use the provided shell script,
simply run `./build_rpm.sh` in the root directory of the repository. The script will run `git pull`,
create a tarball with the source code, put it into the SOURCES directory and run `rpmbuild ...`.

## Development
You can use CLion and open the CMake project, hence the `CMakeLists.txt`.

## Usage
Simply run the application and follow the instructions.
