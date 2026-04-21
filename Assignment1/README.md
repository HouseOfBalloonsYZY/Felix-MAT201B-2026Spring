# Assignment 1

Implement an interesting agent simulation in AlloLib. Use `al::Nav` as the basis of each agent. First implement *random chasing* where each agent “loves” some other random agent, following that agent wherever it goes: 1) turn a little toward your love and 2) move in the forward direction. Call this code `p1.cpp`. 

Next, add code that maintains distance between agents. For each agent, search through all the other agents to find those that are closer than some threshold value. Nudge away from these too close neighbors. Call this code `p2.cpp`. 

Next, abandon love as a concept. Instead, make the agents “flock” by 
1) adjusting their heading to match the average heading of their neighbors, 
2) maintaining a small distance from their neighbors, while 
3) maintaining proximity to their neighbors by nudging toward their average position. 
For each agent, find up to K neighbors closer than some threshold distance T. Compute the average position of the neighbors and their average heading. If the agent is too far away from the center of the neighbors, nudge toward the center. If the agent is too close to any neighbor, nudge away from that neighbor. Turn a little to match the average heading. The agents may wander off into the distance, in different directions, so implement [wrap around](https://en.wikipedia.org/wiki/Wraparound_(video_games)). Call this code `p3.cpp`. 

Now add something interesting of your own design. Perhaps:

- Introduce food into the system
- Add obstacles
- Find some way to keep agents contained without wrap around
- Add lions that eat flocking agents
- Make agents fight for survival
- Give agents different traits: size, speed, agility, color
- Give agents flexible bodies
- ??

Call this code `p4.cpp`.

You will turn these in with git on GitHub.

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

7. Definitely needing some GPU computing now.