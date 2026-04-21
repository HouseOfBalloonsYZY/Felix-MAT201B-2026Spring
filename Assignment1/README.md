# Assignment 1

## Questions for Karl Week 4 Tuesday 4/21/26

### p1.cpp

1. Little confused about `faceToward (const Vec3d &point, const Vec3d &up, double amt=1.)` and `faceTowardLine (const Vec3d &point, const Vec3d &dir, double amt=1.)` could we re-explain?

2. Still confused about the unit in openGL. Could we re-explain it? If it is such an arbiturary thing why don't we mutiply everything by 10000 or sth so that everything besides unit vectors can be stored in integers? Sry kinda dumb question...

3. Interesting finding: my bluetooth headphone breaks the program. The error code was
    ```
    RtApiCore::probeDeviceOpen: system error (kAudioHardwareIllegalOperationError) setting sample rate for device (1). 
    RtApiCore::probeDeviceOpen: system error (kAudioHardwareIllegalOperationError) setting sample rate for device (1). 
    RtApi:: a stream is not open!
    ```
    AI says it's because "indicates that the software is trying to set a sample rate (e.g., 44.1 kHz or 48 kHz) that your current audio hardware or driver cannot accept"


### p3.cpp

4. Does nudge also change speed or just position?

5. The red lines of code in editors other than vs code (say cursor) is very annoying. Also how come it is possible for filed to find `#include "al//xxx//xxx.hpp` in any folder inside `allolib_playground/` ? And what should I do if some editors can't find it.

6. The `while (distance > aloofDistance)` problem. I can't see where I had a infinite loop.