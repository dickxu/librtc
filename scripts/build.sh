#!/bin/bash

ROOT=`pwd`/..

proc_ios_lipo ()
{
    [ "$BUILD_TYPE" = "" ] && BUILD_TYPE="Release"

    mod_list="ubase rtc webrtc_all"
    for mod in $mod_list; do
        fname_armv7=$ROOT/libs/out_ios/${BUILD_TYPE}-iphoneos/lib${mod}.a
        fname_i386=$ROOT/libs/out_sim/${BUILD_TYPE}/lib${mod}.a
        [ ! -f $fname_armv7 -o ! -f $fname_i386 ] && return
        lipo_param="-arch armv7 $fname_armv7 -arch i386 $fname_i386"
        mkdir -p $ROOT/libs/ios && lipo $lipo_param -output $ROOT/libs/ios/lib${mod}.a -create
    done
}

usage() {
    echo "usage: $0 ios|ios-sim|ios-lipo|mac|android|unix [webrtc|xrtc|clean]"
}

main() {
    [ $# -lt 1 ] && usage && exit 1

    pushd $ROOT/scripts
    source $ROOT/scripts/env_ninja.sh
    if [ $1 = "ios" ]; then
        wrios 
    elif [ $1 = "ios-sim" ]; then
        wrsim
    elif [ $1 = "ios-lipo" ]; then
        proc_ios_lipo 
        exit 0
    elif [ $1 = "mac" ]; then
        wrmac
    elif [ $1 = "android" ]; then
        wrandroid
    elif [ $1 = "unix" ]; then
        wrunix
    else
        usage && exit 1
    fi

    if [ $# -eq 2 ]; then
        sh run_cfg.sh $2
    else
        sh run_cfg.sh webrtc && sh run_cfg.sh xrtc
    fi
    popd
}

main $*
exit 0
