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

    raven::pack::cEngine myPack;

    bool fbusy;

    void pack();
    void repack();
    void draw(wex::shapes &S);
};

cGUI::cGUI()
    : cStarterGUI(
          "Packer",
          {50, 50, 1000, 500}),
      fbusy(false)
{
    srand(77);
    myPack.setSize(6, 7);
    myPack.setSpin();

    fm.events().click(
        [this]()
        {
            // start packing arrivals
            myTimer = new wex::timer(fm, 1000);
        });

    fm.events().timer(
        [this](int id)
        {
            if (fbusy)
                return;
            fbusy = true;

            pack();

            // update display
            fm.update();

            fbusy = false;
        });

    show();
    run();
}

void cGUI::draw(wex::shapes &S)
{
    double scale = 70;
    int xoff = 1;
    int yoff = 1;
    for (const auto &rect : myPack.getPack())
        S.rectangle(
            cxy(scale * (rect.loc.x + xoff), scale * (rect.loc.y + yoff)),
            cxy(scale * rect.wlh.x, scale * rect.wlh.y));
    std::cout << myPack.itemCount() << " ";
}

void cGUI::pack()
{
    // generate random arrival
    item_t arrival(
        rand() % (3 - 1) + 1,
        rand() % (3 - 1) + 1 );

    try
    {
        myPack.pack(arrival);
    }
    catch (std::runtime_error &e)
    {
        if (e.what() != std::string("No space for item"))
        {
            std::cout << e.what();
            exit(1);
        }
        wex::msgbox("Need to repack");
        myPack.addItem(arrival);
        repack();
    }
}

void cGUI::repack()
{
    try
    {
        auto v_all_items = myPack.getPack();
        std::sort(
            v_all_items.begin(), v_all_items.end(),
            [](const item_t &a, const item_t &b)
            {
                return a.volume() > b.volume();
            });
        myPack.setSize(6, 7);
        myPack.setSpin();
        for (auto &item : v_all_items)
            myPack.pack(item);
        std::cout << "repack succeeded\n";
    }
    catch (std::runtime_error &e)
    {
        std::cout << "repack failed\n";
        delete myTimer;
    }
}

main()
{
    cGUI theGUI;

    return 0;
}
