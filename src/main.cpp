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

class cGUI : public cStarterGUI
{
public:
    cGUI();

private:
    wex::timer *myTimer;
    void draw(wex::shapes &S);

    raven::pack::cEngine myPack;

    item_t genRandom(int min, int max);
    void pack(item_t arrival);
};

cGUI::cGUI()
    : cStarterGUI(
          "Packer",
          {50, 50, 1000, 500})
{
    srand(77);
    myPack.setSize(6, 7);

    fm.events().click(
        [this]()
        {
            // start packing arrivals
            myTimer = new wex::timer(fm, 500);
        });

    fm.events().timer(
        [this](int id)
        {
            // generate random arrival
            auto arrival = genRandom(1, 3);

            // try to pack arrival
            pack(arrival);

            // update display
            fm.update();
        });

    show();
    run();
}

void cGUI::draw(wex::shapes &S)
{
    double scale = 50;
    int xoff = 1;
    int yoff = 1;
    for (const auto &rect : myPack.getPack())
        S.rectangle(
            cxy(scale * (rect.loc.x + xoff), scale * (rect.loc.y + yoff)),
            cxy(scale * rect.wlh.x, scale * rect.wlh.y));
}

item_t cGUI::genRandom(int min, int max)
{
    double x = rand() % (max - min) + min;
    double y = rand() % (max - min) + min;
    return item_t(x, y);
}
void cGUI::pack(item_t arrival)
{
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
}

main()
{
    cGUI theGUI;

    return 0;
}
