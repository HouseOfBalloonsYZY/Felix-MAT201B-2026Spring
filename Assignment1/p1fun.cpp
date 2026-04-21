#include <iostream>

#include "al/app/al_App.hpp"  // al::App
#include "al/graphics/al_Shapes.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

float r() { return rnd::uniform(); }
float rs() { return rnd::uniformS(); }

struct MyApp : public App 
{
    ParameterInt populationNumber{"/N", "", 50, 2, 100};
    ParameterColor backgroundColor{"/color"};

    Light light;
    Material material;

    Mesh mesh;

    std::vector<Nav> agent;
    std::vector<int> lovedNeighbour;



    void randomlyFallInLove() 
    {
        lovedNeighbour.clear();
        lovedNeighbour.resize(agent.size());
        for (auto& i : lovedNeighbour) 
        {
            lovedNeighbour[i] = rand()%(agent.size() - 1);
            while (lovedNeighbour[i] == i) 
            {
                lovedNeighbour[i] = rand()%(agent.size() - 1);
            }
        }
    }

    void onInit() override 
    {
        auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
        auto &gui = GUIdomain->newGUI();
        gui.add(populationNumber);
        gui.add(backgroundColor);
    }
    
    void reset(int n) 
    {
        agent.clear();
        agent.resize(n);
        randomlyFallInLove();
        for (auto& a : agent) 
        {
            a.pos(Vec3d(rs(), rs(), rs()));
            a.quat(Quatd(Vec3d(rs(), rs(), rs())).normalize());
        }
    }

    void onCreate() override 
    {
        addCone(mesh);
        mesh.scale(1, 0.2, 1);
        mesh.scale(0.2);
        mesh.generateNormals();

        nav().pos(0, 0, 15);
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
            agent[i].step(dt);

            // then turn a little towards loved one
            agent[i].faceTowardLine(agent[lovedNeighbour[i]].pos(), agent[i].uu(), dt); //confused by dt here, feels like i'm absuing dt
            
            //dis one is fun
            agent[i].nudgeToward(agent[lovedNeighbour[i]].pos(), 2);
        }
    }

    void onDraw(Graphics& g) override 
    {
        g.clear(backgroundColor);

        light.ambient(RGB(0));          // Ambient reflection for this light
        light.diffuse(RGB(1, 1, 0.5));  // Light scattered directly from light
        g.lighting(true);
        g.light(light);
        material.specular(light.diffuse() * 0.2);  // Specular highlight, "shine"
        material.shininess(50);  // Concentration of specular component [0,128]

        g.material(material);

        for (auto& a : agent) 
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
