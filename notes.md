# FPS NOTES

Just FPS counter -> 190fps

cvtColour dropped to 70;
so using extractChannel instead -> 160;
in the future could avoid the copy by using a direct view into memory of original frame?




# Later efficency


## Frame Mask
is created on every init;
the mask could be created only once and applied repleatedly;

## Frame Display image
curr each og img is stored in memory
can just draw ontop of what is used for comp and throw out og img