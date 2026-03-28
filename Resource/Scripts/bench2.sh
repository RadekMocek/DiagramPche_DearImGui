#!/bin/bash

printf "== DiagramPche_DearImGui benchmarks ==\n"
printf "\nThis script:\n\t* will run the complete 3in1 benchmark with SHON and TXOFF\n\t* expects to be run from the ./Resources/Scripts folder.\n"
read -p "Press enter to continue. Press Ctrl+C to cancel."

cd ..
cd ..

printf "[1/2] BENCH NODES COMPLETE 3in1 YES_SYNTAX_HIGHLIGHT\n"
./DiagramPche_DearImGui b 3 1

printf "[2/2] BENCH NODES COMPLETE 3in1 NO_TEXT_EDITOR\n"
./DiagramPche_DearImGui b 3 2

printf "====================\nAll benchmarks done.\nPress enter to exit.\n"
read -p ""
