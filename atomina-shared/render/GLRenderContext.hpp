#pragma once
#include "pch.hpp"
#include "core/api.hpp"
#include "GUI/AppWindow.hpp"
#include "render/GLRenderable.hpp"
#include "render/GLBuffer.hpp"

namespace ATMA
{
    
    /**
     * Abstract class of Graphics Rendering Context, used to draw drawable objects.
     * Must be pointed to an app window first.
    */
    class ATMA_API GLRenderContext
    {
    public:
        //default deconstructor
        virtual ~GLRenderContext();

        /**
         * Points the render context to the corresponding app window to draw to it 
         * from now on
         * @param l_window the App window to target
        */
        virtual void setWindow(std::shared_ptr<AppWindow> l_window) = 0;

        /**
         * Starts a draw call for the renderable object
         * @param l_renderable the object to be drawn
        */
        virtual void draw(std::shared_ptr<GLRenderable> l_renderable) = 0;

        /**
         * clears the view port that he Render context has been set to
         * to be all one colour
        */
        virtual void clear() = 0;

        // virtual void drawText(const std::string &l_text) = 0;

        /**
         * Factory function for creating a render context for the 
         * platform
         * @returns unique pointer of the render context
        */
        static std::unique_ptr<GLRenderContext> makeRenderContext();
    protected:
        //protected constructor so it cannot be publicly instantiated
        GLRenderContext();
    };

}