# Assignment 2

Make a sphere of charged particles. First force each particle to the surface of a sphere as if it were attached by a spring (Hook’s law) to the origin. Now force each particle away from all other particles as if all the particles have positive charge (Coulomb’s law). Start with `particle.cpp` which already does semi-implicit Euler integration and drag force.

Once your physics code is correct, your values for spring stiffness and charge constant and drag will need to be tuned carefully. This can be tricky. Here are some tips:

- start the value at zero and increase slowly observing behavior
    - is the behavior correct? do you have the physics code right?
- test one kind of force at a time
    - make sure springs work before moving on to charge
- add values to the GUI
    - you can tune them while the simulation is running
- if particles bounce around like crazy, try
    - increasing drag
    - decreasing time step

Once you have this all done, introduce an assymetrical force (love or hunger) to make the particles move around and chase each other.

## Questions for Karl Week 5 Tuesday 4/28/26

1. Something is very wrong with the force calculation. I want a dynamic balanced system where the spring force is dominant, so when `dragFactor = 0` ,particles are bouncing around the sphere in certain range and never stops, and get more and more chaotic when manually adding velocity, while the replusive force is altering the shape of the sphere so it's a morphed chamber like mesh. Reference:

    ![Morphed Sphere](https://www.shutterstock.com/image-vector/3d-sphere-mesh-globe-shapes-600w-2357958967.jpg)

    But somehow the outer part of the particles seems not sensitive to the spring force at all, especially the drag. I guess at this point includingthe law of conservation of energy is needed. 

2. A little more on Lambda expression?

3. So I can say that we're abusing mesh to store points and only draw points but not the whole mesh right?

4. Why is auto not allowed outside of a function?

4. In here
    ```cpp
    void reset(int pointNumber)
        {
            mesh.reset();
            velocity.clear();
            force.clear();
            mass.clear();

            // c++11 "lambda" function
            auto randomColor = []()
            { return HSV(rnd::uniform(), 1.0f, 1.0f); };

            mesh.primitive(Mesh::POINTS);
            // does 1000 work on your system? how many can you make before you get a low
            // frame rate? do you need to use <1000?
            for (int _ = 0; _ < pointNumber; _++)
            {
                mesh.vertex(randomVec3f(5));
                mesh.color(randomColor());

                // float m = rnd::uniform(3.0, 0.5);
                float m = 3 + rnd::normal() / 2;
                if (m < 0.5)
                    m = 0.5;
                mass.push_back(m);

                // using a simplified volume/size relationship
                mesh.texCoord(pow(m, 1.0f / 3), 0); // s, t

                // separate state arrays
                velocity.push_back(randomVec3f(0.1));
                force.push_back(randomVec3f(1));
            }
    ```
    Is there actual difference between `velocity.clear()` + `velocity.push_back(randomVec3f(0.1))` and `velocity.resize(pointNumber)` + `velocity[i] = randomVec3f(0.1)`? I guess not really though since memeory is the same. Dumb question.

6. Confused in starter code `for (int i = 0; i < velocity.size(); i++)` and `for (int i = 0; i < mesh.vestices().size(); i++)` ? Why not just use `velocity.size()` for both? I guess it's the same. Dumb question.