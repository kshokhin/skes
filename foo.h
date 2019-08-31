#ifndef FOO_H
#define FOO_H

#include "eventdispatcher.h"
#include "types.h"


struct NewPlayer
{
    player_id_t player_id = 0;
    uint8_t team_id = 0;
};

class Foo : public EventProcessor
{
public:
    explicit Foo(EventDispatcher& dispatcher)
    {
        dispatcher.subscribeEvent<NewPlayer>(*this,
            [this](const NewPlayer& np_info)
            {
                onNewPlayer(np_info);
                //std::cout << "ololo\n";
            });
    }

    virtual ~Foo() = default;

    void onNewPlayer(const NewPlayer& np_info)
    {
        std::cout << "new player " << (uint32_t)np_info.player_id;
        std::cout << " from team " << (uint32_t)np_info.team_id << " added\n";
    }
private:
};

#endif
