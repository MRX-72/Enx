#!/bin/bash

clear

printf "\n\n[Installer]> Installing required packages";

apt-get install clang

printf "\n\n[Installer]> Installed required packages\n";

g++ EncDec.cpp -o Enx

mv Enx $PATH

printf "\n Setup has completed.\n\n"

printf "Type: Enx --help from any directory \n\n"
