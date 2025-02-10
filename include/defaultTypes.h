#pragma once
#include "tsys.h"

//#ifndef BOOST_PYTHON_STATIC_LIB
//#define BOOST_PYTHON_STATIC_LIB
//#endif

#include <any>
#include <string>
#include <map>
#include <vector>
#include "rapidjson/reader.h"
#include "rapidjson/document.h"

#include "api.h"


namespace TSys
{
    template <typename T>
    std::any ExtractPythonToAny(const boost::python::object& pyObj)
    {
        boost::python::extract<T> extractor(pyObj);
        if (!extractor.check())
            return {};

        return std::make_any<T>(extractor());
    }


    enum class Status
    {
        None,
        Failed,
        Success
    };

    struct Success
    {
        bool status = false;
    };


    // Declare types
    class TSYS_API Enum
    {
    protected:
        std::map<unsigned int, std::string> values;
        unsigned int currentIndex;

    public:
        Enum();


        Enum(const Enum& other);


        Enum(const std::map<unsigned int, std::string>& v);


        Enum(const std::map<unsigned int, std::string>& v, unsigned int i);


        Enum(const std::vector<std::string>& v);


        Enum(const std::vector<std::string>& v, unsigned int i);


        std::string CurrentValue() const;


        unsigned int CurrentIndex() const;


        bool SetCurrentIndex(unsigned int index);


        bool SetCurrentValue(std::string value);


        void AddValue(int index, std::string value);


        std::vector<int> Indices();


        std::string ValueAtIndex(int index);


        bool operator ==(const Enum& other) const;


        bool operator ==(const Enum* other) const;
    };


    // Any
    struct TSYS_API InvalidAnyCast
    {
    public:
        InvalidAnyCast() = default;

        static size_t Hash();

        static bool IsInvalid(const std::any& other);

        static bool IsValid(const std::any& other);
    };


    class TSYS_API AnyValue
    {
    protected:
        std::any value;

    public:
        explicit AnyValue() = default;

        template<class T>
        explicit AnyValue(T v)
        {
            value = std::make_any<T>(v);
        }

        explicit AnyValue(std::any v);

        template<class T>
        T Get(Success* success=nullptr)
        {
            if (typeid(T).hash_code() != value.type().hash_code() && success)
            {
                success->status = false;
            }
            else if (success)
            {
                success->status = true;
            }

            return std::any_cast<T>(value);
        }

        template<class T>
        void Set(T newValue)
        {
            value = std::make_any<T>(newValue);
        }

        void SetInput(const std::any& val);

        size_t Hash() const;

        std::string Name() const;

        std::any InputValue() const;

        boost::python::object Python_Get();

        bool Python_Set(boost::python::object val);

        std::any ConvertTo(size_t hash);

        bool  operator == (AnyValue& other);

        bool operator == (std::any& other);
    };


    struct AnyConverter: public TSys::TypeConverter
    {
        [[nodiscard]]
        std::any Convert(std::any from, std::any to) const override
        {
            auto anyval = std::any_cast<TSys::AnyValue>(from);
            if (anyval.Hash() == to.type().hash_code())
            {
                return anyval.InputValue();
            }

            auto handle = TypeRegistry::GetRegistry()->GetTypeHandle(to.type().hash_code());
            if (!handle)
            {
                return to;
            }

            if (!handle->CanConvertFrom(anyval.Hash()))
            {
                return to;
            }

            return handle->ConvertFrom(anyval.InputValue(), to);
        }
    };


    // String
    struct TSYS_API StringHandler: GenericTypeHandler<std::string>
    {
        StringHandler();

        std::string ApiName() const override;

        std::any InitValue() const override;

        std::any CopyValue(const std::any& source) const override;

        std::any FromPython(const boost::python::object& obj) const override;

        boost::python::object ToPython(const std::any& value) const override;

        void SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

    };


    struct TSYS_API BoolHandler: TSys::GenericTypeHandler<bool>
    {
        BoolHandler();

        std::string ApiName() const override;

        std::any InitValue() const override;

        std::any CopyValue(const std::any& source) const override;

        std::any FromPython(const boost::python::object& obj) const override;

        boost::python::object ToPython(const std::any& value) const override;

        void SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(const std::any&, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

    };


    // Int
    struct TSYS_API IntHandler: TSys::GenericTypeHandler<int>
    {
        IntHandler();

        std::string ApiName() const override;

        std::any InitValue() const override;

        std::any CopyValue(const std::any& source) const override;

        std::any FromPython(const boost::python::object& obj) const override;

        boost::python::object ToPython(const std::any& value) const override;

        void SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(const std::any&, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any&, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;
    };


    // Float
    struct TSYS_API FloatHandler: TSys::GenericTypeHandler<float>
    {
        FloatHandler();

        std::string ApiName() const override;

        std::any InitValue() const override;

        std::any CopyValue(const std::any& source) const override;

        std::any FromPython(const boost::python::object& obj) const override;

        boost::python::object ToPython(const std::any& value) const override;

        void SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(const std::any&, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;
    };


    // Double
    struct DoubleHandler: TSys::GenericTypeHandler<double>
    {
        DoubleHandler();

        std::string ApiName() const override;

        std::any InitValue() const override;

        std::any CopyValue(const std::any& source) const override;

        std::any FromPython(const boost::python::object& obj) const override;

        boost::python::object ToPython(const std::any& value) const override;

        void SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(const std::any&, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;
    };


    // Enum
    struct EnumHandler: TSys::TypeHandler
    {
        EnumHandler();

        std::string ApiName() const override;

        size_t Hash() const override
        {
            return typeid(Enum).hash_code();
        }

        std::any InitValue() const override;

        std::any CopyValue(const std::any& source) const override;

        std::any FromPython(const boost::python::object& obj) const override;

        boost::python::object ToPython(const std::any& value) const override;

        void SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(const std::any&, rapidjson::Value& value)
                                  const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& doc)
                                   const override;

        std::any DeserializeConstruction(rapidjson::Value& value)
                                         const override;

        size_t ValueHash(const std::any& value) const override;

        bool CompareValue(const std::any& v1, const std::any& v2) const override;
    };


    struct AnyHandler: TSys::TypeHandler
    {
        AnyHandler();

        std::string ApiName() const override;

        size_t Hash() const override;

        std::any InitValue() const override;

        std::any CopyValue(const std::any& source) const override;

        std::any FromPython(const boost::python::object& obj) const override;

        boost::python::object ToPython(const std::any& value) const override;

        void SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(const std::any&, rapidjson::Value& value)
                                  const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& doc)
                                   const override;

        std::any DeserializeConstruction(rapidjson::Value& value)
                                         const override;

        size_t ValueHash(const std::any& val) const override;

        bool CompareValue(const std::any& v1, const std::any& v2) const override;
    };


    class None
    {
    public:
        None() = default;

        bool operator==(const None& other) const;
    };


    struct NoneHandler: TSys::BaseTypeHandler<None>
    {
        std::string ApiName() const override;

        std::any InitValue() const override;

        std::any CopyValue(const std::any& source) const override;

        std::any FromPython(const boost::python::object& obj) const override;

        boost::python::object ToPython(const std::any& value) const override;

        void SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(const std::any&, rapidjson::Value& value)
                                  const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& doc)
                                   const override;

        std::any DeserializeConstruction(rapidjson::Value& value)
                                         const override;

        size_t ValueHash(const std::any& val) const override;
    };

}

