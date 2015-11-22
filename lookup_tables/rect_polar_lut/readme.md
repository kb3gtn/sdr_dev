s polar to rectangular conversion table

## Number system
The code in this directory defines an signed 8-bit x,y coordinate.  This has the values of:   -127>=x>=127 and -127>=y>=127  of whole integer numbers.

Any x,y vector can also be represented in polar coordinate system.  The polar coordinate system has a magnitude and angle components.
Magnitude and Angle are defined as a unsigned 8-bit numbers.   Magnitude has the range of 0 -> 180 and angle has the range of 0 -> 255.

Angle used here is something call Normalized Radian Angle (NRA).   This is simply just a normalized mapping of 0->2*PI to the range 0->255.

 Simple Lookup table:
| Degrees | Radians | NRA |
|---------|---------|-----|
| 0       | 0       | 0   |
| 45      | PI 1/4  | 32  |
| 90      | PI 1/2  | 64  |
| 135	  | PI 3/4  | 96  |
| 180     | PI      | 128 |
| 225     | PI 5/4  | 160 |
| 270     | PI 6/4  | 192 |
| 315     | PI 7/4  | 224 |
| 360     | 2 PI    | 0   |





