#pragma once
#include <atomina.hpp>
#include "../GameEnums.hpp"

using namespace std::placeholders;
using namespace std::string_literals;

/**
 * Game Implementation of state to implement a user defined extended state
 * that is external to the engine
 */
class LobbyState: public ATMA::BaseState, public ATMA::ObjectEventListener
{
public:
    ATMA::ATMAContext &ctx = ATMA::ATMAContext::getContext();

    // default constructor
    LobbyState(
        std::shared_ptr<ATMA::AppWindow> l_win,
        const unsigned int &l_vertID,
        const unsigned int &l_fragID,
        const unsigned int &l_fontID
    ):
        BaseState()
    {
        m_win = l_win;
        auto vertShader = ctx.loadResource<ATMA::GLShader>(l_vertID);
        vertShader->compile(ATMA::ShaderType::Vertex);
        auto fragShader = ctx.loadResource<ATMA::GLShader>(l_fragID);
        fragShader->compile(ATMA::ShaderType::Fragment);
        m_font = ctx.loadResource<ATMA::GLTexture>(l_fontID);
        m_defaultProg = ATMA::GLProgram::makeProgram();
        m_defaultProg->attachShader(vertShader);
        m_defaultProg->attachShader(fragShader);
        m_defaultProg->link();
        auto unselectedTextID = ctx.registerResource("unselected", 0u, "res/unselected.png");
        m_unselectedTexture = ctx.loadResource<ATMA::GLTexture>(unselectedTextID);
        for(int i = 0; i < 2; i++)
        {
            m_connectedPlayers[i].first = ctx.createObject();
            m_connectedPlayers[i].second = ctx.createObject();
        }
        m_startButtonObjId = ctx.createObject();
        m_startButtonTextId = ctx.createObject();
    }

    virtual ~LobbyState() {}

    /**
     * stub implementation of activate function
     */
    virtual void activate() override
    {

        ctx.netManager.startConnection(ATMA::URL{"127.0.0.1"}, 4734);
        static ATMA::NetworkMessage nm{ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_REQUEST)};
        ctx.netManager.sendMessage(nm);

        for(int i = 0; i < 2; i++)
        {
            ctx.addAttribute(m_connectedPlayers[i].first, ATMA::AttributeType(ATMA::Attribute::Render));
            ctx.addAttribute(m_connectedPlayers[i].second, ATMA::AttributeType(ATMA::Attribute::Text));
            m_connectedPlayersObjs[i].first = ctx.getAttribute<ATMA::AttrRenderable>(
                m_connectedPlayers[i].first, ATMA::AttributeType(ATMA::Attribute::Render)
            );
            m_connectedPlayersObjs[i].second = ctx.getAttribute<ATMA::AttrText>(
                m_connectedPlayers[i].second, ATMA::AttributeType(ATMA::Attribute::Text)
            );
            m_connectedPlayersObjs[i].first->m_self->m_prog = m_defaultProg;
            m_connectedPlayersObjs[i].first->m_self->m_texture = m_unselectedTexture;
            m_connectedPlayersObjs[i].first->m_self->m_stackPos = 0;
            m_connectedPlayersObjs[i].second->m_self->m_prog = m_defaultProg;
            m_connectedPlayersObjs[i].second->m_self->m_texture = m_font;
            m_connectedPlayersObjs[i].second->m_self->m_stackPos = 1;
        }

        m_connectedPlayersObjs[0].first->m_self->m_pos = ATMA::Vec2{290.f, 190.f};
        m_connectedPlayersObjs[0].first->m_self->m_size = ATMA::Vec2{90.f, 45.f};
        m_connectedPlayersObjs[0].second->m_self->m_pos = ATMA::Vec2{215.f, 190.f};
        m_connectedPlayersObjs[0].second->m_self->m_size = ATMA::Vec2{8.f, 14.f};

        m_connectedPlayersObjs[1].first->m_self->m_pos = ATMA::Vec2{290.f, -145.f};
        m_connectedPlayersObjs[1].first->m_self->m_size = ATMA::Vec2{90.f, 45.f};
        m_connectedPlayersObjs[1].second->m_self->m_pos = ATMA::Vec2{215.f, -145.f};
        m_connectedPlayersObjs[1].second->m_self->m_size = ATMA::Vec2{8.f, 14.f};
        for(int i = 0; i < 2; i++)
        {
            m_connectedPlayersObjs[i].second->m_self->m_text = "Waiting...";
        }
        m_active = true;
    }

    /**
     * stub implementation of deactivate function
     */
    virtual void deactivate() override
    {
        m_active = false;
        m_startable = false;
        m_connectCount = 0;
        for(int i = 0; i < 3; i++)
        {
            if(ctx.hasAttribute(m_connectedPlayers[i].first, ATMA::AttributeType(ATMA::Attribute::Render)))
            {
                ctx.removeAttribute(m_connectedPlayers[i].first, ATMA::AttributeType(ATMA::Attribute::Render));
                ctx.removeAttribute(m_connectedPlayers[i].second, ATMA::AttributeType(ATMA::Attribute::Text));
            }
        }
        if(ctx.hasAttribute(m_startButtonObjId, ATMA::AttributeType(ATMA::Attribute::Render)))
        {
            ctx.removeAttribute(m_startButtonObjId, ATMA::AttributeType(ATMA::Attribute::Render));
            ctx.removeAttribute(m_startButtonTextId, ATMA::AttributeType(ATMA::Attribute::Text));
        }
    }

    virtual void notify(const ATMA::ObjectEventContext &l_e) override
    {
        if(l_e.m_objectEventType != ATMA::ObjectEventType(ATMA::ObjectEvent::Network))
            return;
        auto &ctx = ATMA::ATMAContext::getContext();
        switch(l_e.m_properties.getAs<unsigned int>("msgType"))
        {
        case static_cast<unsigned int>(ATMA::NetworkMessageEnum::PORT_RESPONSE):
            {
                while(!m_active)
                {
                }
                for(int i = 0; i < 2; i++)
                {
                    switch(l_e.m_properties.getAs<short>("port" + std::to_string(i)))
                    {
                    case -1:
                        m_connectedPlayersObjs[i].second->m_self->m_text = "OtherPlayer";
                        m_connectCount++;
                        break;
                    case 1:
                        m_connectedPlayersObjs[i].second->m_self->m_text = "You";
                        m_connectCount++;
                        break;
                    }
                }
                break;
            }
        case static_cast<unsigned int>(ATMA::NetworkMessageEnum::PORT_JOIN):
            {
                while(!m_active)
                {
                }
                m_connectedPlayersObjs[l_e.m_properties.getAs<short>("port")].second->m_self->m_text = "OtherPlayer";
                m_connectCount++;
                break;
            }
        case static_cast<unsigned int>(ATMA::NetworkMessageEnum::STATE_CHANGE):
            {
                ctx.switchToState(GameStateType(GameStateEnum::PLAYSTATE));
            }
            break;
        default:
            break;
        }
        if(m_connectCount == 2)
        {
            ctx.addAttribute(m_startButtonObjId, ATMA::AttributeType(ATMA::Attribute::Render));
            ctx.addAttribute(m_startButtonTextId, ATMA::AttributeType(ATMA::Attribute::Text));
            auto startRender = ctx.getAttribute<ATMA::AttrRenderable>(
                m_startButtonObjId, ATMA::AttributeType(ATMA::Attribute::Render)
            );
            startRender->m_self->m_prog = m_defaultProg;
            startRender->m_self->m_texture = m_unselectedTexture;
            startRender->m_self->m_stackPos = 0;
            startRender->m_self->m_pos = ATMA::Vec2{150.f, 100.f};
            startRender->m_self->m_size = ATMA::Vec2{90.f, 45.f};
            auto startText =
                ctx.getAttribute<ATMA::AttrText>(m_startButtonTextId, ATMA::AttributeType(ATMA::Attribute::Text));
            startText->m_self->m_prog = m_defaultProg;
            startText->m_self->m_texture = m_font;
            startText->m_self->m_stackPos = 1;
            startText->m_self->m_pos = ATMA::Vec2{150.f, 100.f};
            startText->m_self->m_size = ATMA::Vec2{8.f, 14.f};
            startText->m_self->m_text = "Start";
            m_startable = true;
        }
    }

    /**
     * gets state id
     * @returns state id
     */
    virtual unsigned int getId() const override
    {
        return GameStateType(GameStateEnum::MAINMENU);
    }

    /**
     * passes window events to be handled by the state
     * @param l_winEvent the event generated by the window
     * @returns whether or not the event was handled
     */
    virtual void handleInput(const ATMA::WindowEvent &l_winEvent)
    {
        if(!m_active)
            return;
        switch(l_winEvent.m_type)
        {
        case ATMA::WindowEventEnum::KeyDowned:
            int keyindex = l_winEvent.getProp<int>("keycode"s);
            ATMA::KeyEnum keycode = ATMA::KeyEnum(keyindex);
            ATMA_ENGINE_INFO("key pressed with code {}", keyindex);
            switch(keycode)
            {
            case ATMA::KeyEnum::RETURN:
                if(m_startable)
                {
                    ATMA::Props p{
                        {{"state",
                          std::pair<unsigned char, std::any>{
                              ATMA::NetworkMessageValueType(ATMA::NetworkMessageValueEnum::UNSIGNEDINT),
                              (unsigned int)(GameStateType(GameStateEnum::PLAYSTATE))
                          }}}
                    };
                    ATMA::NetworkMessage nm{ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::STATE_CHANGE), p};
                    ctx.netManager.sendMessage(nm);
                }
            }
        }
        return;
    }
protected:
    std::shared_ptr<ATMA::GLProgram> m_defaultProg;
    std::shared_ptr<ATMA::AppWindow> m_win;
    std::shared_ptr<ATMA::GLTexture> m_font;
    std::shared_ptr<ATMA::GLTexture> m_unselectedTexture;
    std::pair<unsigned int, unsigned int> m_connectedPlayers[2]{};
    unsigned int m_startButtonObjId;
    unsigned int m_startButtonTextId;
    std::atomic<short> m_connectCount = 0;
    std::atomic<bool> m_startable = false;
    std::pair<std::shared_ptr<ATMA::AttrRenderable>, std::shared_ptr<ATMA::AttrText>> m_connectedPlayersObjs[2]{};
};