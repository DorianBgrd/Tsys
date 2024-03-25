#pragma once

//#ifndef BOOST_PYTHON_STATIC_LIB
//#define BOOST_PYTHON_STATIC_LIB
//#endif
#include <map>
#include <vector>
#include <set>
#include <string>
#include <typeinfo>
#include <any>
#include <functional>

#include "rapidjson/document.h"
#include "boost/python.hpp"

#include "api.h"

#ifndef NODELIBRARY2_ATTRIBUTE_CONFIG
#define NODELIBRARY2_ATTRIBUTE_CONFIG


namespace TSys
{
    struct TypeConverter
    {
        [[nodiscard]]
        virtual std::any Convert(std::any from, std::any to) const = 0;
    };


    template<typename From, typename To>
    struct StaticCastConverter: public TypeConverter
    {
        [[nodiscard]]
        std::any Convert(std::any from, std::any to) const override
        {
            return std::make_any<To>(static_cast<To>(std::any_cast<From>(from)));
        }
    };


    typedef std::shared_ptr<TypeConverter> ConverterHandle;


    /**
     * TypeHandler base class.
     * Pure virtual class that should be overriden to create
     * new handleable types.
     */
    struct TSYS_API TypeHandler
    {
    protected:
        std::unordered_map<size_t, ConverterHandle> converters;

    public:
        template<typename From, typename Converter>
        void RegisterConverter()
        {
            this->converters[typeid(From).hash_code()] = std::make_shared<Converter>();
        }

        ConverterHandle GetConverter(std::any from) const
        {
            size_t sourceHash = from.type().hash_code();
            if (converters.find(sourceHash) != converters.end())
            {
                return converters.at(sourceHash);
            }

            return {};
        }

    public:
        /**
         * Serializes type value.
         * @param std::any v: value.
         * @param rapidjson::Value& value: json value.
         * @param rapidjson::Document document: json doc.
         */
        virtual void SerializeValue(std::any v, rapidjson::Value& value,
                                    rapidjson::Document& document) const = 0;

        /**
         * Deserializes type value.
         * @param std::any v: value.
         * @param rapidjson::Value& value: json value.
         * @param rapidjson::Document document: json doc.
         * @return std::any deserialized value.
         */
        virtual std::any DeserializeValue(std::any v, rapidjson::Value& value) const = 0;

        /**
         * Serializes type construction.
         * @param std::any v: value.
         * @param rapidjson::Value& value: json value.
         * @param rapidjson::Document document: json doc.
         */
        virtual void SerializeConstruction(std::any v, rapidjson::Value& value,
                                           rapidjson::Document& document) const = 0;

        /**
         * Deserializes type construction.
         * @param std::any v: value.
         * @param rapidjson::Value& value: json value.
         * @param rapidjson::Document document: json doc.
         * @return std::any built value.
         */
        virtual std::any DeserializeConstruction(rapidjson::Value& value) const = 0;

        /**
         * Initializes base value.
         * @return std::any base value.
         */
        virtual std::any InitValue() const = 0;

        /**
         * Compares 2 values.
         * @return bool: comparison.
         */
        virtual bool CompareValue(std::any, std::any) const = 0;

        /**
         * Converts from python object.
         * @return std::any value.
         */
        virtual std::any FromPython(boost::python::object) const = 0;

        /**
         * Converts to python.
         * @return boost::python::object: boost python object.
         */
        virtual boost::python::object ToPython(std::any) const = 0;

        /**
         * Copies value.
         * @param std::any source: source value.
         * @return std::any copy.
         */
        virtual std::any CopyValue(std::any source) const = 0;

        /**
         * Returns handled type hash.
         * @return size_t hash.
         */
        virtual size_t Hash() const = 0;

        /**
         * Returns c++ type name.
         * @return std::string type name.
         */
        virtual std::string Name() const = 0;

        /**
         * Returns python type name.
         * @return std::string type name.
         */
        virtual std::string PythonName() const = 0;

        /**
         * Returns name used in Api when referencing
         * the name.
         * @return std::string name.
         */
        virtual std::string ApiName() const = 0;

        /**
         * Returns value hash.
         * @param std::any val: value.
         * @return size_t hash.
         */
        virtual size_t ValueHash(std::any val) const = 0;

    public:
        /**
         * Converts from source value to handled type value.
         * @param std::any sourceValue: value.
         * @param std::any currentValue: current attribute value, can be useful for
         * modifications, like for int -> Enum conversion that changes enum index.
         * @return std::any result value / empty std::any if no conversion could
         * be performed.
         */
        virtual std::any ConvertFrom(const std::any& sourceValue, std::any currentValue) const;

        /**
          * Returns whether type can be converted to
          * handled type.
          * @param std::any value to convert from.
          * @return bool convertible from.
          */
        virtual bool CanConvertFrom(const std::any& value) const;

        bool operator==(TypeHandler* h) const;
    };


    template<class T>
    struct TSYS_API BaseTypeHandler: TypeHandler
    {
        size_t Hash() const override
        {
            return typeid(T).hash_code();
        }

        std::string Name() const override
        {
            return std::string(typeid(T).name());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            return (std::any_cast<T>(v1) == std::any_cast<T>(v2));
        }
    };

    template<class T>
    struct TSYS_API GenericTypeHandler: BaseTypeHandler<T>
    {
        template<typename From>
        void RegisterConstructibleConverter()
        {
            this->converters[typeid(From).hash_code()] = std::make_shared<StaticCastConverter<From, T>>();
        }

        size_t ValueHash(std::any val) const override
        {
            std::hash<T> hash{};
            T v = std::any_cast<T>(val);
            return hash(v);
        }
    };


    class TSYS_API TypeRegistry
	{
    private:
        static TypeRegistry* registry;

	protected:
        std::set<TypeHandler*> handlers;

        /**
         * Constructor (default).
         */
        TypeRegistry();

	public:
		size_t GetHashFromName(std::string name, bool& success);

		std::string GetNameFromHash(size_t hash, bool& success);

        std::string GetApiNameFromHash(size_t hash, bool& success);

        size_t GetHashFromPythonType(std::string name, bool& success);

        size_t HashFromPythonTypeName(std::string name, bool& success);

        size_t HashFromApiName(std::string name, bool& success);

        bool RegisterType(size_t hash, TypeHandler* handler, bool force=false);

        template<class T>
        bool RegisterType(
                TypeHandler* handler,
                bool force=false
        )
        {
            size_t hash = typeid(T).hash_code();
            return RegisterType(hash, handler, force);
        }

        template <class T, class H>
        bool RegisterType(bool force=false)
        {
            size_t hash = typeid(T).hash_code();
            TypeHandler* handler = new H();
            return RegisterType(hash, handler, force);
        }

        TypeHandler* GetTypeHandle(size_t hash);

        TypeHandler* GetTypeHandleFromName(std::string name);

        TypeHandler* GetTypeHandleFromPythonName(std::string name);

        TypeHandler* GetTypeHandleFromApiName(std::string name);

        template<class T>
        TypeHandler* GetTypeHandle()
        {
            return GetTypeHandle(typeid(T).hash_code());
        }

        static TypeRegistry* GetRegistry();
	};

}

#endif // NODELIBRARY2_ATTRIBUTE_CONFIG