#ifndef GLOBALS_H
#define GLOBALS_H

enum Directions
{ // bitwise ORed
    Up    = 1,
    Right = 2,
    Down  = 4,
    Left  = 8,
    
    None  = 0
};

// TODO: use settings??
// easy to convert to bool ("isWrapped = wrapping;")
enum Wrapping {NotWrapped = 0, Wrapped = 1};

#endif // GLOBALS_H
