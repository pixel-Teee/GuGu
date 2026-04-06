local EventSystem = {}
EventSystem.listeners = {}

function EventSystem:Register(eventName, target, methodName)
    self.listeners[eventName] = self.listeners[eventName] or {}
    table.insert(self.listeners[eventName], {target = target, methodName = methodName})
end

function EventSystem:Send(eventName, sender, ...)
    for _, listener in ipairs(self.listeners[eventName] or {}) do
        local targetObj = listener.target
        local method = targetObj[listener.methodName]
        if method then
            method(targetObj, ...)  -- 调用时传入 self 作为第一个参数
        end
    end
end

function EventSystem:Remove(eventName)
    self.listeners[eventName] = nil
end

return EventSystem