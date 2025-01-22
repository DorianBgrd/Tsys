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

    boost::python::object pythonClass = o.attr("__class__");
    std::string pythonClassName = boost::python::extract<std::string>(
            pythonClass.attr("__name__"));

    if (pythonClassName == "type")
    {
        boost::python::object clsname = o.attr("__name__");
        pythonClassName = boost::python::extract<std::string>(clsname);
    }

    if (!mod.empty())
    {
        const std::string& pymodule = boost::python::extract<const std::string&>(
                o.attr("__module__"));

        return (pymodule + "." + pythonClassName) == (mod + "." + cls);
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


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandle(const std::any& value)
{
    return GetTypeHandle(value.type().hash_code());
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandle(
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


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandle(const std::string& name)
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


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandle(const char* name)
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

