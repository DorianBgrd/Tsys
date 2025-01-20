#include <any>
#include "include/tsys.h"
#include "rapidjson/document.h"
#include <boost/python.hpp>
#include "include/defaultTypes.h"


std::string TSys::TypeHandler::PythonModule() const
{
    return "";
}


bool TSys::TypeHandler::HoldsPythonObject(const boost::python::object& o) const
{
    std::string mod = PythonModule();
    std::string cls = PythonName();

    if (!mod.empty())
    {
        const std::string& pymodule = boost::python::extract<const std::string&>(
                o.attr("__module__"));

        const std::string& pyname = boost::python::extract<const std::string&>(
                o.attr("__class__").attr("__name__"));

        return (pymodule + "." + pyname) == (mod + "." + cls);
    }

    const std::string& pyobjame = boost::python::extract<const std::string&>(o.attr("__name__"));

    return (PythonName() == pyobjame);
}


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


size_t TSys::TypeRegistry::GetHashFromName(const std::string& name, bool& success)
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


size_t TSys::TypeRegistry::GetHashFromPythonType(const std::string& name, bool& success)
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


class MatchHash
{
    size_t hash;
public:
    MatchHash(size_t h) {hash = h;}

    bool operator()(TSys::TypeHandler* f) const
    {
        return (f->Hash() == hash);
    }
};


bool TSys::TypeRegistry::IsRegistered(size_t hash) const
{
    auto h = std::find_if(handlers.begin(), handlers.end(), MatchHash(hash));

    return (h != handlers.end());
}


size_t TSys::TypeRegistry::HashFromPythonTypeName(const std::string& name, bool& success)
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


size_t TSys::TypeRegistry::HashFromApiName(const std::string& name, bool& success)
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


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromName(const std::string& name)
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


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromPythonName(const std::string& name)
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


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromPythonObject(
        const boost::python::object& o)
{
    for (auto* h : handlers)
    {
        if (h->HoldsPythonObject(o))
        {
            return h;
        }
    }

    return nullptr;
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromApiName(const std::string& name)
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

