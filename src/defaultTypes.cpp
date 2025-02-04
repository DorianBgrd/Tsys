#include "include/defaultTypes.h"

#include <boost/python.hpp>

#include <any>
#include <string>
#include <map>
#include <vector>
#include "rapidjson/reader.h"
#include "rapidjson/document.h"


TSys::Enum::Enum()
{
    currentIndex = 0;
}


TSys::Enum::Enum(const Enum& other)
{
    values = other.values;
    currentIndex = other.currentIndex;
}


TSys::Enum::Enum(const std::map<unsigned int, std::string>& v)
{
    values = v;
    currentIndex = 0;
}


TSys::Enum::Enum(const std::map<unsigned int, std::string>& v, unsigned int i)
{
    values = v;
    currentIndex = i;
}


TSys::Enum::Enum(const std::vector<std::string>& v) {
    for (unsigned int i = 0; i < v.size(); i++) {
        values[i] = v[i];
    }

    currentIndex = 0;
}


TSys::Enum::Enum(const std::vector<std::string>& v, unsigned int i)
{
    for (unsigned int u = 0; u < v.size(); u++)
    {
        values[u] = v[u];
    }

    currentIndex = i;
}


std::string TSys::Enum::CurrentValue() const
{
    if (values.find(currentIndex) == values.end())
    {
        return "";
    }

    return values.at(currentIndex);
}


unsigned int TSys::Enum::CurrentIndex() const
{
    return currentIndex;
}


bool TSys::Enum::SetCurrentIndex(unsigned int index)
{
    if (values.find(index) == values.end())
    {
        return false;
    }

    currentIndex = index;
    return true;
}


bool TSys::Enum::SetCurrentValue(std::string value)
{
    for (std::pair<int, std::string> p : values)
    {
        if (p.second == value)
        {
            SetCurrentIndex(p.first);
            return true;
        }
    }

    return false;
}


void TSys::Enum::AddValue(int index, std::string value)
{
    values[index] = value;
}


std::vector<int> TSys::Enum::Indices()
{
    std::vector<int> v;
    for (std::pair<int, std::string> p : values)
    {
        v.push_back(p.first);
    }

    return v;
}

std::string TSys::Enum::ValueAtIndex(int index)
{
    return values.at(index);
}


bool TSys::Enum::operator==(const Enum& other) const
{
    return (CurrentValue() == other.CurrentValue());
}


bool TSys::Enum::operator==(const Enum* other) const
{
    return (CurrentValue() == other->CurrentValue());
}


// Invalid Any Cast.
size_t TSys::InvalidAnyCast::Hash()
{
    return typeid(InvalidAnyCast).hash_code();
}


bool TSys::InvalidAnyCast::IsInvalid(const std::any& other)
{
    return (other.type().hash_code() == Hash());
}


bool TSys::InvalidAnyCast::IsValid(const std::any& other)
{
    return (!IsInvalid(other));
}


// Any value
TSys::AnyValue::AnyValue(std::any v)
{
    value = std::move(v);
}


void TSys::AnyValue::SetInput(const std::any& val)
{
    value = val;
}


size_t TSys::AnyValue::Hash() const
{
    return value.type().hash_code();
}


std::string TSys::AnyValue::Name() const
{
    return value.type().name();
}


std::any TSys::AnyValue::InputValue() const
{
    return value;
}

boost::python::object TSys::AnyValue::Python_Get()
{
    auto handler = TypeRegistry::GetRegistry()->GetTypeHandle(Hash());
    if (!handler)
    {
        return {};
    }

    return handler->ToPython(value);
}

bool TSys::AnyValue::Python_Set(boost::python::object val)
{
    auto handler = TypeRegistry::GetRegistry()->GetTypeHandle(val);
    if (!handler)
    {
        return false;
    }

    value = handler->FromPython(val);
    return true;
}

std::any TSys::AnyValue::ConvertTo(size_t hash)
{
    if (hash == Hash())
    {
        return value;
    }

    auto handler = TypeRegistry::GetRegistry()->GetTypeHandle(hash);
    if (!handler)
    {
        return std::make_any<InvalidAnyCast>(InvalidAnyCast());
    }

    return handler->ConvertFrom(value, value);
}

bool  TSys::AnyValue::operator == (AnyValue& other)
{
    if (Hash() != other.Hash())
    {
        return false;
    }

    auto handler = TypeRegistry::GetRegistry()->GetTypeHandle(Hash());

    if (!handler)
    {
        return false;
    }

    return handler->CompareValue(value, other.InputValue());
}

bool TSys::AnyValue::operator == (std::any& other)
{
    if (Hash() != other.type().hash_code())
    {
        return false;
    }

    auto handler = TypeRegistry::GetRegistry()->GetTypeHandle(Hash());
    if (!handler)
    {
        return false;
    }

    return handler->CompareValue(value, other);
}


// String
template<typename N>
struct NumberToStr: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        int val = std::any_cast<N>(from);

        try
        {
            std::string sval = std::to_string(val);
            return std::make_any<std::string>(sval);
        }
        catch(...)
        {
            return to;
        }
    }
};


struct EnumToStr: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto enum_ = std::any_cast<TSys::Enum>(from);
        return std::make_any<std::string>(enum_.CurrentValue());
    }
};



TSys::StringHandler::StringHandler()
{
    RegisterConverter<int, NumberToStr<int>>();
    RegisterConverter<float, NumberToStr<float>>();
    RegisterConverter<double, NumberToStr<double>>();
    RegisterConverter<Enum, EnumToStr>();
    RegisterConverter<AnyValue, AnyConverter>();
}


std::any TSys::StringHandler::InitValue() const
{
    return std::make_any<std::string>(std::string(""));
}


std::any TSys::StringHandler::CopyValue(const std::any& source) const
{
    std::string value = std::string(std::any_cast<std::string>(source));

    return std::make_any<std::string>(value);
}


std::string TSys::StringHandler::ApiName() const
{
    return "String";
}


std::string TSys::StringHandler::PythonName() const
{
    return "str";
}


std::string TSys::StringHandler::PythonModule() const
{
    return "builtins";
}


std::any TSys::StringHandler::FromPython(const boost::python::object& obj) const
{
    if (!PyUnicode_Check(obj.ptr()))
    {
        return std::make_any<std::string>("");
    }

    std::string val = PyUnicode_AsUTF8(obj.ptr());
    return std::make_any<std::string>(val);
}


boost::python::object TSys::StringHandler::ToPython(const std::any& value) const
{
    return boost::python::object(std::any_cast<std::string>(value));
}


void TSys::StringHandler::SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                                         rapidjson::Document& doc) const
{
    rapidjson::Value stringValue(rapidjson::kStringType);

    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    jsonValue.PushBack(rapidjson::Value().SetString(
                               vname.c_str(), (rapidjson::SizeType)vname.size(), doc.GetAllocator()),
                       doc.GetAllocator());

    std::string saveValue = std::any_cast<std::string>(v);
    rapidjson::Value& _v = stringValue.SetString(
            rapidjson::StringRef(saveValue.c_str(), (rapidjson::SizeType)saveValue.size()),
            doc.GetAllocator());

    jsonValue.PushBack(_v, doc.GetAllocator());
}


std::any TSys::StringHandler::DeserializeValue(const std::any&, rapidjson::Value& value) const
{
    rapidjson::Value& result = value.GetArray()[1];
    std::string res = result.GetString();
    return std::make_any<std::string>(res);
}


void TSys::StringHandler::SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                                rapidjson::Document& doc) const
{
    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    // jsonValue.PushBack(rapidjson::StringRef(vname.c_str()), doc.GetAllocator());
    value.PushBack(
            rapidjson::Value().SetString(
                    vname.c_str(), (rapidjson::SizeType)vname.size(),
                    doc.GetAllocator()
            ),
            doc.GetAllocator()
    );
}


std::any TSys::StringHandler::DeserializeConstruction(rapidjson::Value& value) const
{
    return InitValue();
}


// Bool
struct StrToBool: TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        bool empty = std::any_cast<std::string>(from).empty();
        return std::make_any<bool>(empty);
    }
};


struct EnumToBool: TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        return std::make_any<bool>(std::any_cast<TSys::Enum>(from).CurrentIndex());
    }
};


TSys::BoolHandler::BoolHandler(): TSys::GenericTypeHandler<bool>()
{
    RegisterConstructibleConverter<int>();
    RegisterConstructibleConverter<float>();
    RegisterConstructibleConverter<double>();
    RegisterConverter<std::string, StrToBool>();
    RegisterConverter<Enum, EnumToBool>();
    RegisterConverter<AnyValue, AnyConverter>();
}


std::string TSys::BoolHandler::ApiName() const
{
    return "Bool";
}


std::any TSys::BoolHandler::InitValue() const
{
    return std::make_any<bool>(true);
}


std::any TSys::BoolHandler::CopyValue(const std::any& source) const
{
    bool value = std::any_cast<bool>(source);

    return std::make_any<bool>(value);
}


std::string TSys::BoolHandler::PythonName() const
{
    return "bool";
}


std::string TSys::BoolHandler::PythonModule() const
{
    return "builtins";
}


std::any TSys::BoolHandler::FromPython(const boost::python::object& obj) const
{
    bool value = boost::python::extract<bool>(obj);
    return std::make_any<bool>(value);
}


boost::python::object TSys::BoolHandler::ToPython(const std::any& value) const
{
    return boost::python::object(std::any_cast<bool>(value));
}


void TSys::BoolHandler::SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                                       rapidjson::Document& doc) const
{
    bool success;
    rapidjson::Value stringValue(rapidjson::kStringType);

    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    jsonValue.PushBack(rapidjson::Value().SetString(
                               vname.c_str(), (rapidjson::SizeType)vname.size(), doc.GetAllocator()),
                       doc.GetAllocator());

    rapidjson::Value intValue;
    bool saveValue = std::any_cast<bool>(v);
    intValue.SetBool(saveValue);

    jsonValue.PushBack<bool>(saveValue, doc.GetAllocator());
}


std::any TSys::BoolHandler::DeserializeValue(const std::any&, rapidjson::Value& value) const
{
    bool result = value.GetArray()[1].GetBool();
    return std::make_any<bool>(result);
}


void TSys::BoolHandler::SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                              rapidjson::Document& doc) const
{
    bool success;
    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    // jsonValue.PushBack(rapidjson::StringRef(vname.c_str()), doc.GetAllocator());
    value.PushBack(
            rapidjson::Value().SetString(
                    vname.c_str(), (rapidjson::SizeType)vname.size(),
                    doc.GetAllocator()
            ),
            doc.GetAllocator()
    );
}


std::any TSys::BoolHandler::DeserializeConstruction(rapidjson::Value& value) const
{
    return InitValue();
}


// Int
struct StrToInt: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        try
        {
            return std::make_any<int>(std::stoi(std::any_cast<std::string>(from)));
        }
        catch (...)
        {
            return 0;
        }
    }
};


struct EnumToInt: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto en = std::any_cast<TSys::Enum>(from);
        return std::make_any<int>(en.CurrentIndex());
    }
};


TSys::IntHandler::IntHandler(): TSys::GenericTypeHandler<int>()
{
    RegisterConstructibleConverter<bool>();
    RegisterConstructibleConverter<float>();
    RegisterConstructibleConverter<double>();
    RegisterConverter<std::string, StrToInt>();
    RegisterConverter<Enum, EnumToInt>();
    RegisterConverter<AnyValue, AnyConverter>();
}


std::string TSys::IntHandler::ApiName() const
{
    return "Int";
}


std::any TSys::IntHandler::InitValue() const
{
    return std::make_any<int>(0);
}


std::any TSys::IntHandler::CopyValue(const std::any& source) const
{
    int value = int(std::any_cast<int>(source));

    return std::make_any<int>(value);
}


std::string TSys::IntHandler::PythonName() const
{
    return "int";
}


std::string TSys::IntHandler::PythonModule() const
{
    return "builtins";
}


std::any TSys::IntHandler::FromPython(const boost::python::object& obj) const
{
    int value = boost::python::extract<int>(obj);
    return std::make_any<int>(value);
}


boost::python::object TSys::IntHandler::ToPython(const std::any& value) const
{
    return boost::python::object(std::any_cast<int>(value));
}


void TSys::IntHandler::SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                                      rapidjson::Document& doc) const
{
    rapidjson::Value stringValue(rapidjson::kStringType);

    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    jsonValue.PushBack(rapidjson::Value().SetString(
                               vname.c_str(), (rapidjson::SizeType)vname.size(), doc.GetAllocator()),
                       doc.GetAllocator());

    rapidjson::Value intValue;
    int saveValue = std::any_cast<int>(v);
    intValue.SetInt(saveValue);

    jsonValue.PushBack<int>(saveValue, doc.GetAllocator());
}


std::any TSys::IntHandler::DeserializeValue(const std::any&, rapidjson::Value& value) const
{
    int result = value.GetArray()[1].GetInt();
    return std::make_any<int>(result);
}


void TSys::IntHandler::SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                             rapidjson::Document& doc) const
{
    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    // jsonValue.PushBack(rapidjson::StringRef(vname.c_str()), doc.GetAllocator());
    value.PushBack(
            rapidjson::Value().SetString(
                    vname.c_str(), (rapidjson::SizeType)vname.size(),
                    doc.GetAllocator()
            ),
            doc.GetAllocator()
    );
}


std::any TSys::IntHandler::DeserializeConstruction(rapidjson::Value& value) const
{
    return InitValue();
}


// Float
struct StrToFloat: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        try
        {
            return std::make_any<float>(std::stof(std::any_cast<std::string>(from)));
        }
        catch (...)
        {
            return 0.0f;
        }
    }
};


struct EnumToFloat: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto en = std::any_cast<TSys::Enum>(from);
        return std::make_any<float>((float)en.CurrentIndex());
    }
};


TSys::FloatHandler::FloatHandler(): TSys::GenericTypeHandler<float>()
{
    RegisterConstructibleConverter<bool>();
    RegisterConstructibleConverter<int>();
    RegisterConstructibleConverter<double>();
    RegisterConverter<std::string, StrToFloat>();
    RegisterConverter<Enum, EnumToFloat>();
    RegisterConverter<AnyValue, AnyConverter>();
}


std::any TSys::FloatHandler::InitValue() const
{
    return std::make_any<float>(0.0f);
}


std::any TSys::FloatHandler::CopyValue(const std::any& source) const
{
    float value = float(std::any_cast<float>(source));

    return std::make_any<float>(value);
}


std::string TSys::FloatHandler::ApiName() const
{
    return "Float";
}


std::string TSys::FloatHandler::PythonName() const
{
    return "float";
}


std::string TSys::FloatHandler::PythonModule() const
{
    return "builtins";
}


std::any TSys::FloatHandler::FromPython(const boost::python::object& obj) const
{
    float value = boost::python::extract<float>(obj);
    return std::make_any<float>(value);
}


boost::python::object TSys::FloatHandler::ToPython(const std::any& value) const
{
    return boost::python::object(std::any_cast<float>(value));
}


void TSys::FloatHandler::SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                                        rapidjson::Document& doc) const
{
    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    jsonValue.PushBack(rapidjson::Value().SetString(
                               vname.c_str(), (rapidjson::SizeType)vname.size(), doc.GetAllocator()),
                       doc.GetAllocator());

    rapidjson::Value floatValue;
    float saveValue = std::any_cast<float>(v);
    floatValue.SetFloat(saveValue);

    jsonValue.PushBack<float>(saveValue, doc.GetAllocator());
}


std::any TSys::FloatHandler::DeserializeValue(const std::any&, rapidjson::Value& value) const
{
    float result = value.GetArray()[1].GetFloat();
    return std::make_any<float>(result);
}


void TSys::FloatHandler::SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                               rapidjson::Document& doc) const
{
    IntHandler().SerializeConstruction(v, value, doc);
}


std::any TSys::FloatHandler::DeserializeConstruction(rapidjson::Value& value) const
{
    return InitValue();
}


// Double
struct StrToDouble: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        try
        {
            return std::make_any<double>(std::stod(std::any_cast<std::string>(from)));
        }
        catch (...)
        {
            return 0.0;
        }
    }
};


struct EnumToDouble: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto en = std::any_cast<TSys::Enum>(from);
        return std::make_any<double>((double)en.CurrentIndex());
    }
};


TSys::DoubleHandler::DoubleHandler(): TSys::GenericTypeHandler<double>()
{
    RegisterConstructibleConverter<bool>();
    RegisterConstructibleConverter<int>();
    RegisterConstructibleConverter<float>();
    RegisterConverter<std::string, StrToDouble>();
    RegisterConverter<Enum, EnumToDouble>();
    RegisterConverter<AnyValue, AnyConverter>();
}


std::any TSys::DoubleHandler::InitValue() const
{
    return std::make_any<double>(0.0);
}


std::any TSys::DoubleHandler::CopyValue(const std::any& source) const
{
    auto value = double(std::any_cast<double>(source));

    return std::make_any<double>(value);
}


std::string TSys::DoubleHandler::ApiName() const
{
    return "Double";
}


std::string TSys::DoubleHandler::PythonName() const
{
    return "float";
}


std::string TSys::DoubleHandler::PythonModule() const
{
    return "builtins";
}


std::any TSys::DoubleHandler::FromPython(const boost::python::object& obj) const
{
    double value = boost::python::extract<double>(obj);
    return std::make_any<double>(value);
}


boost::python::object TSys::DoubleHandler::ToPython(const std::any& value) const
{
    return boost::python::object(std::any_cast<double>(value));
}


void TSys::DoubleHandler::SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                                         rapidjson::Document& doc) const
{
    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    jsonValue.PushBack(rapidjson::Value().SetString(
                               vname.c_str(), (rapidjson::SizeType)vname.size(), doc.GetAllocator()),
                       doc.GetAllocator());

    rapidjson::Value doubleValue;
    double saveValue = std::any_cast<double>(v);
    doubleValue.SetDouble(saveValue);

    jsonValue.PushBack<double>(saveValue, doc.GetAllocator());
}


std::any TSys::DoubleHandler::DeserializeValue(const std::any&, rapidjson::Value& value) const
{
    double result = value.GetArray()[1].GetDouble();
    return std::make_any<double>(result);
}


void TSys::DoubleHandler::SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                                rapidjson::Document& doc) const
{
    IntHandler().SerializeConstruction(v, value, doc);
}


std::any TSys::DoubleHandler::DeserializeConstruction(rapidjson::Value& value) const
{
    return IntHandler().InitValue();
}


// Enum
struct BoolToEnum: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto en = std::any_cast<TSys::Enum>(to);
        en.SetCurrentIndex((int)std::any_cast<bool>(from));
        return std::make_any<TSys::Enum>(en);
    }
};

struct IntToEnum: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto en = std::any_cast<TSys::Enum>(to);
        en.SetCurrentIndex(std::any_cast<int>(from));
        return std::make_any<TSys::Enum>(en);
    }
};

struct FloatToEnum: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto en = std::any_cast<TSys::Enum>(to);
        en.SetCurrentIndex((int)std::any_cast<float>(from));
        return std::make_any<TSys::Enum>(en);
    }
};

struct DoubleToEnum: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto en = std::any_cast<TSys::Enum>(to);
        en.SetCurrentIndex((int)std::any_cast<double>(from));
        return std::make_any<TSys::Enum>(en);
    }
};


struct StrToEnum: public TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto en = std::any_cast<TSys::Enum>(to);
        en.SetCurrentValue(std::any_cast<std::string>(from));
        return std::make_any<TSys::Enum>(en);
    }
};


TSys::EnumHandler::EnumHandler(): TSys::TypeHandler()
{
    RegisterConverter<bool, BoolToEnum>();
    RegisterConverter<int, IntToEnum>();
    RegisterConverter<float, FloatToEnum>();
    RegisterConverter<double, DoubleToEnum>();
    RegisterConverter<std::string, StrToEnum>();
    RegisterConverter<AnyValue, AnyConverter>();
}


std::any TSys::EnumHandler::InitValue() const
{
    return std::make_any<Enum>(Enum());
}


std::any TSys::EnumHandler::CopyValue(const std::any& source) const
{
    Enum value = Enum(std::any_cast<Enum>(source));

    return std::make_any<Enum>(value);
}


std::string TSys::EnumHandler::ApiName() const
{
    return "Enum";
}


std::string TSys::EnumHandler::PythonName() const
{
    return "Enum";
}


std::any TSys::EnumHandler::FromPython(const boost::python::object& obj) const
{
    auto value = boost::python::extract<Enum>(obj);
    return std::make_any<Enum>(value);
}


std::string TSys::EnumHandler::PythonModule() const
{
    return "builtins";
}


boost::python::object TSys::EnumHandler::ToPython(const std::any& value) const
{
    return boost::python::object(std::any_cast<Enum>(value));
}


void TSys::EnumHandler::SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                                       rapidjson::Document& doc) const
{
    rapidjson::Value stringValue(rapidjson::kStringType);

    std::string vname = TypeRegistry::GetRegistry()->GetTypeHandle(v)->ApiName();

    jsonValue.PushBack(rapidjson::Value().SetString(
                               vname.c_str(), (rapidjson::SizeType)vname.size(), doc.GetAllocator()),
                       doc.GetAllocator());

    rapidjson::Value intValue;
    Enum saveValue = std::any_cast<Enum>(v);
    intValue.SetInt(saveValue.CurrentIndex());

    jsonValue.PushBack(intValue, doc.GetAllocator());
}


std::any TSys::EnumHandler::DeserializeValue(const std::any& v, rapidjson::Value& value) const
{
    int result = value.GetArray()[1].GetInt();

    Enum e = std::any_cast<Enum>(v);

    e.SetCurrentIndex(result);
    return std::make_any<Enum>(e);
}


void TSys::EnumHandler::SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                              rapidjson::Document& doc) const
{
    IntHandler().SerializeConstruction(v, value, doc);

    Enum en = std::any_cast<Enum>(v);
    for (int i : en.Indices())
    {
        std::string st = en.ValueAtIndex(i);

        rapidjson::Value& index = rapidjson::Value().SetInt(i);
        rapidjson::Value& enumValue = rapidjson::Value().SetString(
                rapidjson::StringRef(st.c_str(), (rapidjson::SizeType)st.size()),
                doc.GetAllocator());
        value.PushBack(index, doc.GetAllocator());
        value.PushBack(enumValue, doc.GetAllocator());
    }
}


std::any TSys::EnumHandler::DeserializeConstruction(rapidjson::Value& value) const
{
    Enum result;

    rapidjson::Value& _array = value.GetArray();
    for (unsigned int i = 1; i < _array.Size(); i += 2)
    {
        rapidjson::Value& key = _array[i];
        rapidjson::Value& value_ = _array[i + 1];

        result.AddValue(key.GetInt(), value_.GetString());
    }

    return std::make_any<Enum>(result);
}


size_t TSys::EnumHandler::ValueHash(const std::any& value) const
{
    std::string str;
    Enum en = std::any_cast<Enum>(value);

    for (int i : en.Indices())
    {
        if (!str.empty())
            str += ":";

        str += en.ValueAtIndex(i);
    }

    str += ":" + std::to_string(en.CurrentIndex());
    return std::hash<std::string>{}(str);
}


bool TSys::EnumHandler::CompareValue(
        const std::any& v1,
        const std::any& v2)
        const
{
    Enum ev1;
    Enum ev2;
    size_t hash = Hash();
    if (v1.type().hash_code() != hash ||
        v2.type().hash_code() != hash)
    {
        return false;
    }

    return (std::any_cast<Enum>(v1) ==
            std::any_cast<Enum>(v2));
}



template<typename From>
struct ToAny: TSys::TypeConverter
{
    std::any Convert(std::any from, std::any to) const override
    {
        auto val = std::any_cast<TSys::AnyValue>(to);
        val.SetInput(from);

        return std::make_any<TSys::AnyValue>(val);
    }
};


// Any
TSys::AnyHandler::AnyHandler(): TSys::TypeHandler()
{
    RegisterConverter<int, ToAny<int>>();
    RegisterConverter<float, ToAny<float>>();
    RegisterConverter<double, ToAny<double>>();
    RegisterConverter<std::string, ToAny<std::string>>();
    RegisterConverter<Enum, ToAny<Enum>>();
}


std::any TSys::AnyHandler::InitValue() const
{
    return std::make_any<AnyValue>(AnyValue());
}


std::any TSys::AnyHandler::CopyValue(const std::any& source) const
{
    AnyValue value = std::any_cast<AnyValue>(source);

    return std::make_any<AnyValue>(AnyValue(value));
}


std::string TSys::AnyHandler::ApiName() const
{
    return "Any";
}


size_t TSys::AnyHandler::Hash() const
{
    return typeid(AnyValue).hash_code();
}


std::string TSys::AnyHandler::PythonName() const
{
    return "AnyValue";
}


std::any TSys::AnyHandler::FromPython(const boost::python::object& obj) const
{
    AnyValue v = boost::python::extract<AnyValue>(obj);
    return std::make_any<AnyValue>(v);
}


boost::python::object TSys::AnyHandler::ToPython(const std::any& value) const
{
    return boost::python::object(std::any_cast<AnyValue>(value));
}


void TSys::AnyHandler::SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                                      rapidjson::Document& doc) const
{
    AnyValue value = std::any_cast<AnyValue>(v);
    size_t hash = value.Hash();

    rapidjson::Value& inValue = rapidjson::Value().SetArray();

    auto handler = TypeRegistry::GetRegistry()->GetTypeHandle(hash);
    if(!handler)
    {
        return;
    }

    handler->SerializeValue(value.InputValue(), inValue, doc);

    jsonValue.PushBack(rapidjson::StringRef(value.Name().c_str()), doc.GetAllocator());

    jsonValue.PushBack(inValue, doc.GetAllocator());
}


std::any TSys::AnyHandler::DeserializeValue(const std::any& v, rapidjson::Value& value) const
{
    AnyValue val = std::any_cast<AnyValue>(v);

    // If no value was saved.
    if (!value.Size())
    {
        return std::make_any<AnyValue>(val);
    }

    rapidjson::Value& name = value[0];

    rapidjson::Value& typeValue = value[1];

    auto handle = TypeRegistry::GetRegistry()->GetTypeHandle(v);
    if (!handle)
    {
        return InitValue();
    }

    val.SetInput(handle->DeserializeValue(handle->InitValue(), typeValue));
    return std::make_any<AnyValue>(val);
}


void TSys::AnyHandler::SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                             rapidjson::Document& doc) const
{

}


std::any TSys::AnyHandler::DeserializeConstruction(rapidjson::Value& value) const
{
    return std::make_any<AnyValue>(AnyValue());
}


size_t TSys::AnyHandler::ValueHash(const std::any& val) const
{
    auto anyval = std::any_cast<AnyValue>(val);
    auto handler = TypeRegistry::GetRegistry()->GetTypeHandle(anyval.Hash());
    if (!handler)
    {
        return 0;
    }

    return handler->ValueHash(anyval.InputValue());
}


bool TSys::AnyHandler::CompareValue(const std::any& v1, const std::any& v2) const
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


bool TSys::None::operator==(const None &other) const
{
    return true;
}


std::any TSys::NoneHandler::InitValue() const
{
    return std::make_any<None>(None());
}


std::any TSys::NoneHandler::CopyValue(const std::any& source) const
{

    return InitValue();
}


std::string TSys::NoneHandler::ApiName() const
{
    return "None";
}


std::string TSys::NoneHandler::PythonName() const
{
    return "None";
}


std::any TSys::NoneHandler::FromPython(const boost::python::object& obj) const
{
    return InitValue();
}


boost::python::object TSys::NoneHandler::ToPython(const std::any& value) const
{
    return {};
}

void TSys::NoneHandler::SerializeValue(const std::any& v, rapidjson::Value& jsonValue,
                                       rapidjson::Document& doc) const
{

}


std::any TSys::NoneHandler::DeserializeValue(const std::any& v, rapidjson::Value& value) const
{
    return InitValue();
}


void TSys::NoneHandler::SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                              rapidjson::Document& doc) const
{

}


std::any TSys::NoneHandler::DeserializeConstruction(rapidjson::Value& value) const
{
    return InitValue();
}


size_t TSys::NoneHandler::ValueHash(const std::any& val) const
{
    return 0;
}