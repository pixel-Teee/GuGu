#pragma once

#include "Renderer.h"

namespace GuGu{

    class VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer();

        virtual ~VulkanRenderer();

        virtual void init() override;

        virtual void onRender() override;

        virtual void onDestroy() override;
  
        virtual void createSurface(std::shared_ptr<WindowWidget> windowWidget) override;

        virtual void createSwapChain(std::shared_ptr<WindowWidget> windowWidget) override;
    };
}