#pragma once

//#ifndef BOOST_PYTHON_STATIC_LIB
//#define BOOST_PYTHON_STATIC_LIB
//#endif
#include <map>
#include <vector>
#include <set>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <any>
#include <functional>

#include "rapidjson/document.h"
#include "boost/python.hpp"

#include "api.h"

#ifndef NODELIBRARY2_ATTRIBUTE_CONFIG
#define NODELIBRARY2_ATTRIBUTE_CONFIG


namespace TSys
{
    template<typename From, typename To>
    struct StaticCastConverter
    {
        [[nodiscard]]
        std::any operator()(const std::any& from, const std::any& current) const
        {
            return std::make_any<To>(static_cast<To>(std::any_cast<From>(from)));
        }
    };


    typedef std::function<std::any(const std::any&, const std::any&)> Converter;


    /**
     * TypeHandler base class.
     * Pure virtual class that should be overriden to create
     * new handleable types.
     */
    struct TSYS_API TypeHandler
    {
    protected:
        std::unordered_map<std::type_index, Converter> converters;

    public:
        template<typename From, typename Converter>
        void RegisterConverter()
        {
            this->converters[std::type_index(typeid(From))] = Converter();
        }

        template<typename From>
        void RegisterConverter(Converter cvrt)
        {
            this->converters[std::type_index(typeid(From))] = cvrt;
        }

        Converter GetConverter(const std::any& from) const;

    public:
        /**
         * Serializes type value.
         * @param std::any v: value.
         * @param rapidjson::Value& value: json value.
         * @param rapidjson::Document document: json doc.
         */
        virtual void SerializeValue(const std::any& v, rapidjson::Value& value,
                                    rapidjson::Document& document) const = 0;

        /**
         * Deserializes type value.
         * @param std::any v: value.
         * @param rapidjson::Value& value: json value.
         * @param rapidjson::Document document: json doc.
         * @return std::any deserialized value.
         */
        virtual std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const = 0;

        /**
         * Serializes type construction.
         * @param std::any v: value.
         * @param rapidjson::Value& value: json value.
         * @param rapidjson::Document document: json doc.
         */
        virtual void SerializeConstruction(const std::any& v, rapidjson::Value& value,
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
        virtual bool CompareValue(const std::any&, const std::any&) const = 0;

        /**
         * Converts from python object.
         * @return std::any value.
         */
        virtual std::any FromPython(const boost::python::object&) const = 0;

        /**
         * Converts to python.
         * @return boost::python::object: boost python object.
         */
        virtual boost::python::object ToPython(const std::any&) const = 0;

        /**
         * Copies value.
         * @param std::any source: source value.
         * @return std::any copy.
         */
        virtual std::any CopyValue(const std::any& source) const = 0;

        /**
         * Returns handled type hash.
         * @return size_t hash.
         */
        virtual size_t Hash() const = 0;

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
        virtual size_t ValueHash(const std::any& val) const = 0;

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

        bool CompareValue(const std::any& v1, const std::any& v2) const override
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
            this->converters[std::type_index(typeid(From))] = StaticCastConverter<From, T>();
        }

        size_t ValueHash(const std::any& val) const override
        {
            std::hash<T> hash{};
            T v = std::any_cast<T>(val);
            return hash(v);
        }
    };


    typedef std::shared_ptr<TypeHandler> TypeHandlerPtr;


    class TSYS_API TypeRegistry
	{
    private:
        static TypeRegistry* registry;

	protected:
        std::unordered_map<std::type_index, TypeHandlerPtr> handlers;

        /**
         * Constructor (default).
         */
        TypeRegistry();

	public:
        bool RegisterType(
                const std::type_index& t,
                const TypeHandlerPtr& handler,
                bool force=false
        );

        template<class T>
        bool RegisterType(
                const TypeHandlerPtr& handler,
                bool force=false
        )
        {
            return RegisterType(
                    std::type_index(typeid(T)),
                    handler, force
            );
        }

        template <class T, class H>
        bool RegisterType(bool force=false)
        {
            return RegisterType(
                    std::type_index(typeid(T)),
                    std::make_shared<H>(),
                    force
            );
        }

        bool IsRegistered(const std::type_index& t) const;

        bool IsRegistered(const std::type_info& t) const;

        bool IsRegistered(const std::any& value) const;

        bool IsRegistered(const char* t) const;

        std::vector<std::string> RegisteredTypes() const;

        TypeHandlerPtr GetTypeHandle(const std::type_info& t) const;

        TypeHandlerPtr GetTypeHandle(const std::type_index& t) const;

        TypeHandlerPtr GetTypeHandle(const std::any& value) const;

        TypeHandlerPtr GetTypeHandle(const std::string& apiName) const;

        TypeHandlerPtr GetTypeHandle(const char* apiName) const;

        template<class T>
        TypeHandlerPtr GetTypeHandle() const
        {
            return GetTypeHandle(typeid(T));
        }

        static TypeRegistry* GetRegistry();
	};

}

#endif // NODELIBRARY2_ATTRIBUTE_CONFIG