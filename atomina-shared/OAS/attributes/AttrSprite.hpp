#pragma once
#include "core/api.hpp"
#include "OAS/Attribute.hpp"
#include "OAS/AttrBase.hpp"
#include "render/GLSprite.hpp"

namespace ATMA
{

    /**
     * An Attribute that holds the members of an object required to render text to a display
     */
    class AttrSprite: public AttrBase
    {
    public:
        // default constructor
        AttrSprite();

        // deconstructor
        virtual ~AttrSprite();

        // friend function to output attributes
        friend std::stringstream &operator>>(std::stringstream &l_stream, AttrBase &b);

        /**
         * fill attribute members with data contained within a string stream
         * @param l_stream string stream containing data of the attribute members
         */
        void readIn(std::stringstream &l_stream);

        std::shared_ptr<GLSprite> m_self = GLSprite::makeSprite();
    };

}