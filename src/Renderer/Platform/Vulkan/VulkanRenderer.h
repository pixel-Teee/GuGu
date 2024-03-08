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
  
    };
}