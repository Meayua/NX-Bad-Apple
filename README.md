# NX-Bad-Apple
the bad apple for the nintendo switch

# Compiling

## Requirements

devkitpro with switch libraries and Bad Apple MV converted to mpeg1video (https://www.youtube.com/watch?v=FtutLA63Cp8 convert with ffmpeg using `ffmpeg -i input.mp4 -vcodec mpeg1video -acodec mp2 -format mpeg -vf "scale=1280:720:force_original_aspect_ratio=decrease,pad=1280:720:(ow-iw)/2:(oh-ih)/2,setsar=1" output.mpg`)

## Building

1. Place the converted MV in romfs (overwrite the one currently there)
2. In the root of the project, run `make`.

You get the resulting .nro in the root.
