// Felix Yuan
// Starter code from Karl :D
// 4/26/26

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f randomVec3f(float scale)
{
    return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

string slurp(string fileName); // forward declaration

struct AlloApp : App
{
    ParameterInt pointNumber{"Point Number", "", 1000, 500, 5000};
    Parameter pointSize{"Point size", "", 5.0, 1.0, 10.0};
    Parameter timeStep{"Time step", "", 0.01, 0.001, 0.02};
    Parameter dragFactor{"Drag factor", "", 1.0, 0.00, 10.0};
    Parameter repulsivity{"Repulsivity", "", 0.01, -1.0, 1.0};
    Parameter springLength{"Spring length", "", 5.0, 0.1, 10.0};
    Parameter springTension{"Spring tension", "", 2.0, 0.1, 10.0};

    ShaderProgram pointShader;

    //  simulation state
    Mesh mesh; // position *is inside the mesh* mesh.vertices() are the positions
    vector<Vec3f> velocity;
    vector<Vec3f> force;
    vector<float> mass;

    void onInit() override
    {
        // set up GUI
        auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
        auto &gui = GUIdomain->newGUI();
        gui.add(pointNumber);
        gui.add(pointSize);
        gui.add(timeStep);
        gui.add(dragFactor);
        gui.add(repulsivity);
        gui.add(springLength);
        gui.add(springTension);
    }

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
        for (int _ = 0; _ < pointNumber; _++)
        {
            mesh.vertex(randomVec3f(5));
            mesh.color(randomColor());

            // float m = rnd::uniform(3.0, 0.5);
            // float m = 3 + rnd::normal() / 2;
            // if (m < 0.5)
            //     m = 0.5;
            float m = 1 + rnd::normal() / 2;
            if (m < 0.5)
                m = 0.5;
            mass.push_back(m);

            // using a simplified volume/size relationship
            mesh.texCoord(pow(m, 1.0f / 3), 0); // s, t

            // separate state arrays
            velocity.push_back(randomVec3f(0.1));
            force.push_back(randomVec3f(1));
        }

        // for (int i = 0; i < mesh.vertices().size(); i++) {
        //   mesh.vertices()[i].normalize();
        // }
    }

    void onCreate() override
    {
        // compile shaders
        // XXX Should I recompile shaders when restart?
        pointShader.compile(slurp("../point-vertex.glsl"),
                            slurp("../point-fragment.glsl"),
                            slurp("../point-geometry.glsl"));

        // set initial conditions of the simulation

        reset(pointNumber);

        nav().pos(0, 0, 50);
    }

    bool freeze = false;
    int lastN = 0;
    void onAnimate(double dt) override
    {
        if (pointNumber != lastN)
        {
            lastN = pointNumber;
            reset(pointNumber);
        }

        if (freeze)
            return;

        // Vec3f has lots of operations you might use...
        // • +=
        // • -=
        // • +
        // • -
        // • .normalize() ~ Vec3f points in the direction as it did, but has length 1
        // • .normalize(float scale) ~ same but length `scale`
        // • .mag() ~ length of the Vec3f
        // • .magSqr() ~ squared length of the Vec3f
        // • .dot(Vec3f f)
        // • .cross(Vec3f f)

        vector<Vec3f> &position(mesh.vertices());
        for (int i = 0; i < mesh.vertices().size(); i++)
        {
            auto &me = mesh.vertices()[i];

            // attach this particle to the origin with a sping of some stiffness (k) with a resting length of 1
            // calculate spring force between this particle and the origin
            // make stiffness a parameter
            // force_amount == -(resting_length - actual_length) * stiffness
            //
            // what is the unit vector that points in the direction of force?
            //

            // ***
            // Spring force

            // me.normalize points outward
            // if springLength > me.mag(), force is outward
            Vec3f springForce_direction = me / me.mag(); // XXX Don't normalize
            float springForce_magnitude = (springLength - me.mag()) * springTension;
            Vec3f springForce = springForce_direction * springForce_magnitude;

            // limiter
            //  if (springForce.mag() > 10)
            //  {
            //      springForce = springForce.normalize() * 10;
            //  }

            // apply spring force
            force[i] += springForce;

            // Spring force
            // ***

            // ***
            // Repulsive force

            for (int j = i + 1; j < velocity.size(); j++)
            {
                // i - j points towards i
                // repulsive force works on i and points outward to i
                Vec3f repulsiveDistance = (mesh.vertices()[i] - mesh.vertices()[j]);
                Vec3f repulsiveDirection = repulsiveDistance.normalize();
                float repulsiveMagnitude = repulsivity * mass[i] * mass[j] / (repulsiveDistance.mag() * repulsiveDistance.mag());
                Vec3f repulsiveForce = repulsiveDirection * repulsiveMagnitude;

                // limiter
                if (repulsiveForce.mag() > 10)
                {
                    repulsiveForce = repulsiveForce.normalize() * 10;
                }

                // apply repulsive force
                // i and j are a pair
                force[i] += repulsiveForce;
                force[j] -= repulsiveForce;
            }

            // Repulsive Force
            // ***

            // Viscous drag
            force[i] += -velocity[i] * dragFactor; // F = -bv

            // "semi-implicit" Euler integration
            velocity[i] += force[i] / mass[i] * timeStep;
            position[i] += velocity[i] * timeStep;
        }

        // clear all accelerations (IMPORTANT!!)
        //
        for (auto &a : force)
            a.set(0);
    }

    bool onKeyDown(const Keyboard &k) override
    {
        if (k.key() == ' ')
        {
            freeze = !freeze;
        }

        if (k.key() == '1')
        {
            // introduce some "random" forces
            for (int i = 0; i < velocity.size(); i++)
            {
                // F = ma
                force[i] += randomVec3f(10);
            }
        }

        if (k.key() == '2')
        {
            for (int i = 0; i < velocity.size(); i++)
            {
                velocity[i] = randomVec3f(1);
            }
        }

        return true;
    }

    void onDraw(Graphics &g) override
    {
        g.clear(0.3);
        g.shader(pointShader);
        // TODO how to send a list of pointSize contigent of mass
        g.shader().uniform("pointSize", pointSize / 100);
        g.blending(true);
        g.blendTrans();
        g.depthTesting(true);
        g.draw(mesh);
    }
};

int main()
{
    AlloApp app;
    app.configureAudio(48000, 512, 2, 0);
    app.start();
}

string slurp(string fileName)
{
    fstream file(fileName);
    string returnValue = "";
    while (file.good())
    {
        string line;
        getline(file, line);
        returnValue += line + "\n";
    }
    return returnValue;
}
