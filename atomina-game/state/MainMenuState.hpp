#pragma once
#include <atomina.hpp>
#include "../GameEnums.hpp"

using namespace std::placeholders;
using namespace std::string_literals;

/**
 * Game Implementation of state to implement a user defined extended state
 * that is external to the engine
 */
class MainMenuState: public ATMA::BaseState
{
public:
    ATMA::ATMAContext &ctx = ATMA::ATMAContext::getContext();

    // default constructor
    MainMenuState(
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
        m_defaultProg = ATMA::GLProgram::makeProgram();
        m_defaultProg->attachShader(vertShader);
        m_defaultProg->attachShader(fragShader);
        m_defaultProg->link();
        auto selectedTextID = ctx.registerResource("selected", 0u, "res/selected.png");
        auto unselectedTextID = ctx.registerResource("unselected", 0u, "res/unselected.png");
        m_font = ctx.loadResource<ATMA::GLTexture>(l_fontID);
        m_selectedTexture = ctx.loadResource<ATMA::GLTexture>(selectedTextID);
        m_unselectedTexture = ctx.loadResource<ATMA::GLTexture>(unselectedTextID);
        for(int i = 0; i < 3; i++)
        {
            m_menuOpts[i].first = ctx.createObject();
            m_menuOpts[i].second = ctx.createObject();
        }
    }

    virtual ~MainMenuState() {}

    /**
     * stub implementation of activate function
     */
    virtual void activate() override
    {
        m_active = true;
        m_selected = 2;
        // ctx.m_renderCtx->setFont(m_font);
        for(int i = 0; i < 3; i++)
        {
            ctx.addAttribute(m_menuOpts[i].first, ATMA::AttributeType(ATMA::Attribute::Render));
            ctx.addAttribute(m_menuOpts[i].second, ATMA::AttributeType(ATMA::Attribute::Text));
            m_menuObjs[i].first = ctx.getAttribute<ATMA::AttrRenderable>(
                m_menuOpts[i].first, ATMA::AttributeType(ATMA::Attribute::Render)
            );
            m_menuObjs[i].second =
                ctx.getAttribute<ATMA::AttrText>(m_menuOpts[i].second, ATMA::AttributeType(ATMA::Attribute::Text));
            m_menuObjs[i].first->m_self->m_prog = m_defaultProg;
            m_menuObjs[i].first->m_self->m_texture = m_unselectedTexture;
            m_menuObjs[i].first->m_self->m_pos = ATMA::Vec2<float>{1.f, 1.f};
            m_menuObjs[i].first->m_self->m_size = ATMA::Vec2<float>{30.f, 10.f};
            m_menuObjs[i].first->m_self->m_stackPos = 0;
        }
        m_menuObjs[0].second->m_self->m_text = "Exit";
        m_menuObjs[0].second->m_self->m_texture = m_font;
        m_menuObjs[0].second->m_self->m_prog = m_defaultProg;
        m_menuObjs[0].second->m_self->m_stackPos = 1;
        m_menuObjs[0].second->m_self->m_size = ATMA::Vec2<float>{7.f, 10.f};
        m_menuObjs[0].second->m_self->m_pos = ATMA::Vec2<float>{-21.f, -25.f};
        m_menuObjs[0].first->m_self->m_pos = ATMA::Vec2<float>{0.f, -25.f};

        m_menuObjs[1].second->m_self->m_text = "Options";
        m_menuObjs[1].second->m_self->m_texture = m_font;
        m_menuObjs[1].second->m_self->m_prog = m_defaultProg;
        m_menuObjs[1].second->m_self->m_stackPos = 1;
        m_menuObjs[1].second->m_self->m_size = ATMA::Vec2<float>{4.f, 10.f};
        m_menuObjs[1].second->m_self->m_pos = ATMA::Vec2<float>{-25.f, 0.f};
        m_menuObjs[1].first->m_self->m_pos = ATMA::Vec2<float>{0.f, 0.f};

        m_menuObjs[2].second->m_self->m_text = "Connect";
        m_menuObjs[2].second->m_self->m_texture = m_font;
        m_menuObjs[2].second->m_self->m_prog = m_defaultProg;
        m_menuObjs[2].second->m_self->m_stackPos = 1;
        m_menuObjs[2].second->m_self->m_size = ATMA::Vec2<float>{7.f, 10.f};
        m_menuObjs[2].second->m_self->m_pos = ATMA::Vec2<float>{-21.f, 25.f};
        m_menuObjs[2].first->m_self->m_pos = ATMA::Vec2<float>{0.f, 25.f};
        m_menuObjs[2].first->m_self->m_texture = m_selectedTexture;
        ctx.onUpdate([&](const long long &l_dt) {});
    }

    /**
     * stub implementation of deactive function
     */
    virtual void deactivate() override
    {
        m_active = false;
        for(int i = 0; i < 3; i++)
        {
            ctx.removeAttribute(m_menuOpts[i].first, ATMA::AttributeType(ATMA::Attribute::Render));
            ctx.removeAttribute(m_menuOpts[i].second, ATMA::AttributeType(ATMA::Attribute::Text));
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
            {
                int keyindex = l_winEvent.getProp<int>("keycode"s);
                ATMA::KeyEnum keycode = ATMA::KeyEnum(keyindex);
                ATMA_ENGINE_INFO("key pressed with code {}", keyindex);
                switch(keycode)
                {
                case ATMA::KeyEnum::DOWN:
                    m_menuObjs[m_selected].first->m_self->m_texture = m_unselectedTexture;
                    if(m_selected == 0)
                    {
                        m_selected = 2;
                    }
                    else
                    {
                        m_selected--;
                    }
                    m_menuObjs[m_selected].first->m_self->m_texture = m_selectedTexture;
                    break;
                case ATMA::KeyEnum::UP:
                    m_menuObjs[m_selected].first->m_self->m_texture = m_unselectedTexture;
                    if(m_selected == 2)
                    {
                        m_selected = 0;
                    }
                    else
                    {
                        m_selected++;
                    }
                    m_menuObjs[m_selected].first->m_self->m_texture = m_selectedTexture;
                    break;
                case ATMA::KeyEnum::RETURN:
                    switch(m_selected)
                    {
                    case 0:
                        m_win->notifyClose();
                        break;
                    case 1:
                        break;
                    case 2:
                        ctx.switchToState(GameStateType(GameStateEnum::LOBBY));
                        break;
                    }
                    break;
                }

                break;
            }
        case ATMA::WindowEventEnum::Resized:

            break;
        case ATMA::WindowEventEnum::Closed:
            l_winEvent.m_win->notifyClose();
            break;
        }
        return;
    }
private:
    std::shared_ptr<ATMA::GLProgram> m_defaultProg;
    std::shared_ptr<ATMA::AppWindow> m_win;
    std::pair<unsigned int, unsigned int> m_menuOpts[3] = {
        {0, 0},
        {0, 0},
        {0, 0}
    };
    std::pair<std::shared_ptr<ATMA::AttrRenderable>, std::shared_ptr<ATMA::AttrText>> m_menuObjs[3]{};
    int m_selected = 0;
    std::shared_ptr<ATMA::GLTexture> m_selectedTexture;
    std::shared_ptr<ATMA::GLTexture> m_unselectedTexture;
    std::shared_ptr<ATMA::GLTexture> m_font;
};