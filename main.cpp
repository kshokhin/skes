#include <iostream>

#include "EventDispatcher.h"
#include "foo.h"

int main()
{
    EventDispatcher dispatcher1, dispatcher2;

    Foo f1(dispatcher1), f2(dispatcher2);


    dispatcher2.subscribeEvent<NewPlayer>(f1,
        [&f1](const NewPlayer& np)
        {
            std::cout << (uint32_t)np.player_id << ":";
            std::cout << (uint32_t)np.team_id << "\n";
        });
    NewPlayer np{42, 5};
    dispatcher2.postEvent<NewPlayer>(nullptr, np);
    std::cout << "Hello, world!\n";
	return 0;
}
