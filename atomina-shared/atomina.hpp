
#pragma once
#include "pch.hpp"
// Client facing header file that has all the includes needed,
// so that only this file needs to be included

#include "core/api.hpp"

// Util
#include "util/AtominaException.hpp"
#include "util/Log.hpp"
#include "util/ATConst.hpp"
#include "util/Worker.hpp"
#include "util/Props.hpp"
#include "util/RetryExecutor.hpp"
#include "util/Path.hpp"


// Math
#include "math/Vec2.hpp"
#include "math/Vec3.hpp"
#include "math/Mat3.hpp"
#include "math/Stadium.hpp"
#include "math/RandomGenerator.hpp"
#include "math/MathFuncs.hpp"

// Core
#include "core/Game.hpp"
#include "core/Server.hpp"
#include "core/Entry.hpp"
#include "core/ATMAContext.hpp"
#include "core/ATMAFormats.hpp"

// File
#include "file/FileHandler.hpp"
#include "file/FileFuncs.hpp"

// Events
#include "event/ObjectEventContext.hpp"
#include "event/ObjectEvent.hpp"

// States

#include "state/State.hpp"
#include "state/BaseState.hpp"
#include "state/states/DummyState.hpp"

// OAS
#include "OAS/Attribute.hpp"
#include "OAS/System.hpp"
#include "OAS/AttrBase.hpp"
#include "OAS/attributes/AttrShape.hpp"
#include "OAS/attributes/AttrControllable.hpp"
#include "OAS/attributes/AttrRenderable.hpp"
#include "OAS/attributes/AttrText.hpp"
#include "OAS/attributes/AttrSprite.hpp"
#include "OAS/attributes/AttrVelocity.hpp"
#include "OAS/attributes/AttrCollidable.hpp"
#include "OAS/SysBase.hpp"
#include "OAS/systems/SysTranslator.hpp"
#include "OAS/systems/SysController.hpp"
#include "OAS/systems/SysRenderer.hpp"
#include "OAS/systems/SysCollider.hpp"

// Resource
#include "resource/Resource.hpp"
#include "resource/ResourceLoader.hpp"
#include "resource/ResourceEnum.hpp"
#include "resource/DummyResource.hpp"
#include "resource/loaders/AudioWaveLoader.hpp"
#include "resource/loaders/GLTextureLoader.hpp"
#include "resource/loaders/GLShaderLoader.hpp"
#include "resource/loaders/DummyResourceLoader.hpp"

#include "resource/graphics/AnimBase.hpp"
#include "resource/graphics/animation/AnimDefault.hpp"
#include "resource/graphics/animation/AnimIdle.hpp"


// Network
#include "network/URL.hpp"
#include "network/Socket.hpp"
#include "network/SocketListener.hpp"
#include "network/NetworkManager.hpp"
#include "network/NetworkMessageTypes.hpp"
#include "network/NetworkMessage.hpp"
#include "network/NetworkSerde.hpp"

// GUI
#include "GUI/AppWindow.hpp"
#include "GUI/KeyCode.hpp"

//Time
#include "time/Clock.hpp"
#include "time/StopWatch.hpp"

// Render
#include "render/GLBuffer.hpp"
#include "render/GLProgram.hpp"
#include "render/GLRenderer.hpp"
#include "render/GLRenderable.hpp"
#include "render/GLShader.hpp"
#include "render/GLTexture.hpp"
#include "render/GLRenderContext.hpp"
#include "render/GLContext.hpp"
#include "render/GLText.hpp"
#include "render/GLSprite.hpp"
#include "render/renderImpl/Dummy/GLBufferDummyImpl.hpp"
#include "render/renderImpl/Dummy/GLProgramDummyImpl.hpp"
#include "render/renderImpl/Dummy/GLShaderDummyImpl.hpp"
#include "render/renderImpl/Dummy/GLTextureDummyImpl.hpp"
#include "render/renderImpl/Dummy/GLRenderableDummyImpl.hpp"
#include "render/renderImpl/Dummy/GLTextDummyImpl.hpp"
#include "render/renderImpl/Dummy/GLSpriteDummyImpl.hpp"
#include "render/renderImpl/Dummy/GLRenderContextDummyImpl.hpp"
#include "render/renderImpl/OpenGL/GLBufferOpenGLImpl.hpp"
#include "render/renderImpl/OpenGL/GLProgramOpenGLImpl.hpp"
#include "render/renderImpl/OpenGL/GLShaderOpenGLImpl.hpp"
#include "render/renderImpl/OpenGL/GLTextureOpenGLImpl.hpp"
#include "render/renderImpl/OpenGL/GLRenderableOpenGLImpl.hpp"
#include "render/renderImpl/OpenGL/GLTextOpenGLImpl.hpp"
#include "render/renderImpl/OpenGL/GLSpriteOpenGLImpl.hpp"
#include "render/renderImpl/Dummy/GLSpriteDummyImpl.hpp"
#include "render/renderImpl/OpenGL/GLRenderContextOpenGLImpl.hpp"

//Sound
#include "sound/AudioWave.hpp"

#ifdef _WINDOWS
//Network
#    include "platform/Win/network/SocketWinImpl.hpp"
#    include "platform/Win/network/SocketListenerWinImpl.hpp"
#elif __linux__
//Network
#    include "platform/Linux/network/SocketUnixImpl.hpp"
#    include "platform/Linux/network/SocketListenerUnixImpl.hpp"
#endif