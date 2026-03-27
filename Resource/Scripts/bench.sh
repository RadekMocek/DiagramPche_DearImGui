#!/bin/bash

printf "== DiagramPche_DearImGui benchmarks ==\n"
printf "\nThis script:\n\t* will run all the possible benchmarks\n\t* expects to be run from the ./Resources/Scripts folder.\n"

printf "\n[!] Epilepsy/seizure warning: benchmarks (especially the last one!) contain flashing images.\n\n"
read -p "Press enter to continue. Press Ctrl+C to cancel."

i=1
j=10
cd ..
cd ..

printf "[$i/$j] BENCH NODES LIGHT NO_SYNTAX_HIGHLIGHT\n"
./DiagramPche_DearImGui b 0 0
((i++))

printf "[$i/$j] BENCH NODES LIGHT YES_SYNTAX_HIGHLIGHT\n"
./DiagramPche_DearImGui b 0 1
((i++))

printf "[$i/$j] BENCH NODES LIGHT NO_TEXT_EDITOR\n"
./DiagramPche_DearImGui b 0 2
((i++))

printf "[$i/$j] BENCH NODES HEAVY NO_SYNTAX_HIGHLIGHT\n"
./DiagramPche_DearImGui b 1 0
((i++))

printf "[$i/$j] BENCH NODES HEAVY YES_SYNTAX_HIGHLIGHT\n"
./DiagramPche_DearImGui b 1 1
((i++))

printf "[$i/$j] BENCH NODES HEAVY NO_TEXT_EDITOR\n"
./DiagramPche_DearImGui b 1 2
((i++))

printf "[$i/$j] BENCH NODES GRADUAL NO_SYNTAX_HIGHLIGHT\n"
./DiagramPche_DearImGui b 2 0
((i++))

printf "[$i/$j] BENCH NODES GRADUAL YES_SYNTAX_HIGHLIGHT\n"
./DiagramPche_DearImGui b 2 1
((i++))

printf "[$i/$j] BENCH NODES GRADUAL NO_TEXT_EDITOR\n"
./DiagramPche_DearImGui b 2 2
((i++))

printf "[$i/$j] BENCH WIDGETS"
./DiagramPche_DearImGui w

printf "====================\nAll benchmarks done.\nPress enter to exit.\n"
read -p ""
