#!/bin/bash

set -e

rm -rf bootlogo-mod
mkdir bootlogo-mod
cp iso-stuff/isolinux/bootlogo .
cd bootlogo-mod
7z x ../bootlogo
cp ../iso-stuff/isolinux/txt.cfg .
ls | cpio -o > ../bootlogo
cd ..
mv bootlogo iso-stuff/isolinux/
