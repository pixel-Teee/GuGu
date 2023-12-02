#include "pch.h"

#include "AndroidApplication.h"

namespace GuGu{
    AndroidApplication::AndroidApplication()
    {

    }

    AndroidApplication::~AndroidApplication() {

    }

    std::shared_ptr<Application> CreateApplicationFactory()
    {
        return std::make_shared<AndroidApplication>();
    }
}