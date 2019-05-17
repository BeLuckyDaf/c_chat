#!/bin/bash

# eval $(ssh-agent)
# ssh-add ~/beluc_rsa
# git pull
# git submodule update --init --recursive

# MAKE SURE YOU'VE GOT YOUR RPMBUILD AT ~/rpmbuild
# AND THERE IS THE cchat.spec FILE

mkdir -p cchat-1.0
cp Makefile cchat-1.0
cp -r src cchat-1.0
tar -zcvf cchat-1.0.tar.gz cchat-1.0
rm -r cchat-1.0
mv cchat-1.0.tar.gz ~/rpmbuild/SOURCES

rpmbuild -ba --target=x86_64 ~/rpmbuild/SPECS/cchat.spec
rpmbuild -ba --target=i386 ~/rpmbuild/SPECS/cchat.spec
