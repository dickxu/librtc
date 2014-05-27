#!/bin/sh

function wrinit() {
    export GYP_DEFINES=""
    export GYP_GENERATORS=""
    export GYP_GENERATOR_FLAGS=""
    export GYP_CROSSCOMPILE=0
    export TARGET=""
}

function wrbase() {
    wrinit
    export GYP_DEFINES="build_with_libjingle=1 build_with_chromium=0 libjingle_objc=1"
    export GYP_GENERATORS="ninja"
}

function wrios() {
    wrbase
    export OUT_DIR="out_ios"
    export GYP_DEFINES="$GYP_DEFINES OS=ios target_arch=armv7"
    export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_ios"
    export GYP_CROSSCOMPILE=1
    export TARGET=IOS
}

function wrsim() {
    wrbase
    export OUT_DIR="out_sim"
    export GYP_DEFINES="$GYP_DEFINES OS=ios target_arch=ia32"
    export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_sim"
    export GYP_CROSSCOMPILE=1
    export TARGET=IOS-SIM
}

function wrmac() {
    wrbase
    export OUT_DIR="out_mac"
    export GYP_DEFINES="$GYP_DEFINES OS=mac target_arch=x64"
    export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_mac"
    export TARGET=MAC
}

function wrandroid() {
    wrinit
    export OUT_DIR="out_android"
    export GYP_DEFINES="build_with_libjingle=1 build_with_chromium=0"
    export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_android"
    export TARGET=ANDROID
}

function wrunix() {
    wrinit
    export OUT_DIR="out_unix"
    export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_unix"
    export TARGET=UNIX
}

