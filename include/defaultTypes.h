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


        Enum(std::map<unsigned int, std::string> v);


        Enum(std::map<unsigned int, std::string> v, unsigned int i);


        Enum(std::vector<std::string> v);


        Enum(std::vector<std::string> v, unsigned int i);


        std::string CurrentValue() const;


        unsigned int CurrentIndex() const;


        bool SetCurrentIndex(unsigned int index);


        bool SetCurrentValue(std::string value);


        void AddValue(int index, std::string value);


        std::vector<int> Indices();


        std::string ValueAtIndex(int index);

        bool operator ==(Enum& other) const
        {
            return (CurrentValue() == other.CurrentValue());
        }

        bool operator ==(const Enum* other) const
        {
            return (CurrentValue() == other->CurrentValue());
        }
    };


    // Any
    struct TSYS_API InvalidAnyCast
    {
    public:
        InvalidAnyCast() = default;

        static size_t Hash();

        static bool IsInvalid(std::any other);

        static bool IsValid(std::any other);
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

        void SetInput(std::any val);

        size_t Hash() const;

        std::string Name() const;

        std::any InputValue() const;

        boost::python::object Python_Get();

        void Python_Set(boost::python::object val);

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

            auto* handle = TypeRegistry::GetRegistry()->GetTypeHandle(to.type().hash_code());
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

        std::string PythonName() const override;

        std::any InitValue() const override;

        std::any CopyValue(std::any source) const override;

        std::any FromPython(boost::python::object obj) const override;

        boost::python::object ToPython(std::any value) const override;

        void SerializeValue(std::any v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(std::any, rapidjson::Value& value) const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

    };


    struct TSYS_API BoolHandler: TSys::GenericTypeHandler<bool>
    {
        BoolHandler();

        std::string ApiName() const override;

        std::any InitValue() const override;

        std::any CopyValue(std::any source) const override;

        std::string PythonName() const override;

        std::any FromPython(boost::python::object obj) const override;

        boost::python::object ToPython(std::any value) const override;

        void SerializeValue(std::any v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(std::any, rapidjson::Value& value) const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

    };


    // Int
    struct TSYS_API IntHandler: TSys::GenericTypeHandler<int>
    {
        IntHandler();

        std::string ApiName() const override;

        std::any InitValue() const override;

        std::any CopyValue(std::any source) const override;

        std::string PythonName() const override;

        std::any FromPython(boost::python::object obj) const override;

        boost::python::object ToPython(std::any value) const override;

        void SerializeValue(std::any v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(std::any, rapidjson::Value& value) const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;
    };


    // Float
    struct TSYS_API FloatHandler: TSys::GenericTypeHandler<float>
    {
        FloatHandler();

        std::string ApiName() const override;

        std::string PythonName() const override;

        std::any InitValue() const override;

        std::any CopyValue(std::any source) const override;

        std::any FromPython(boost::python::object obj) const override;

        boost::python::object ToPython(std::any value) const override;

        void SerializeValue(std::any v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(std::any, rapidjson::Value& value) const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& doc) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;
    };


    // Double
    struct DoubleHandler: TSys::GenericTypeHandler<double>
    {
        DoubleHandler();

        std::string ApiName() const override;

        std::string PythonName() const override;

        std::any InitValue() const override;

        std::any CopyValue(std::any source) const override;

        std::any FromPython(boost::python::object obj) const override;

        boost::python::object ToPython(std::any value) const override;

        void SerializeValue(std::any v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(std::any, rapidjson::Value& value) const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
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

        std::string Name() const override
        {
            return std::string(typeid(Enum).name());
        }

        std::string PythonName() const override;

        std::any InitValue() const override;

        std::any CopyValue(std::any source) const override;

        std::any FromPython(boost::python::object obj) const override;

        boost::python::object ToPython(std::any value) const override;

        void SerializeValue(std::any v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(std::any, rapidjson::Value& value)
                                  const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& doc)
                                   const override;

        std::any DeserializeConstruction(rapidjson::Value& value)
                                         const override;

        size_t ValueHash(std::any value) const override;

        bool CompareValue(std::any v1, std::any v2) const override
        {
            Enum ev1;
            Enum ev2;
            try
            {
                ev1 = std::any_cast<Enum>(v1);
                ev2 = std::any_cast<Enum>(v2);
            }
            catch(std::bad_any_cast&)
            {
                return false;
            }

            return (ev1 == ev2);
        }
    };


    struct AnyHandler: TSys::TypeHandler
    {
        AnyHandler();

        std::string ApiName() const override;

        size_t Hash() const override
        {
            return typeid(AnyValue).hash_code();
        }

        std::string Name() const override
        {
            return std::string(typeid(AnyValue).name());
        }

        std::string PythonName() const override;

        std::any InitValue() const override;

        std::any CopyValue(std::any source) const override;

        std::any FromPython(boost::python::object obj) const override;

        boost::python::object ToPython(std::any value) const override;

        void SerializeValue(std::any v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(std::any, rapidjson::Value& value)
                                  const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& doc)
                                   const override;

        std::any DeserializeConstruction(rapidjson::Value& value)
                                         const override;

        size_t ValueHash(std::any val) const override;

        bool CompareValue(std::any v1, std::any v2) const override
        {
            AnyValue av1;
            AnyValue av2;
            try
            {
                av1 = std::any_cast<AnyValue>(v1);
                av2 = std::any_cast<AnyValue>(v2);
            }
            catch(std::bad_any_cast&)
            {
                return false;
            }

            return (av1 == av2);
        }
    };


    class None
    {
    public:
        None() = default;

        bool operator ==(const None& other)
        {
            return true;
        }
    };


    struct NoneHandler: TSys::BaseTypeHandler<None>
    {
        std::string ApiName() const override;

        std::string PythonName() const override;

        std::any InitValue() const override;

        std::any CopyValue(std::any source) const override;

        std::any FromPython(boost::python::object obj) const override;

        boost::python::object ToPython(std::any value) const override;

        void SerializeValue(std::any v, rapidjson::Value& jsonValue,
                            rapidjson::Document& doc) const override;

        std::any DeserializeValue(std::any, rapidjson::Value& value)
                                  const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& doc)
                                   const override;

        std::any DeserializeConstruction(rapidjson::Value& value)
                                         const override;

        size_t ValueHash(std::any val) const override;
    };

}

