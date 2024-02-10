#include <any>
#include "include/tsys.h"
#include "rapidjson/document.h"
#include <boost/python.hpp>
#include "include/StandardTypes.h"


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


size_t TSys::TypeRegistry::GetHashFromName(std::string name, bool& success)
{
	if (hashesPerNames.find(name) == hashesPerNames.end())
	{
		success = false;
		return size_t();
	}

	success = true;
	return hashesPerNames.at(name);
}


std::string TSys::TypeRegistry::GetNameFromHash(size_t hash, bool& success)
{
	if (namesPerHashes.find(hash) == namesPerHashes.end())
	{
		success = false;
		return std::string();
	}

	success = true;
	return namesPerHashes.at(hash);
}


size_t TSys::TypeRegistry::GetHashFromPythonType(std::string name, bool& success)
{
	if (hashesPerPythonNames.find(name) == hashesPerPythonNames.end())
	{
		success = false;
		return size_t();
	}

	return hashesPerPythonNames.at(name);
}


bool TSys::TypeRegistry::RegisterType(
            size_t hash, TypeHandler* handler,
            bool force
		)
{
    if ((std::find(hashes.begin(), hashes.end(), hash) != hashes.end()) && !force)
    {
        return false;
    }

    hashes.push_back(hash);
    hashesHandlers[hash] = handler;
    pythonNamesPerHashes[hash] = handler->PythonName();
    namesPerHashes[hash] = handler->Name();
    hashesPerNames[handler->Name()] = hash;
    hashesPerPythonNames[handler->PythonName()] = hash;
    hashesPerApiNames[handler->ApiName()] = hash;
    return true;
}


size_t TSys::TypeRegistry::HashFromPythonTypeName(std::string name)
{
	if (hashesPerPythonNames.find(name) == hashesPerPythonNames.end())
	{
		return -1;
	}

	return hashesPerPythonNames.at(name);
}


size_t TSys::TypeRegistry::HashFromApiName(std::string name)
{
    if (hashesPerApiNames.find(name) == hashesPerApiNames.end())
    {
        return -1;
    }

    return hashesPerApiNames.at(name);
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandle(size_t hash)
{
    if (std::find(hashes.begin(), hashes.end(), hash) == hashes.end())
    {
        return nullptr;
    }

    return hashesHandlers.at(hash);
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromName(std::string name)
{
    if (hashesPerNames.find(name) == hashesPerNames.end())
    {
        return nullptr;
    }

    return GetTypeHandle(hashesPerNames.at(name));
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromPythonName(std::string name)
{
    if (hashesPerPythonNames.find(name) == hashesPerPythonNames.end())
    {
        return nullptr;
    }

    return GetTypeHandle(hashesPerPythonNames.at(name));
}


TSys::TypeHandler* TSys::TypeRegistry::GetTypeHandleFromApiName(std::string name)
{
    if (hashesPerApiNames.find(name) == hashesPerApiNames.end())
    {
        return nullptr;
    }

    return GetTypeHandle(hashesPerApiNames.at(name));
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

