// V8 Helpers: some useful classes and functions for messing up V8
// Author(s):       iFarbod <ifarbod@outlook.com>
//
// Copyright (c) 2015-2016 The San Andreas Online Open Source Project
//
// Distributed under the MIT license (See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <cef3/include/cef_app.h>

using JavascriptCallback = void(*)(CefRefPtr<CefFrame> frame, const CefV8ValueList& arguments);

namespace V8Helpers
{
class V8Handler : public CefV8Handler
{
public:
    V8Handler(CefRefPtr<CefFrame> frame) : frame_(frame)
    {
    }
    void Bind(const std::string& name, JavascriptCallback callback)
    {
        functionMap_[name] = callback;
    }
    virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
    {
        std::map<std::string, JavascriptCallback>::iterator iter = functionMap_.find(std::string(name));
        if (iter != functionMap_.end())
        {
            iter->second(frame_, arguments);
            return true;
        }
        return false;
    }
    void Clear()
    {
        functionMap_.clear();
    }

    IMPLEMENT_REFCOUNTING(V8Handler);

private:
    CefRefPtr<CefFrame> frame_;
    std::map<std::string, JavascriptCallback> functionMap_;
};

std::stringstream conversionStream;
void ConvertV8ArgToString(std::string& outResult, CefRefPtr<CefV8Value> cefValue)
{
    // Prepare stringstream for next argument (clear and reset states)
    conversionStream.str("");
    conversionStream.clear();

    if (cefValue->IsBool())
        conversionStream << cefValue->GetBoolValue();
    else if (cefValue->IsDouble())
        conversionStream << cefValue->GetDoubleValue();
    else if (cefValue->IsInt())
        conversionStream << cefValue->GetIntValue();
    else if (cefValue->IsNull())
        conversionStream << "nil";
    else if (cefValue->IsString())
        conversionStream << std::string(cefValue->GetStringValue());
    else if (cefValue->IsUInt())
        conversionStream << cefValue->GetUIntValue();
    else
        conversionStream << "unsupported type";

    outResult = conversionStream.str();
}

CefRefPtr<CefProcessMessage> SerializeV8Arguments(const std::string& messageName, const CefV8ValueList& arguments)
{
    // Create the process message
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(messageName);
    CefRefPtr<CefListValue> argList = message->GetArgumentList();

    // Write the event name
    argList->SetString(0, arguments[0]->GetStringValue());

    // Write number of arguments
    argList->SetInt(1, arguments.size() - 1);

    // Write arguments
    for (size_t i = 1; i < arguments.size(); ++i)
    {
        std::string strArgument;
        V8Helpers::ConvertV8ArgToString(strArgument, arguments[i]);
        argList->SetString(i + 1, strArgument);
    }

    return message;
}

void BindV8Function(CefRefPtr<V8Handler> handler, CefRefPtr<CefV8Value> object, const std::string& name, JavascriptCallback cb)
{
    handler->Bind(name, cb);
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(name, handler);
    object->SetValue(name, func, V8_PROPERTY_ATTRIBUTE_NONE);
}

}