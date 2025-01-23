#include <atomina.hpp>
#include "GameEnums.hpp"
#include "state/MainMenuState.hpp"
#include "state/LobbyState.hpp"
#include "state/PlayState.hpp"
#include "OAS/AttrConnection.hpp"
#include "OAS/SysConnection.hpp"

/*
 * extension of the Game class from atomina-api
 * used as actual implementation
 */
class GameApp: public ATMA::Game
{
public:
    std::shared_ptr<ATMA::AppWindow> m_win;
    std::shared_ptr<ATMA::GLRenderer> m_renderer;

    // default constructor
    GameApp();

    // deconstructor
    ~GameApp();

    /**
     * real implementation of the game apps setup function to house the setup
     */
    virtual void setup(ATMA::ATMAContext &l_ctx) override;

    /**
     * real implementation of the game apps run function to house the
     * game loop
     */
    virtual void update(ATMA::ATMAContext &l_ctx) override;

    /**
     * real implementation of the game apps shut down function
     * to initiate shut down of the app
     */
    virtual void shutdown(ATMA::ATMAContext &l_ctx) override;
};
