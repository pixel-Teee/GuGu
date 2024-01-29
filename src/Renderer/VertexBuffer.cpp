#include <pch.h>

#include <Renderer/DeviceManager.h>

namespace GuGu{
    class VertexBuffer : public IRenderPass{
    private:

    public:
        using IRenderPass::IRenderPass;

        bool Init()
        {

        }

        void Animate()
        {

        }

        void BackBufferResizing() override
        {

        }

        //void Render(nvrhi::IFramebuffer* framebuffer) override
        //{
//
        //}


    };
}