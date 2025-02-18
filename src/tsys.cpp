#include <any>
#include "include/tsys.h"
#include "rapidjson/document.h"
#include <boost/python.hpp>
#include "include/defaultTypes.h"


TSys::Converter TSys::TypeHandler::GetConverter(const std::any& from) const
{
    auto idx = std::type_index(from.type());
    if (converters.find(idx) != converters.end())
    {
        return converters.at(idx);
    }

    return {};
}


std::any TSys::TypeHandler::ConvertFrom(const std::any& sourceValue, std::any currentValue) const
{
    if (!CanConvertFrom(sourceValue))
    {
        return {};
    }

    Converter converter = GetConverter(sourceValue);

    return converter(sourceValue, currentValue);
}


bool TSys::TypeHandler::CanConvertFrom(const std::any& value) const
{
    return bool(GetConverter(value));
}


bool TSys::TypeHandler::operator==(TypeHandler* h) const
{
    return Hash() == h->Hash();
}


TSys::TypeRegistry::TypeRegistry()
{
    RegisterType<TSys::Enum, TSys::EnumHandler>();
    RegisterType<TSys::AnyValue, TSys::AnyHandler>();
    RegisterType<std::string, StringHandler>();
    RegisterType<bool, BoolHandler>();
    RegisterType<int, IntHandler>();
    RegisterType<float, FloatHandler>();
    RegisterType<double, DoubleHandler>();
    RegisterType<None, NoneHandler>();
}


bool TSys::TypeRegistry::RegisterType(
            const std::type_index& t,
            const TypeHandlerPtr& handler,
            bool force
		)
{
    if (!force && handlers.find(t) != handlers.end())
    {
        return false;
    }

    handlers[t] = handler;
    return true;
}


bool TSys::TypeRegistry::IsRegistered(const std::type_index& t) const
{
    return (handlers.find(t) != handlers.end());
}


bool TSys::TypeRegistry::IsRegistered(const std::type_info& t) const
{
    return IsRegistered(std::type_index(t));
}


bool TSys::TypeRegistry::IsRegistered(const std::any& t) const
{
    return IsRegistered(t.type());
}


bool TSys::TypeRegistry::IsRegistered(const char* t) const
{
    return static_cast<bool>(GetTypeHandle(t));
}


std::vector<std::string> TSys::TypeRegistry::RegisteredTypes() const
{
    std::vector<std::string> types;
    for (const auto& handler : handlers)
    {
        types.push_back(handler.second->ApiName());
    }

    return types;
}


TSys::TypeHandlerPtr TSys::TypeRegistry::GetTypeHandle(const std::type_info& t) const
{
    return GetTypeHandle(std::type_index(t));
}


TSys::TypeHandlerPtr TSys::TypeRegistry::GetTypeHandle(const std::type_index& t) const
{
    auto iter = handlers.find(t);
    if (iter == handlers.end())
        return {};

    return iter->second;
}


TSys::TypeHandlerPtr TSys::TypeRegistry::GetTypeHandle(const std::any& value) const
{
    return GetTypeHandle(value.type());
}


TSys::TypeHandlerPtr TSys::TypeRegistry::GetTypeHandle(const std::string& name) const
{
    auto cmp = [name](
            const std::pair<std::type_index, TypeHandlerPtr>& h
    ) -> bool
    {
        return h.second->ApiName() == name;
    };

    auto h = std::find_if(handlers.begin(), handlers.end(), cmp);

    if (h == handlers.end())
        return {};

    return h->second;
}


TSys::TypeHandlerPtr TSys::TypeRegistry::GetTypeHandle(const char* name) const
{
    return GetTypeHandle(std::string(name));
}


TSys::TypeRegistry* TSys::TypeRegistry::registry = nullptr;


TSys::TypeRegistry* TSys::TypeRegistry::GetRegistry()
{
    if (!registry)
    {
        registry = new TypeRegistry();
    }

    return registry;
}

