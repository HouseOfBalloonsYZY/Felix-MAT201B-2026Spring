#include <cmath>
#include <iostream>

#include "al/app/al_App.hpp" // al::App
#include "al/graphics/al_Shapes.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

float r() { return rnd::uniform(); }
float rs() { return rnd::uniformS(); }

struct MyApp : public App
{
    ParameterInt populationNumber{"Population number", "", 15, 2, 80};
    Parameter moveSpeed{"Move speed", "", 5.0, 1.0, 10.0};   // there are two great suitable turn speed
    Parameter turnSpeed{"Turn speed", "", 0.01, 0.005, 0.3}; // there are two great suitable turn speed
    ParameterColor backgroundColor{"Background color"};
    Parameter neighborDistance{"Neighbor distance", "", 10, 5, 20};
    Parameter intimateDistance{"Intimate distance", "", 0.5, 0.1, 1};
    Parameter aloofDistance{"Aloof distance", "", 4.0, 3.0, 5.0};
    // Parameter worldSize{"World size", "", 30.0, 10.0, 50.0};
    // Parameter renderRadius{"Render radius", "", 100.0, 50.0, 300.0};
    Parameter nudgeDistance{"Nudge distance", "", 0.05, 0.01, 0.1};

    Light light;
    Material material;

    Mesh mesh;

    std::vector<Nav> agent;
    std::vector<int> lovedByNeighbour;

    // XXX dis is nested function, not good
    void randomlyFallInLove()
    {
        lovedByNeighbour.clear();
        lovedByNeighbour.resize(agent.size());
        for (int i = 0; i < (int)lovedByNeighbour.size(); i++)
        {
            lovedByNeighbour[i] = rand() % (agent.size() - 1);
            while (lovedByNeighbour[i] == i)
            {
                lovedByNeighbour[i] = rand() % (agent.size() - 1);
            }
        }
    }

    void onInit() override
    {
        auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
        auto &gui = GUIdomain->newGUI();
        gui.add(populationNumber);
        gui.add(moveSpeed);
        gui.add(turnSpeed);
        gui.add(backgroundColor);
        gui.add(neighborDistance);
        gui.add(intimateDistance);
        gui.add(aloofDistance);
        // gui.add(worldSize);
        // gui.add(renderRadius);
        gui.add(nudgeDistance);
    }

    void reset(int n)
    {
        agent.clear();
        agent.resize(n);
        randomlyFallInLove();
        for (auto &a : agent)
        {
            a.quat(Quatd(Vec3d(rs(), rs(), rs())).normalize());
        }
    }

    void onCreate() override
    {
        addCone(mesh);
        mesh.scale(1, 0.2, 1);
        mesh.scale(0.2);
        mesh.generateNormals();

        nav().pos(0, 0, 50);
        light.pos(-2, 7, 0);
    }

    int lastN = 0;
    void onAnimate(double dt) override
    {
        if (populationNumber != lastN)
        {
            lastN = populationNumber;
            reset(populationNumber);
        }

        for (int i = 0; i < agent.size(); i++)
        {
            // first drift a bit
            agent[i].moveF(moveSpeed);

            // then turn a little towards loved one
            
            agent[lovedByNeighbour[i]].faceToward(agent[i].pos(), turnSpeed);
        }

        // XXX if as a never nester how to improve dis code
        for (int i = 0; i < agent.size(); i++)
        {
            auto &me = agent[i];

            // me versus them
            Vec3d sum(0, 0, 0);
            int count = 0;

            // count neighbours
            for (int j = 0; j < agent.size(); j++)
            {
                if (i == j)
                {
                    continue;
                }

                auto &them = agent[j];

                float distance = (me.pos() - them.pos()).mag();
                if (distance < neighborDistance)
                {
                    count++;

                    if (distance < intimateDistance)
                        me.nudgeToward(me.pos() - them.pos(), nudgeDistance);

                    // XXX something is very wrong with the while loop
                    // // if we r too close we must be apart :(
                    // while (distance < intimateDistance)
                    // {
                    //     me.nudgeToward(me.pos() - them.pos(), nudgeDistance);
                    //     distance += nudgeDistance;
                    // }
                    sum += them.pos();
                }
                sum += me.pos();
            }

            if (count > 1)
            {
                Vec3d center = sum / (count + 1);
                float distance = (me.pos() - center).mag();

                if (distance > aloofDistance)
                    me.nudgeToward(center, nudgeDistance);

                // XXX something is very wrong with the while loop
                // // // dis breaks here, wondering why
                // while (distance > aloofDistance)
                // {
                //     me.nudgeToward(center, nudgeDistance);
                //     distance -= nudgeDistance;
                //     // distance = (me.pos() - center).mag();
                // }

                // dis cause a huge chain blast of all things...
                // probably because instant speed was way too fast
                // does nudge increase speed??? or just position change
                // if (distance > aloofDistance)
                // {
                //    me.nudgeToward(center, 0.1 * aloofDistance);
                // }
            }
        }

        for (auto &a : agent)
        {
            a.step(dt);
        }
    }

    void onDraw(Graphics &g) override
    {
        g.clear(backgroundColor);

        light.ambient(RGB(0));         // Ambient reflection for this light
        light.diffuse(RGB(1, 1, 0.5)); // Light scattered directly from light
        g.lighting(true);
        g.light(light);
        material.specular(light.diffuse() * 0.2); // Specular highlight, "shine"
        material.shininess(50);                   // Concentration of specular component [0,128]

        g.material(material);

        for (auto &a : agent)
        {
            g.pushMatrix();
            g.translate(a.pos());
            g.rotate(a.quat());
            g.draw(mesh);
            g.popMatrix();
        }
    }
};

int main() { MyApp().start(); }
