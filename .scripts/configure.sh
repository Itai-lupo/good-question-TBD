#!/bin/bash

#Declare list of dependencies
declare -Ag deps=([spdlog]='spdlog' [glm]='glm' [gtest]="gtest")

#Declare list of package managers and their usages
declare -Ag packman_list=([pacman]='pacman -Sy' [apt]='echo "deb http://deb.debian.org/debian buster-backports main contrib non-free" > /etc/apt/sources.list.d/buster-backports.list; apt update -y; apt install -y' [yum]='yum install -y epel-release; yum repolist -y; yum install -y')

#Find the package manager on the system and install the package
install_deps(){
    for packman in ${!packman_list[@]}
    do
        which $packman &>/dev/null && eval $(echo ${packman_list[$packman]} "$*")
    done
}

#Find the missing packages from list of dependencies
declare -ag missing_deps=()
for pack in ${!deps[@]}
do
    which $pack &>/dev/null || missing_deps+=(${deps[$pack]})
done

#Install missing dependenci
test -z ${missing_deps[0]} || install_deps ${missing_deps[@]} 
