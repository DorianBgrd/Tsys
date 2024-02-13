#include <any>
#include "include/tsys.h"
#include "rapidjson/document.h"
#include <boost/python.hpp>
#include "include/defaultTypes.h"


std::any TSys::TypeHandler::ConvertFrom(const std::any& sourceValue, std::any currentValue) const
{
    if (!CanConvertFrom(sourceValue))
    {
        return currentValue;
    }

    ConverterHandle converter = GetConverter(sourceValue);

    return converter->Convert(sourceValue, currentValue);
}


bool TSys::TypeHandler::CanConvertFrom(const std::any& value) const
{
    return bool(GetConverter(value).get());
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


size_t TSys::TypeRegistry::GetHashFromName(std::string name, bool& success)
{
    for (auto* h : handlers)
    {
        if (h->Name() == name)
        {
            success = true;
            return h->Hash();
        }
    }

    success = false;
    return {};
}


std::string TSys::TypeRegistry::GetNameFromHash(size_t hash, bool& success)
{
    for (auto* h : handlers)
    {
        if (h->Hash() == hash)
        {
            success = true;
            return h->Name();
        }
    }
    success = false;
    return {};
}


std::string TSys::TypeRegistry::GetApiNameFromHash(size_t hash, bool& success)
{
    for (auto* h : handlers)
    {
        if (h->Hash() == hash)
        {
            success = true;
            return h->ApiName();
        }
    }

    success = false;
    return {};
}


size_t TSys::TypeRegistry::GetHashFromPythonType(std::string name, bool& success)
{
    for (auto* h : handlers)
    {
        if (h->Name() == name)
        {
            success = true;
            return h->Hash();
        }
    }

    success = false;
	return {};
}


bool TSys::TypeRegistry::RegisterType(
            size_t hash, TypeHandler* handler,
            bool force
		)
{
    if (!force && handlers.find(handler) != handlers.end())
    {
        return false;
    }

    handlers.insert(handler);
    return true;
}


size_t TSys::TypeRegistry::HashFromPythonTypeName(std::string name, bool& success)
{
    for (auto* h : handlers)
    {
        if (h->PythonName() == name)
        {
            success = true;
            return h->Hash();
        }
    }

    success = false;
    return {};
}


size_t TSys::TypeRegistry::HashFromApiName(std::string name, bool& success)
{
    for (auto* h : handlers)
    {
        if (h->ApiName() == name)
        {
            success = true;
            return h->Hash();
        }
    }

    success = false;
    return {};
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandle(size_t hash)
{
    for (auto* h : handlers)
    {
        if (h->Hash() == hash)
        {
            return h;
        }
    }

    return nullptr;
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromName(std::string name)
{
    for (auto* h : handlers)
    {
        if (h->Name() == name)
        {
            return h;
        }
    }

    return nullptr;
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromPythonName(std::string name)
{
    for (auto* h : handlers)
    {
        if (h->PythonName() == name)
        {
            return h;
        }
    }

    return nullptr;
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromApiName(std::string name)
{
    for (auto* h : handlers)
    {
        if (h->ApiName() == name)
        {
            return h;
        }
    }

    return nullptr;
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

