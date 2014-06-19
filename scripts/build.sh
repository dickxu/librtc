#!/bin/bash

ROOT=`pwd`/..

usage() {
    echo "usage: $0 ios|ios-sim|mac|android|unix [webrtc|xrtc|clean]"
}

main() {
    [ $# -lt 1 ] && usage && exit 1

    pushd $ROOT/scripts
    source $ROOT/scripts/env_ninja.sh
    if [ $1 = "ios" ]; then
        wrios 
    elif [ $1 = "ios-sim" ]; then
        wrsim
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
