Write-Host "== DiagramPche_DearImGui benchmarks =="
Write-Host "`nThis script:`n`t* will run the complete 3in1 benchmark with SHON and TXOFF`n`t* expects to be run from the .\Resources\Scripts folder."
Write-Host "Press enter to continue. Press Ctrl+C to cancel."
Read-Host

cd..
cd..

Write-Host "[1/2] BENCH NODES COMPLETE 3in1 YES_SYNTAX_HIGHLIGHT"
.\DiagramPche_DearImGui.exe b 3 1

Write-Host "[2/2] BENCH NODES COMPLETE 3in1 NO_TEXT_EDITOR"
.\DiagramPche_DearImGui.exe b 3 2

Write-Host "====================`nAll benchmarks done.`nPress enter to exit."
Read-Host
