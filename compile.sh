gcc="gcc"
warning_options="-Wall -pedantic-errors -Wunused-result -Wextra -Werror -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wno-unused-parameter"

bitmode=64
include_paths="-Iinclude -Isrc -Isrc/scenes -Isrc/tests"
libs="-L/lib/x86_64-linux-gnu/ `sdl2-config --cflags --libs --static-libs` -lSDL2_mixer -lSDL2_image"
options="-fdiagnostics-color=always -fmax-errors=1 -Wfatal-errors"

defines="-D_"

files="*.c"
output="out/main"

clear && $gcc -g -m$bitmode $defines $warning_options $include_paths $files -o $output $libs $options
