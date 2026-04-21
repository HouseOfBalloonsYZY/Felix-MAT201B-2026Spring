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
    ParameterInt populationNumber{"Population number", "", 50, 2, 100};
    // dis r two great suitable turn speed
    Parameter moveSpeed{"Move speed", "", 2.0, 1.0, 5.0};
    Parameter turnSpeed{"Turn speed", "", 0.01, 0.005, 0.05};
    ParameterColor backgroundColor{"Background color"};
    Parameter neighborDistance{"Neighbor distance", "", 10, 5, 20};
    Parameter intimateDistance{"Intimate distance", "", 0.5, 0.1, 1};



    Light light;
    Material material;

    Mesh mesh;

    std::vector<Nav> agent;
    std::vector<int> lovedNeighbour;

    void randomlyFallInLove() 
    {
        lovedNeighbour.clear();
        lovedNeighbour.resize(agent.size());
		for (int i = 0; i < (int)lovedNeighbour.size(); i++)
		{
			lovedNeighbour[i] = rand() % (agent.size() - 1);
			while (lovedNeighbour[i] == i)
			{
				lovedNeighbour[i] = rand() % (agent.size() - 1);
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
    }
    
    void reset(int n) 
    {
        agent.clear();
        agent.resize(n);
        randomlyFallInLove();
        for (auto& a : agent) 
        {
            a.pos(Vec3d(rs(), rs(), rs())*5);
            a.quat(Quatd(Vec3d(rs(), rs(), rs())).normalize());
        }
    }

    void onCreate() override 
    {
        addCone(mesh);
        mesh.scale(1, 0.2, 1);
        mesh.scale(0.2);
        mesh.generateNormals();

        // camera position is important aestically
        nav().pos(0, 0, 30);
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

        // running towards ur love
        for (int i = 0; i < agent.size(); i++)
        {
            // first drift a bit
            agent[i].moveF(moveSpeed);

            // then turn a little towards loved one
            agent[i].faceToward(agent[lovedNeighbour[i]].pos(), turnSpeed);
        }

        // TODO: if as a never-nester how to improve dis code
        for (int i = 0; i < agent.size(); i++) 
        {
            auto& me = agent[i];

            for (int j = 0; j < agent.size(); j++) 
            {
                if (i == j) 
                {
                    continue;
                }

                auto& them = agent[j];
                
                float distance = (me.pos() - them.pos()).mag();
                if (distance < neighborDistance) 
                {
                    // if we r too close we must be apart :(
                    if (distance < intimateDistance)
                    {
                        me.nudgeToward(them.pos(), -0.5*intimateDistance);
                    }
                }
            }
        }

        for (auto& a : agent) 
        {
            a.step(dt);
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
