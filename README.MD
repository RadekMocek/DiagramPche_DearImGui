# DiagramPche :: Dear ImGui

This is part of a IMGUI paradigm research, [go back to main repo](https://github.com/RadekMocek/DP).

* Jump to:
  * [Building locally](#building-locally)
  * [Dependencies](#dependencies)
  * [Problems](#problems)

## TODO

* [x] Benchmark csv filename should be `./bnchres_{framework}_{os_id}_{bench_id}_{sh_info}_{timestamp}.csv`
* [x] Accept cmd args to start benchmark immediatelly
* [ ] Widgetbench – benchmark that creates window full of widgets and measures to time to show&hide it
  * Maximize main window
  * BenchWin must be as top left as possible
  * Run from terminal

## Building locally

### Debian based distros

```bash
sudo apt install cmake ninja-build libdbus-1-dev

# Navigate to the root directory of this repository

cmake -S . -B build

cmake --build build
```

### Windows

* The most straightforward way is probably to use an IDE:
  * In CLion, go to _File_ → _Open..._ and select root directory of this repository
  * In Visual Studio's launcher, click on the _Open a local folder_ button and select root directory of this repository

## Dependencies

* [Dear ImGui](https://github.com/ocornut/imgui) ([MIT](https://github.com/ocornut/imgui/blob/master/LICENSE.txt))
* [GLFW](https://www.glfw.org/) ([zlib](https://www.glfw.org/license))
* [glad](https://github.com/Dav1dde/glad) ([MIT](https://github.com/Dav1dde/glad/blob/glad2/LICENSE))
* [FreeType](https://gitlab.freedesktop.org/freetype/freetype) ([Custom](https://gitlab.freedesktop.org/freetype/freetype/-/blob/master/docs/FTL.TXT))
* [toml++](https://github.com/marzer/tomlplusplus) ([MIT](https://github.com/marzer/tomlplusplus/blob/master/LICENSE))
* [stb](https://github.com/nothings/stb) ([MIT](https://github.com/nothings/stb/blob/master/LICENSE))
* [simple-svg](https://github.com/adishavit/simple-svg) ([BSD-3-Clause](https://github.com/adishavit/simple-svg/blob/master/LICENSE))
* [Native File Dialog Extended](https://github.com/btzy/nativefiledialog-extended) ([zlib](https://github.com/btzy/nativefiledialog-extended/blob/master/LICENSE))
* [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit) ([MIT](https://github.com/BalazsJako/ImGuiColorTextEdit/blob/master/LICENSE))
* [IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders) ([zlib](https://github.com/juliettef/IconFontCppHeaders/blob/main/licence.txt))
* David Robert Nadeau's `getCurrentRSS` (CC BY 3.0)
* [cpp-linux-system-stats](https://github.com/improvess/cpp-linux-system-stats) ([Apache-2.0](https://github.com/improvess/cpp-linux-system-stats/blob/main/LICENSE))

<br>

* [Inconsolata](https://levien.com/type/myfonts/inconsolata.html) ([SIL Open Font](https://github.com/google/fonts/blob/main/ofl/inconsolata/OFL.txt))
* [Pictogrammers Material Design Icons](https://pictogrammers.com/library/mdi/) ([Apache-2.0](https://pictogrammers.com/docs/general/license/))

<br>

* Changes done to _simple-svg_:
  * Modified to support transparency (not very pretty)
* Changes done to _ImGuiColorTextEdit_:
  * Added support for NSOLD (the diagram language)
  * Added a custom color palette
  * Disabled line numbers
  * Disabled error and identifier tooltips
  * Removed `ImGuiWindowFlags_AlwaysHorizontalScrollbar` flag
  * Added `GetErrorMarkersRef` function
  * Pressing <kbd>Tab</kbd> inserts four spaces instead of `\t`
  * Accept the numpad enter as well as the "normal" enter

## Problems

* Design problems:
  * No undo for changing node's color via the color picker, changing color even clears the history 😬
* InputTextMultiline problems:
  * Loses focus on AltGR
  * Horizontal scrolling is problematic [...](https://github.com/hoffstadt/DearPyGui/issues/2381#issuecomment-2303922790)
  * Scrollbar flashing while typing
* ImGuiColorTextEdit problems:
  * Scrolling by holding up/down arrow key weird "jumpy" behavior, it's even worse with horizontal scroll (right arrow), it does not do that in their demo, but that's older version on older Dear ImGui
  * No Ctrl + Backspace for removing the whole word (same for Delete)
  * Selecting text by holding LMB and dragging pauses when curosr is out of the widget area
  * Sometimes it thinks the "#RRGGBBAA" string is comment (TOML comments start with '#'), comments with `"` also not ideal
  * Setting curosr position when the text is selected will move the cursor, but won't unselect the text and won't highlight the new line
* Error highlight breaks on tab (can fix by counting tabs (`\t`) on the row with error and shifting EH_x accordingly)
* Hacky SVG edit
  * It seems that only modern web browsers support svg fill transparency via rgba(r,g,b,a) or #RRGGBBAA, so I'm using the style="..." option, which the svg library wasn't made for. The way I added it is not very pretty.
  * ⇒ Exported SVG currently does not support node border transparency (alpha is always 255, this is true for Nodes and Path labels' background)
