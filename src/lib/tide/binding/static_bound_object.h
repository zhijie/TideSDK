/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef _STATIC_BOUND_OBJECT_H_
#define _STATIC_BOUND_OBJECT_H_

#include <vector>
#include <string>
#include <map>

#include <Poco/Mutex.h>

namespace tide
{
    /**
     * Extending this class is the easiest way to get started with your own
     * TiObject implementation. In your sub-class' constructor, you can bind
     * properties and methods, i.e:
     * \code
     * MyObject::MyObject() {
     *   this->Set("x", Value::NewInt(100));
     *   this->Set("description", Value::NewString("my object"));
     *   this->SetMethod("add", &MyObject::Add);
     * }
     *
     * void MyObject::Add(const ValueList& args, ValueRef result) {
     *   result->SetInt(args[0]->ToInt() + args[1]->ToInt());
     * }
     * \endcode
     *
     * And a supported language would access your object ala:
     * \code
     * var myObject = //..
     * alert(myObject.x); // 100
     * alert(myObject.description); // "my object"
     * alert(myObject.add(10, 15)); // 25
     * \endcode
     */
    class TIDE_API StaticBoundObject : public TiObject
    {
    public:
        StaticBoundObject(const char* type = "StaticBoundObject");
        virtual ~StaticBoundObject();

        virtual bool HasProperty(const char* name);
        virtual ValueRef Get(const char* name);
        virtual SharedStringList GetPropertyNames();
        virtual void Set(const char* name, ValueRef value);
        virtual void Unset(const char* name);

        /**
         * Set a property on this object to the given method. When an error
         * occurs will throw an exception of type ValueException.
         */
        template <typename T>
        void SetMethod(const char* name, void (T::*method)(const ValueList&, ValueRef))
        {
            this->Set(name, Value::NewMethod(new StaticBoundMethod(
                NewCallback<T, const ValueList&, ValueRef>(static_cast<T*>(this), method))));
        }


    protected:
        std::map<std::string, ValueRef> properties;
        Poco::Mutex mutex;

    private:
        DISALLOW_EVIL_CONSTRUCTORS(StaticBoundObject);
    };

}

#endif
