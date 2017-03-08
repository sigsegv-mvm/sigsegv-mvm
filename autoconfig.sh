#!/bin/bash

CONFIGURE=$(realpath configure.py)
PATHS="--hl2sdk-root=/pool/Game/SourceMod --mms-path=/pool/Game/SourceMod/mmsource --sm-path=/pool/Game/SourceMod/sourcemod"

mkdir -p build
pushd build
	$CONFIGURE $PATHS --sdks=tf2 --enable-debug
popd

mkdir -p build/release
pushd build/release
	$CONFIGURE $PATHS --sdks=tf2 --enable-debug --enable-optimize --exclude-mods-debug --exclude-mods-visualize
popd

mkdir -p build/clang
pushd build/clang
	CC=clang CXX=clang++ $CONFIGURE $PATHS --sdks=tf2 --enable-debug
popd
