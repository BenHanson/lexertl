#include "../../lexertl/memory_file.hpp"
#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"

struct entry
{
    std::string _disp_name;
    bool _properties;
    std::string _id;
    std::string _type;
    std::string _name;
    std::vector<std::pair<std::string, std::string> > _params;

    entry() :
        _properties(false)
    {
    }

    void clear()
    {
        _id.clear();
        _type.clear();
        _name.clear();
        _params.clear();
    }
};

struct type_conv
{
    const char *_in;
    const char *_out;
};

enum {eDispName = 1, eProperties, eMethods, eID, eType, eName, eEntry,
    eParamType, eParamName};

std::string out_var_type(const std::string &in_)
{
    static type_conv list_[] =
        {{"short", "VTS_I2"},
        {"long", "VTS_I4"},
        {"float", "VTS_R4"},
        {"double", "VTS_R8"},
        {"CY", "VTS_CY"},
        {"CY *", "VTS_CY"},
        {"DATE", "VTS_DATE"},
        {"LPCOLESTR", "VTS_WBSTR"},
        {"IDispatch *", "VTS_DISPATCH"},
        {"SCODE", "VTS_SCODE"},
        {"VARIANT_BOOL", "VTS_BOOL"},
        {"const VARIANT&", "VTS_VARIANT"},
        {"VARIANT *", "VTS_VARIANT"},
        {"IUnknown *", "VTS_UNKNOWN"},
        {"LPCSTR", "VTS_BSTR"},
        {"BSTR", "VTS_BSTR"},
        {"signed char", "VTS_I1"},
        {"BYTE", "VTS_UI1"},
        {"WORD", "VTS_UI2"},
        {"DWORD", "VTS_UI4"},
        {"LONGLONG", "VTS_I8"},
        {"ULONGLONG", "VTS_UI8"},
        {"short *", "VTS_PI2"},
        {"long *", "VTS_PI4"},
        {"float *", "VTS_PR4"},
        {"double *", "VTS_PR8"},
        {"CY *", "VTS_PCY"},
        {"DATE *", "VTS_PDATE"},
        {"BSTR *", "VTS_PBSTR"},
        {"IDispatch **", "VTS_PDISPATCH"},
        {"SCODE *", "VTS_PSCODE"},
        {"VARIANT_BOOL *", "VTS_PBOOL"},
        {"VARIANT *", "VTS_PVARIANT"},
        {"IUnknown **", "VTS_PUNKNOWN"},
        {"signed char *", "VTS_PI1"},
        {"BYTE *", "VTS_PUI1"},
        {"WORD *", "VTS_PUI2"},
        {"DWORD *", "VTS_PUI4"},
        {"LONGLONG *", "VTS_PI8"},
        {"ULONGLONG *", "VTS_PUI8"},
        {"unsigned int", "VTS_UI4"},
        {"NULL", "VTS_NONE"}, {0, 0}};
    std::string out_;
    const type_conv *ptr_ = list_;

    while (ptr_->_in != 0 && in_ != ptr_->_in)
    {
        ++ptr_;
    }

    if (ptr_->_out)
    {
        out_ = ptr_->_out;
    }
    else
    {
        out_ = "UNKNOWN TYPE";
    }

    return out_;
}

std::string in_var_type(const std::string &in_)
{
    static type_conv list_[] =
        {{"void", "VT_EMPTY"},
        {"NULL", "VT_NONE"},
        {"short", "VT_I2"},
        {"long", "VT_I4"},
        {"float", "VT_R4"},
        {"double", "VT_R8"},
        {"CY", "VT_CY"},
        {"DATE", "VT_DATE"},
        {"BSTR", "VT_BSTR"},
        {"IDispatch *", "VT_DISPATCH"},
        {"SCODE", "VT_ERROR"},
        {"VARIANT_BOOL", "VT_BOOL"},
        {"VARIANT *", "VT_VARIANT"},
        {"IUnknown *", "VT_UNKNOWN"},
        {"char", "VT_I1"},
        {"signed char", "VT_I1"},
        {"unsigned char", "VT_UI1"},
        {"unsigned short", "VT_UI2"},
        {"unsigned long", "VT_UI4"},
        {"__int64", "VT_I8"},
        {"unsigned __int64", "VT_UI8"},
        {"int", "VT_I4"},
        {"unsigned int", "VT_UI4"},
        {"int *", "VT_INT_PTR"},
        {"unsigned int *", "VT_UINT_PTR"}, {0, 0}};
    std::string out_;
    const type_conv *ptr_ = list_;

    while (ptr_->_in != 0 && in_ != ptr_->_in)
    {
        ++ptr_;
    }

    if (ptr_->_out)
    {
        out_ = ptr_->_out;
    }
    else
    {
        out_ = "UNKNOWN TYPE";
    }

    return out_;
}

std::string out_type(const std::string &in_)
{
    std::string out_;

    if (in_ == "BSTR")
    {
        out_ = "CString";
    }
    else if (in_ == "VARIANT_BOOL")
    {
        out_ = "BOOL";
    }
    else
    {
        out_ = in_;
    }

    return out_;
}

std::string in_type(const std::string &in_)
{
    std::string out_;

    if (in_ == "BSTR")
    {
        out_ = "LPCTSTR";
    }
    else if (in_ == "VARIANT_BOOL")
    {
        out_ = "BOOL";
    }
    else if (in_ == "single")
    {
        out_ = "float";
    }
    else
    {
        out_ = in_;
    }

    return out_;
}

void operations(const entry &entry_, std::string &header_, std::string &source_)
{
    std::stringstream ss_;
    long val_ = -1;
    std::string hex_val_;

    header_ += '\t';
    header_ += out_type(entry_._type);
    source_ += out_type(entry_._type);
    header_ += ' ' + entry_._name + '(';
    source_ += ' ' + entry_._disp_name + "::" + entry_._name + '(';

    for (std::size_t i_ = 0, size_ = entry_._params.size(); i_ < size_; ++i_)
    {
        if (i_ > 0)
        {
            header_ += ", ";
            source_ += ", ";
        }

        header_ += in_type(entry_._params[i_].first) + ' ' +
            entry_._params[i_].second;
        source_ += in_type(entry_._params[i_].first) + ' ' +
            entry_._params[i_].second;
    }

    header_ += ");\r\n";
    source_ += ")\r\n{";

    if (entry_._type != "void")
    {
        source_ += "\r\n\t" + out_type(entry_._type) + " result;";
    }

    if (!entry_._params.empty())
    {
        source_ += "\r\n\tstatic BYTE parms[] = ";

        for (std::size_t i_ = 0, size_ = entry_._params.size(); i_ < size_; ++i_)
        {
            source_ += out_var_type(entry_._params[i_].first) + ' ';
        }

        source_ += ';';
    }

    source_ += "\r\n\tInvokeHelper(0x";

    if (entry_._id[2] == '0')
    {
        ss_ << entry_._id;
        ss_ >> std::hex >> val_;
        ss_.clear();
        ss_ << std::hex << val_;
        ss_ >> hex_val_;
    }
    else
    {
        hex_val_ = entry_._id.substr(2);
    }

    source_ += hex_val_;
    source_ += ", DISPATCH_METHOD, ";
    source_ += in_var_type(entry_._type) + ", ";

    if (entry_._type == "void")
    {
        source_ += "NULL";
    }
    else
    {
        source_ += "(void*)&result";
    }

    source_ += ", ";

    if (entry_._params.empty())
    {
        source_ += "NULL";
    }
    else
    {
        source_ += "parms";
    }

    for (std::size_t i_ = 0, size_ = entry_._params.size(); i_ < size_; ++i_)
    {
        source_ += ", " + entry_._params[i_].second;
    }

    source_ += ");\r\n";

    if (entry_._type != "void")
    {
        source_ += "\treturn result;\r\n";
    }

    source_ += "}\r\n\r\n";
}

void properties(const entry &entry_, std::string &header_, std::string &source_)
{
    std::stringstream ss_;
    long val_ = -1;
    std::string hex_val_;

    header_ += '\t';
    header_ += out_type(entry_._type);
    source_ += out_type(entry_._type);

    source_ += ' ' + entry_._disp_name + "::";
    header_ += " Get";
    source_ += "Get";
    header_ += entry_._name;
    source_ += entry_._name;
    header_ += "();\r\n";
    source_ += "()\r\n{\r\n\t";
    source_ += out_type(entry_._type);
    source_ += " result;\r\n\tGetProperty(0x";

    if (entry_._id[2] == '0')
    {
        ss_ << entry_._id;
        ss_ >> std::hex >> val_;
        ss_.clear();
        ss_ << std::hex << val_;
        ss_ >> hex_val_;
    }
    else
    {
        hex_val_ = entry_._id.substr(2);
    }

    source_ += hex_val_;
    source_ += ", ";
    source_ += in_var_type(entry_._type);
    source_ += ", (void*)&result);\r\n\treturn result;\r\n}\r\n\r\n";

    header_ += "\tvoid Set";
    source_ += "void " + entry_._disp_name + "::Set";
    header_ += entry_._name;
    source_ += entry_._name;
    header_ += '(';
    source_ += '(';

    header_ += in_type(entry_._type);
    source_ += in_type(entry_._type);

    header_ += ");\r\n";
    source_ += " propVal)\r\n{\r\n\tSetProperty(0x";
    source_ += hex_val_;
    source_ += ", ";
    source_ += in_var_type(entry_._type);
    source_ += ", propVal);\r\n}\r\n\r\n";
}

void process(const entry &entry_, std::string &header_, std::string &source_)
{
    if (entry_._properties)
    {
        properties(entry_, header_, source_);
    }
    else
    {
//        operations(entry_, header_, source_);
    }
}

void build_sm(lexertl::state_machine &sm_)
{
    lexertl::rules rules_;

    rules_.push_state("DISPNAME");
    rules_.push_state("BLOCK");
    rules_.push_state("PROP_METHODS");

    rules_.push_state("PROP_START_ID");
    rules_.push_state("PROP_ID");
    rules_.push_state("PROP_END_ID");
    rules_.push_state("PROP_TYPE");
    rules_.push_state("PROP_NAME");
    rules_.push_state("PROP_PAREN");

    rules_.push_state("METHOD_START_ID");
    rules_.push_state("METHOD_ID");
    rules_.push_state("METHOD_END_ID");
    rules_.push_state("METHOD_TYPE");
    rules_.push_state("METHOD_NAME");
    rules_.push_state("METHOD_PARAMS");

    rules_.push_state("PARAM_TYPE");
    rules_.push_state("PARAM_NAME");
    rules_.push_state("END_PARAM_NAME");

    rules_.push("INITIAL", "dispinterface", sm_.skip(), "DISPNAME");
    rules_.push("DISPNAME", "[_a-zA-Z]+", eDispName, "BLOCK");
    rules_.push("BLOCK", ";", sm_.skip(), "INITIAL");
    rules_.push("BLOCK", "\\{", sm_.skip(), "PROP_METHODS");
    rules_.push("PROP_METHODS,METHOD_START_ID", "properties:", eProperties, "PROP_START_ID");
    rules_.push("PROP_METHODS,PROP_START_ID", "methods:", eMethods, "METHOD_START_ID");

    rules_.push("PROP_START_ID", "\\[id\\(", sm_.skip(), "PROP_ID");
    rules_.push("PROP_ID", "[^)]+", eID, "PROP_END_ID");
    rules_.push("PROP_END_ID", "\\)[^\\]]*]", sm_.skip(), "PROP_TYPE");
    rules_.push("PROP_TYPE", "(unsigned[ \t]+)?[_a-zA-Z]+", eType, "PROP_NAME");
    rules_.push("PROP_NAME", "[a-zA-Z0-9]+", eName, "PROP_PAREN");
    rules_.push("PROP_PAREN", ";", eEntry, "PROP_START_ID");

    rules_.push("METHOD_START_ID", "\\[id\\(", sm_.skip(), "METHOD_ID");
    rules_.push("METHOD_ID", "[^)]+", eID, "METHOD_END_ID");
    rules_.push("METHOD_END_ID", "\\)[^\\]]*]", sm_.skip(), "METHOD_TYPE");
    rules_.push("METHOD_TYPE", "(unsigned[ \t]+)?[_a-zA-Z]+", eType, "METHOD_NAME");
    rules_.push("METHOD_NAME", "[a-zA-Z0-9]+", eName, "METHOD_PARAMS");
    rules_.push("METHOD_PARAMS", "\\(", sm_.skip(), "PARAM_TYPE");

    rules_.push("PARAM_TYPE", "(unsigned[ \t]+)?[_a-zA-Z]+\\**", eParamType, "PARAM_NAME");
    rules_.push("PARAM_NAME", "[_a-zA-Z]+", eParamName, "END_PARAM_NAME");
    rules_.push("END_PARAM_NAME", ",", sm_.skip(), "PARAM_TYPE");
    rules_.push("PARAM_TYPE,END_PARAM_NAME", "\\);", eEntry, "METHOD_START_ID");

    rules_.push("*", "\\s+", sm_.skip(), ".");
    lexertl::generator::build(rules_, sm_);
}

void lookup(const lexertl::state_machine &sm_, lexertl::cmatch &results_,
    std::string &header_, std::string &source_)
{
    entry entry_;

    do
    {
        lexertl::lookup(sm_, results_);

        switch (results_.id)
        {
        case eDispName:
            entry_._disp_name = results_.str();
            break;
        case eProperties:
            entry_._properties = true;
            header_ += "// Properties\r\n";
            source_ += "// Properties\r\n";
            break;
        case eMethods:
            entry_._properties = false;
            header_ += "// Operations\r\n";
            source_ += "// Operations\r\n";
            break;
        case eID:
            entry_.clear();
            entry_._id = results_.str();
            break;
        case eType:
            entry_._type = results_.str();
            break;
        case eName:
            entry_._name = results_.str();
            break;
        case eParamType:
        {
            std::string type_;
            std::size_t star_index_ = std::string::npos;
            std::pair<std::string, std::string> pair_;

            type_ = results_.str();
            star_index_ = type_.find('*');

            if (star_index_ != std::string::npos)
            {
                type_.insert(type_.begin() + star_index_, ' ');
            }

            pair_.first = type_;
            entry_._params.push_back(pair_);
            break;
        }
        case eParamName:
            entry_._params.back().second = results_.str();
            break;
        case eEntry:
            process(entry_, header_, source_);
            break;
        }
    } while (results_.id != sm_.eoi());
}

int main(int /*argc*/, char** /*argv[]*/)
{
    lexertl::state_machine sm_;
    lexertl::memory_file if_("Corebridge.txt");
    const char *start_ = if_.data();
    const char *end_ = start_ + if_.size();
    lexertl::cmatch results_(start_, end_);
    std::string header_;
    std::string source_;

    build_sm(sm_);
    lookup(sm_, results_, header_, source_);
}
