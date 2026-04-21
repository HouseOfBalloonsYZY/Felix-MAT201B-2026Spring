# Assignment 1

## Questions for Karl

1. Little confused about `faceToward (const Vec3d &point, const Vec3d &up, double amt=1.)` and `faceTowardLine (const Vec3d &point, const Vec3d &dir, double amt=1.)` could we re-explain?

2. Still confused about the unit in openGL. Could we re-explain it? If it is such an arbiturary thing why don't we mutiply everything by 10000 or sth so that everything besides unit vectors can be stored in integers? Sry kinda dumb question...

3. Interesting finding: my bluetooth headphone breaks the program. The error code was
    ```
    RtApiCore::probeDeviceOpen: system error (kAudioHardwareIllegalOperationError) setting sample rate for device (1). 
    RtApiCore::probeDeviceOpen: system error (kAudioHardwareIllegalOperationError) setting sample rate for device (1). 
    RtApi:: a stream is not open!
    ```
    AI says it's because "indicates that the software is trying to set a sample rate (e.g., 44.1 kHz or 48 kHz) that your current audio hardware or driver cannot accept"