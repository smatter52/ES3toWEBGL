# ES3toWEBGL

## OpenGles3 to WebGl2 covertor

The GL3Web.c file replaces the GLESv2 library in the compile and link process.
It intercepts the gl<X> function calls and spits out gl.<x>  WebGL2 javascript.
This is mostly a one to one subsitution, but additional code handles gl C pointer
data conversion to javascript text arrays.


## Use

The Javascript Webgl output together with a HTML header and footer creates a HTML webpage copy
of the OpenGLes3 screen draw. Have fun.

## Example
I have included one of my test page HTML output.

Chief Engineer