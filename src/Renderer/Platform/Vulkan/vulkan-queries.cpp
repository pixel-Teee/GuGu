#include <pch.h>

#include "vulkan-backend.h"

#include <Renderer/misc.h>

namespace GuGu{

    namespace nvrhi::vulkan{
        void Device::waitEventQuery(IEventQuery* _query)
        {
            EventQuery* query = checked_cast<EventQuery*>(_query);

            if (query->commandListID == 0)
                return;

            auto& queue = *m_Queues[uint32_t(query->queue)];

            bool success = queue.waitCommandList(query->commandListID, ~0ull);
            assert(success);
            (void)success;
        }


        EventQueryHandle Device::createEventQuery() {
            EventQuery *query = new EventQuery();
            return EventQueryHandle::Create(query);
        }


        void Device::setEventQuery(IEventQuery *_query, CommandQueue queue) {

            EventQuery* query = checked_cast<EventQuery*>(_query);

            assert(query->commandListID == 0);

            query->queue = queue;
            query->commandListID = m_Queues[uint32_t(queue)]->getLastSubmittedID();
        }

        void Device::resetEventQuery(IEventQuery *_query) {
            EventQuery* query = checked_cast<EventQuery*>(_query);

            query->commandListID = 0;
        }


    }

}