Write-Host "== DiagramPche_DearImGui benchmarks =="
Write-Host "`nThis script:`n`t* will run all the possible benchmarks`n`t* expects to be run from the .\Resources\Scripts folder."

Write-Host "`n[!] Epilepsy/seizure warning: benchmarks (especially the last one!) contain flashing images.`n"
Write-Host "Press enter to continue. Press Ctrl+C to cancel."
Read-Host

$i = 1
$j = 10
cd..
cd..

Write-Host "[${i}/${j}] BENCH NODES LIGHT NO_SYNTAX_HIGHLIGHT"
.\DiagramPche_DearImGui.exe b 0 0
$i++

Write-Host "[${i}/${j}] BENCH NODES LIGHT YES_SYNTAX_HIGHLIGHT"
.\DiagramPche_DearImGui.exe b 0 1
$i++

Write-Host "[${i}/${j}] BENCH NODES LIGHT NO_TEXT_EDITOR"
.\DiagramPche_DearImGui.exe b 0 2
$i++

Write-Host "[${i}/${j}] BENCH NODES HEAVY NO_SYNTAX_HIGHLIGHT"
.\DiagramPche_DearImGui.exe b 1 0
$i++

Write-Host "[${i}/${j}] BENCH NODES HEAVY YES_SYNTAX_HIGHLIGHT"
.\DiagramPche_DearImGui.exe b 1 1
$i++

Write-Host "[${i}/${j}] BENCH NODES HEAVY NO_TEXT_EDITOR"
.\DiagramPche_DearImGui.exe b 1 2
$i++

Write-Host "[${i}/${j}] BENCH NODES GRADUAL NO_SYNTAX_HIGHLIGHT"
.\DiagramPche_DearImGui.exe b 2 0
$i++

Write-Host "[${i}/${j}] BENCH NODES GRADUAL YES_SYNTAX_HIGHLIGHT"
.\DiagramPche_DearImGui.exe b 2 1
$i++

Write-Host "[${i}/${j}] BENCH NODES GRADUAL NO_TEXT_EDITOR"
.\DiagramPche_DearImGui.exe b 2 2
$i++

Write-Host "[${i}/${j}] BENCH WIDGETS"
.\DiagramPche_DearImGui.exe w

Write-Host "====================`nAll benchmarks done.`nPress enter to exit."
Read-Host
