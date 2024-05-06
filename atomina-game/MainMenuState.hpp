#pragma once
#include <atomina.hpp>
#include "GameStateType.hpp"

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
    MainMenuState(std::shared_ptr<ATMA::AppWindow> win): BaseState()
    {
        m_win = win;
        auto selectedTextID = ctx.registerResource("selected", 0u, "res/selected.png");
        auto unselectedTextID = ctx.registerResource("unselected", 0u, "res/unselected.png");
        auto fontID = ctx.registerResource("font", 1u, "res/defaultFont.png");
        m_font = ctx.loadResource<ATMA::Font>(fontID);
        m_selectedTexture = ctx.loadResource<ATMA::Texture>(selectedTextID);
        m_unselectedTexture = ctx.loadResource<ATMA::Texture>(unselectedTextID);
        for (int i = 0; i < 3; i++)
        {
            m_menuOpts[i].first = ctx.createObject();
            m_menuOpts[i].second = ctx.createObject();
        }


    }

    virtual ~MainMenuState() {

    }

    /**
     * stub implementation of activate function
     */
    virtual void activate() override 
    {
        m_active = true;
        m_selected = 2;
        ctx.m_renderCtx->setFont(m_font);
        for (int i = 0; i < 3; i++)
        {
            ctx.addAttribute(m_menuOpts[i].first, ATMA::AttributeType(ATMA::Attribute::Render));
            ctx.addAttribute(m_menuOpts[i].second, ATMA::AttributeType(ATMA::Attribute::Render));
            m_menuObjs[i].first = ctx.getAttribute<ATMA::AttrRenderable>(
                m_menuOpts[i].first, ATMA::AttributeType(ATMA::Attribute::Render)
            );
            m_menuObjs[i].second = ctx.getAttribute<ATMA::AttrRenderable>(
                m_menuOpts[i].second, ATMA::AttributeType(ATMA::Attribute::Render)
            );
            m_menuObjs[i].second->m_stackPos = 1;
            m_menuObjs[i].first->m_element = m_unselectedTexture->m_self;
            m_menuObjs[i].first->m_srcRegion = ATMA::Vec2<float>{1.f, 1.f};
            m_menuObjs[i].first->m_region = ATMA::Vec2<float>{0.3f, 0.1f};
            m_menuObjs[i].first->m_stackPos = 0;
        }
        m_menuObjs[0].second->m_element = "Exit";
        m_menuObjs[0].second->m_region = ATMA::Vec2<float>{0.07f, 0.1f};
        m_menuObjs[0].second->m_screenPos = ATMA::Vec2<float>{-0.21f, -0.25f};
        m_menuObjs[0].first->m_screenPos = ATMA::Vec2<float>{0.f, -0.25f};

        m_menuObjs[1].second->m_element = "Options";
        m_menuObjs[1].second->m_region = ATMA::Vec2<float>{0.04f, 0.1f};
        m_menuObjs[1].second->m_screenPos = ATMA::Vec2<float>{-0.25f, 0.f};
        m_menuObjs[1].first->m_screenPos = ATMA::Vec2<float>{0.f, 0.f};

        
        m_menuObjs[2].second->m_element = "Play";
        m_menuObjs[2].second->m_region = ATMA::Vec2<float>{0.07f, 0.1f};
        m_menuObjs[2].second->m_screenPos = ATMA::Vec2<float>{-0.21f, 0.25f};
        m_menuObjs[2].first->m_screenPos = ATMA::Vec2<float>{0.f, 0.25f};
        m_menuObjs[2].first->m_element = m_selectedTexture->m_self;
        ctx.onUpdate(
            [&](const long long &l_dt){}
        );
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
            ctx.removeAttribute(m_menuOpts[i].second, ATMA::AttributeType(ATMA::Attribute::Render));
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

                ATMA::KeyEnum keycode = ATMA::KeyEnum(l_winEvent.getProp<unsigned int>("keycode"s));
                switch(keycode)
                {
                case ATMA::KeyEnum::DOWN:
                    m_menuObjs[m_selected].first->m_element = m_unselectedTexture->m_self;
                    if(m_selected == 0)
                    {
                        m_selected = 2;
                    }
                    else
                    {
                        m_selected--;
                    }
                    m_menuObjs[m_selected].first->m_element = m_selectedTexture->m_self;
                    break;
                case ATMA::KeyEnum::UP:
                    m_menuObjs[m_selected].first->m_element = m_unselectedTexture->m_self;
                    if(m_selected == 2)
                    {
                        m_selected = 0;
                    }
                    else
                    {
                        m_selected++;
                    }
                    m_menuObjs[m_selected].first->m_element = m_selectedTexture->m_self;
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
                        ctx.switchToState(GameStateType(GameStateEnum::PLAYSTATE));
                        break;
                    }
                    break;
                }

                break;
            }
        case ATMA::WindowEventEnum::Resized:
            ATMA::ATMAContext::getContext().m_renderCtx->setSize(
                {l_winEvent.getProp<unsigned int>("width"s),
                 l_winEvent.getProp<unsigned int>("height"s)}
            );
            break;
        case ATMA::WindowEventEnum::Closed:
            l_winEvent.m_win->notifyClose();
            break;
        }
        return;
    }
private:
    std::shared_ptr<ATMA::AppWindow> m_win;
    std::pair<unsigned int,unsigned int> m_menuOpts[3] = {{0,0}, {0,0}, {0,0}};
    std::pair<std::shared_ptr<ATMA::AttrRenderable>,std::shared_ptr<ATMA::AttrRenderable>> m_menuObjs[3]{};
    int m_selected = 0;
    std::shared_ptr<ATMA::Texture> m_selectedTexture;
    std::shared_ptr<ATMA::Texture> m_unselectedTexture;
    std::shared_ptr<ATMA::Font> m_font;
};