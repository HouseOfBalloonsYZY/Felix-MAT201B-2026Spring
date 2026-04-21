#include <cmath>
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
    ParameterInt populationNumber{"Population number", "", 15, 2, 80};
    // dis r two great suitable turn speed
    Parameter moveSpeed{"Move speed", "", 5.0, 1.0, 10.0};
    Parameter turnSpeed{"Turn speed", "", 0.01, 0.005, 0.3};
    ParameterColor backgroundColor{"Background color"};
    Parameter neighborDistance{"Neighbor distance", "", 10, 5, 20};
    Parameter intimateDistance{"Intimate distance", "", 0.5, 0.1, 1};
    Parameter aloofDistance{"Aloof distance", "", 4, 3, 4};
	Parameter worldSize{"World size", "", 20.0, 10.0, 50.0};
	Parameter renderRadius{"Render radius", "", 100.0, 50.0, 300.0};



    Light light;
    Material material;

    Mesh mesh;

    std::vector<Nav> agent;
    std::vector<int> lovedNeighbour;


    // tiling method started from cursor
    // conversation here (the beginning 2 quests are irrevelant other stuff)
    // Assignment1/Wrapping around method.md
	Vec3d nearestTileShift(Vec3d p, Vec3d cam, double worldSize) const
	{
		double kx = std::round((cam.x - p.x) / worldSize);
		double ky = std::round((cam.y - p.y) / worldSize);
		double kz = std::round((cam.z - p.z) / worldSize);
		return Vec3d(kx, ky, kz) * worldSize;
	}

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
        gui.add(aloofDistance);
		gui.add(worldSize);
		gui.add(renderRadius);
    }
    
    void reset(int n) 
    {
        agent.clear();
        agent.resize(n);
        randomlyFallInLove();
        for (auto& a : agent) 
        {
			a.pos(Vec3d(rs(), rs(), rs()) * (worldSize * 0.01));
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
            agent[i].faceToward(agent[lovedNeighbour[i]].pos(), turnSpeed);
        }

        // // // TODO: if as a never nester how to improve dis code
        for (int i = 0; i < agent.size(); i++) 
        {
            auto& me = agent[i];

            // me versus them
            Vec3d sum(0, 0, 0);
            int count = 0;

            //count neighbours
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
                    count++;

                    // if we r too close we must be apart :(
                    if (distance < intimateDistance)
                    {
                        me.nudgeToward(them.pos(), -0.5 * intimateDistance);
                    }
                    sum += them.pos();
                }
                sum += me.pos();
            }


            if (count > 1)
            {
                Vec3d center = sum / (count + 1);
                float distance = (me.pos() - center).mag();

                // // dis breaks here, wondering why
				// while (distance > aloofDistance)
				// {
				// 	me.nudgeToward(center, intimateDistance);
                //     distance = (me.pos() - center).mag();
				// }


                // dis cause a huge chain blast of all things...
                // probably because instant speed was way too fast
                // does nudge increase speed??? or just position change
                if (distance > aloofDistance)
                {
                   me.nudgeToward(center, 0.1 * aloofDistance);
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

		Vec3d cam = nav().pos();
        // TODO: something wrong with the render Radius need to be fixed
        // dis is AI making mistakes
		int copies = (int)std::ceil(renderRadius / worldSize) + 1;
        for (auto& a : agent) 
        {
			Vec3d base = a.pos() + nearestTileShift(a.pos(), cam, worldSize);
			for (int ix = -copies; ix <= copies; ix++)
			{
				for (int iy = -copies; iy <= copies; iy++)
				{
					for (int iz = -copies; iz <= copies; iz++)
					{
						Vec3d p = base + Vec3d(ix, iy, iz) * worldSize;
						if ((p - cam).mag() > renderRadius)
						{
							continue;
						}
						g.pushMatrix();
						g.translate(p);
						g.rotate(a.quat());
						g.draw(mesh);
						g.popMatrix();
					}
				}
			}
        }
    }
};

int main() { MyApp().start(); }
