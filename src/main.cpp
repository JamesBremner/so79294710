#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>

#include "packEngine.h"

#include "cStarterGUI.h"

typedef raven::pack::cItem item_t;

item_t genRandom(int min, int max)
{
    double x = rand() % (max - min) + min;
    double y = rand() % (max - min) + min;
    return item_t(x, y);
}

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500})
    {
        srand(77);
        myPack.setSize(6, 7);

        myTimer = new wex::timer(fm, 500);
        fm.events().timer(
            [this](int id)
            {
                fm.update();
            });
        show();
        run();
    }

private:
    wex::timer *myTimer;
    void draw(wex::shapes &S);

    raven::pack::cEngine myPack;
};

void cGUI::draw(wex::shapes &S)
{
    // generate random arrival
    auto arrival = genRandom(1, 3);

    // try to pack arrival
    try
    {
        myPack.pack(arrival);

    }
    catch (std::runtime_error &e)
    {
        if (e.what() == "No space for item")
        {
            // rotate
            double temp = arrival.wlh.x;
            arrival.wlh.x = arrival.wlh.y;
            arrival.wlh.y = temp;

            // try to pack arrival
            try
            {
                myPack.pack(arrival);
            }
            catch (std::runtime_error &e)
            {
                if (e.what() == "No space for item")
                {
                    std::cout << "grid is full\n";
                    return;
                }
                std::cout << e.what();
                exit(1);
            }
        }
    }
    std::cout << myPack.itemCount() << " ";

    double scale = 30;
    int xoff = 1;
    int yoff = 1;
    for (const auto &rect : myPack.getPack())
        S.rectangle(
            cxy(scale * (rect.loc.x + xoff), scale * (rect.loc.y + yoff)),
            cxy(scale * rect.wlh.x, scale * rect.wlh.y));
}

main()
{
    cGUI theGUI;

    return 0;
}
