#include <iostream>

#include "EventDispatcher.h"
#include "LocalEventSubscriptionPolicy.h"
#include "foo.h"

int main()
{
    event_dispatcher_t dispatcher1, dispatcher2;

    Foo f1(dispatcher1), f2(dispatcher2), f3(dispatcher2);


    dispatcher2.subscribeEvent<NewPlayer>(f1,
        [&f1](const NewPlayer& np)
        {
            std::cout << (uint32_t)np.player_id << ":";
            std::cout << (uint32_t)np.team_id << "\n";
        });
    NewPlayer np{42, 5};
    dispatcher2.postEvent<NewPlayer>(&f3, np);

    dispatcher1.unsubscribeEvent<NewPlayer>(f1);
    dispatcher2.unsubscribeEvent<NewPlayer>(f1);
    dispatcher2.unsubscribeEvent<NewPlayer>(f2);
    dispatcher2.unsubscribeEvent<NewPlayer>(f3);
	return 0;
}
