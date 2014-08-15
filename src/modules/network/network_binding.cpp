/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#include <tideutils/url_utils.h>
#include <tideutils/platform_utils.h>
#include <tide/tide.h>
#include <sstream>

#include "network_binding.h"
#include "protocols/tcp/tcp_socket.h"
#include "protocols/tcp/tcp_server_socket_binding.h"
#include "interface_binding.h"
#include "ipaddress_binding.h"
#include "host_binding.h"
#include "protocols/irc/irc_client_binding.h"
#include "protocols/http/http_client_binding.h"
#include "protocols/http/http_server_binding.h"

#include <tide/net/proxy_config.h>

#include <Poco/Mutex.h>
#include <Poco/Net/NetworkInterface.h>
using Poco::Net::NetworkInterface;

namespace ti
{
    static TiListRef interfaceList(0);
    static std::string firstIPv4Address = "127.0.0.1";

    static void GetInterfaceList()
    {
        if (!interfaceList.isNull())
            return;

        interfaceList = new StaticBoundList();
        std::vector<NetworkInterface> list = NetworkInterface::list();
        for (size_t i = 0; i < list.size(); i++)
        {
            NetworkInterface& interface = list[i];
            interfaceList->Append(Value::NewObject(new InterfaceBinding(interface)));

            if (!interface.address().isLoopback() &&
                interface.address().isIPv4Compatible())
                firstIPv4Address = interface.address().toString();
        }
    }

    NetworkBinding::NetworkBinding(Host* host) :
        AccessorObject("Network"),
        host(host),
        global(host->GetGlobalObject())
    {
        GetInterfaceList();

        // methods that are available on Ti.Network
        /**
         * @tiapi(method=True,name=Network.createTCPSocket,since=0.2) Creates a TCPSocket object
         * @tiarg(for=Network.createTCPSocket,name=host,type=String) the hostname to connect to
         * @tiarg(for=Network.createTCPSocket,name=port,type=Number) the port to use
         * @tiresult(for=Network.createTCPSocket,type=Network.TCPSocket) a TCPSocket object
         */
        this->SetMethod("createTCPSocket",&NetworkBinding::_CreateTCPSocket);
        /**
         * @tiapi(method=True,name=Network.createTCPServerSocket,since=1.2) Creates a TCPServerSocket object
         * @tiarg(for=Network.createTCPServerSocket,name=callback,type=Function) the callback to receive a new connection
         * @tiresult(for=Network.createTCPServerSocket,type=Network.TCPServerSocket) a TCPServerSocket object
         */
        this->SetMethod("createTCPServerSocket",&NetworkBinding::_CreateTCPServerSocket);
        /**
         * @tiapi(method=True,name=Network.createIRCClient,since=0.2) Creates an IRCClient object
         * @tiresult(for=Network.createIRCClient,type=Network.IRCClient) an IRCClient object
         */
        this->SetMethod("createIRCClient",&NetworkBinding::_CreateIRCClient);
        /**
         * @tiapi(method=True,name=Network.createIPAddress,since=0.2) Creates an IPAddress object
         * @tiarg(for=Network.createIPAddress,name=address,type=String) the IP address
         * @tiresult(for=Network.createIPAddress,type=Network.IPAddress) an IPAddress object
         */
        this->SetMethod("createIPAddress",&NetworkBinding::_CreateIPAddress);
        /**
         * @tiapi(method=True,name=Network.createHTTPClient,since=0.3) Creates an HTTPClient object
         * @tiresult(for=Network.createHTTPClient,type=Network.HTTPClient) an HTTPClient object
         */
        this->SetMethod("createHTTPClient",&NetworkBinding::_CreateHTTPClient);
        /**
         * @tiapi(method=True,name=Network.createHTTPServer,since=0.4) Creates an HTTPServer object
         * @tiresult(for=Network.createHTTPServer,type=Network.HTTPServer) a HTTPServer object
         */
        this->SetMethod("createHTTPServer",&NetworkBinding::_CreateHTTPServer);
        /**
         * @tiapi(method=True,name=Network.createHTTPCookie,since=0.7) Creates a new HTTPCookie object
         * @tiresult(for=Network.createHTTPCookie,type=Network.HTTPCookie) a HTTPCookie object
         */
        this->SetMethod("createHTTPCookie",&NetworkBinding::_CreateHTTPCookie);
        /**
         * @tiapi(method=True,name=Network.getHostByName,since=0.2) Returns a Host object using a hostname
         * @tiarg(for=Network.getHostByName,name=name,type=String) the hostname
         * @tiresult(for=Network.getHostByName,type=Network.Host) a Host object referencing the hostname
         */
        this->SetMethod("getHostByName",&NetworkBinding::_GetHostByName);
        /**
         * @tiapi(method=True,name=Network.getHostByAddress,since=0.2) Returns a Host object using an address
         * @tiarg(for=Network.getHostByAddress,name=address,type=String) the address
         * @tiresult(for=Network.getHostByAddress,type=Network.Host) a Host object referencing the address
         */
        this->SetMethod("getHostByAddress",&NetworkBinding::_GetHostByAddress);
        /**
         * @tiapi(method=True,name=Network.encodeURIComponent,since=0.3) Encodes a URI Component
         * @tiarg(for=Network.encodeURIComponent,name=value,type=String) value to encode
         * @tiresult(for=Network.encodeURIComponent,type=String) the encoded value
         */
        this->SetMethod("encodeURIComponent",&NetworkBinding::_EncodeURIComponent);
        /**
         * @tiapi(method=True,name=Network.decodeURIComponent,since=0.3) Decodes a URI component
         * @tiarg(for=Network.decodeURIComponent,name=value,type=String) value to decode
         * @tiresult(for=Network.decodeURIComponent,type=String) the decoded value
         */
        this->SetMethod("decodeURIComponent",&NetworkBinding::_DecodeURIComponent);

        /**
         * @tiapi(method=True,name=Network.setHTTPProxy,since=0.7)
         * @tiapi Override application proxy autodetection with a proxy URL.
         * @tiarg[String, hostname] The full proxy hostname.
         */
        this->SetMethod("setHTTPProxy", &NetworkBinding::_SetHTTPProxy);
        this->SetMethod("setProxy", &NetworkBinding::_SetHTTPProxy);

        /**
         * @tiapi(method=True,name=Network.getHTTPProxy,since=0.7) 
         * @tiapi Return the proxy override, if one is set.
         * @tiresult[String|null] The full proxy override URL or null if none is set.
         */
        this->SetMethod("getHTTPProxy", &NetworkBinding::_GetHTTPProxy);
        this->SetMethod("getProxy", &NetworkBinding::_GetHTTPProxy);

        /**
         * @tiapi(method=True,name=Network.setHTTPSProxy,since=0.7)
         * @tiapi Override application proxy autodetection with a proxy URL.
         * @tiarg[String, hostname] The full proxy hostname.
         */
        this->SetMethod("setHTTPSProxy", &NetworkBinding::_SetHTTPSProxy);

        /**
         * @tiapi(method=True,name=Network.getHTTPSProxy,since=0.7)
         * @tiapi Return the proxy override, if one is set.
         * @tiresult[String|null] The full proxy override URL or null if none is set.
         */
        this->SetMethod("getHTTPSProxy", &NetworkBinding::_GetHTTPSProxy);

        /**
         * @tiapi(method=True,name=Network.getInterfaces,since=0.9)
         * Get a list of interfaces active on this machine.
         * @tiresult[Array<Netowrk.Interface>] An array of active interfaces.
         */
        this->SetMethod("getInterfaces", &NetworkBinding::_GetInterfaces);

        /**
         * @tiapi(method=True,name=Network.getFirstIPAddress,since=0.9)
         * Get the first IPv4 address in this machine's list of interfaces.
         * @tiarg[String, address] The first IPv4 address in this system's list of interfaces.
         */ 
        this->SetMethod("getFirstIPAddress", &NetworkBinding::_GetFirstIPAddress);
        this->SetMethod("getAddress", &NetworkBinding::_GetFirstIPAddress); // COMPATBILITY

        /**
         * @tiapi(method=True,name=Network.getFirstMACAddress,since=0.9)
         * Get the first MAC address in this system's list of interfaces.
         * @tiarg[String, adress] The first MAC address in this system's list of interfaces.
         */
        this->SetMethod("getFirstMACAddress", &NetworkBinding::_GetFirstMACAddress);
        this->SetMethod("getMACAddress", &NetworkBinding::_GetFirstMACAddress);
    }

    NetworkBinding::~NetworkBinding()
    {
    }

    AutoPtr<HostBinding> NetworkBinding::GetHostBinding(std::string hostname)
    {
        AutoPtr<HostBinding> binding(new HostBinding(hostname));
        if (binding->IsInvalid())
            throw ValueException::FromString("Could not resolve address");

        return binding;
    }

    void NetworkBinding::_GetHostByAddress(const ValueList& args, ValueRef result)
    {
        if (args.at(0)->IsObject())
        {
            TiObjectRef obj = args.at(0)->ToObject();
            AutoPtr<IPAddressBinding> b = obj.cast<IPAddressBinding>();
            if (!b.isNull())
            {
                // in this case, they've passed us an IPAddressBinding
                // object, which we can just retrieve the ipaddress
                // instance and resolving using it
                IPAddress addr(b->GetAddress()->toString());
                AutoPtr<HostBinding> binding = new HostBinding(addr);
                if (binding->IsInvalid())
                {
                    throw ValueException::FromString("Could not resolve address");
                }
                result->SetObject(binding);
                return;
            }
            else
            {
                TiMethodRef toStringMethod = obj->GetMethod("toString");
                if (toStringMethod.isNull())
                    throw ValueException::FromString("Unknown object passed");

                result->SetObject(GetHostBinding(toStringMethod->Call()->ToString()));
                return;
            }
        }
        else if (args.at(0)->IsString())
        {
            // in this case, they just passed in a string so resolve as normal
            result->SetObject(GetHostBinding(args.GetString(0)));
        }
    }

    void NetworkBinding::_GetHostByName(const ValueList& args, ValueRef result)
    {
        result->SetObject(GetHostBinding(args.GetString(0)));
    }

    void NetworkBinding::_CreateIPAddress(const ValueList& args, ValueRef result)
    {
        AutoPtr<IPAddressBinding> binding = new IPAddressBinding(args.at(0)->ToString());
        if (binding->IsInvalid())
        {
            throw ValueException::FromString("Invalid address");
        }
        result->SetObject(binding);
    }

    void NetworkBinding::_CreateTCPSocket(const ValueList& args, ValueRef result)
    {
        args.VerifyException("createTCPSocket", "sn");
        std::string host(args.GetString(0));
        int port = args.GetInt(1);
        result->SetObject(new TCPSocket(host, port));
    }

    void NetworkBinding::_CreateTCPServerSocket(const ValueList& args, ValueRef result)
    {
        args.VerifyException("createTCPServerSocket", "m");
        TiMethodRef target = args.at(0)->ToMethod();
        result->SetObject(new TCPServerSocketBinding(host,target));
    }

    void NetworkBinding::_CreateIRCClient(const ValueList& args, ValueRef result)
    {
        AutoPtr<IRCClientBinding> irc = new IRCClientBinding(host);
        result->SetObject(irc);
    }

    void NetworkBinding::_CreateHTTPClient(const ValueList& args, ValueRef result)
    {
        result->SetObject(new HTTPClientBinding(host));
    }

    void NetworkBinding::_CreateHTTPServer(const ValueList& args, ValueRef result)
    {
        result->SetObject(new HTTPServerBinding(host));
    }

    void NetworkBinding::_CreateHTTPCookie(const ValueList& args, ValueRef result)
    {
        result->SetObject(new HTTPCookie());
    }

    void NetworkBinding::_EncodeURIComponent(const ValueList &args, ValueRef result)
    {
        if (args.at(0)->IsNull() || args.at(0)->IsUndefined())
        {
            result->SetString("");
        }
        else if (args.at(0)->IsString())
        {
            std::string src = args.at(0)->ToString();
            std::string sResult = URLUtils::EncodeURIComponent(src);
            result->SetString(sResult);
        }
        else if (args.at(0)->IsDouble())
        {
            std::stringstream str;
            str << args.at(0)->ToDouble();
            result->SetString(str.str().c_str());
        }
        else if (args.at(0)->IsBool())
        {
            std::stringstream str;
            str << args.at(0)->ToBool();
            result->SetString(str.str().c_str());
        }
        else if (args.at(0)->IsInt())
        {
            std::stringstream str;
            str << args.at(0)->ToInt();
            result->SetString(str.str().c_str());
        }
        else
        {
            throw ValueException::FromString("Could not encodeURIComponent with type passed");
        }
    }

    void NetworkBinding::_DecodeURIComponent(const ValueList &args, ValueRef result)
    {
        if (args.at(0)->IsNull() || args.at(0)->IsUndefined())
        {
            result->SetString("");
        }
        else if (args.at(0)->IsString())
        {
            std::string src = args.at(0)->ToString();
            std::string sResult = URLUtils::DecodeURIComponent(src);
            result->SetString(sResult);
        }
        else
        {
            throw ValueException::FromString("Could not decodeURIComponent with type passed");
        }
    }

    static SharedProxy ArgumentsToProxy(const ValueList& args, const std::string& scheme)
    {
        if (args.at(0)->IsNull())
            return 0;

        std::string entry(args.GetString(0));
        if (entry.empty())
            return 0;

        // Do not pass the third argument entryScheme, because it overrides
        // any scheme set in the proxy string.
        return ProxyConfig::ParseProxyEntry(entry, scheme, std::string());
    }

    void NetworkBinding::_SetHTTPProxy(const ValueList& args, ValueRef result)
    {
        args.VerifyException("setHTTPProxy", "s|0 ?s s s");
        SharedProxy proxy(ArgumentsToProxy(args, "http"));
        ProxyConfig::SetHTTPProxyOverride(proxy);
    }

    void NetworkBinding::_GetHTTPProxy(const ValueList& args, ValueRef result)
    {
        SharedProxy proxy = ProxyConfig::GetHTTPProxyOverride();

        if (proxy.isNull())
            result->SetNull();
        else
            result->SetString(proxy->ToString().c_str());
    }

    void NetworkBinding::_SetHTTPSProxy(const ValueList& args, ValueRef result)
    {
        args.VerifyException("setHTTPSProxy", "s|0 ?s s s");
        SharedProxy proxy(ArgumentsToProxy(args, "https"));
        ProxyConfig::SetHTTPSProxyOverride(proxy);
    }

    void NetworkBinding::_GetHTTPSProxy(const ValueList& args, ValueRef result)
    {
        SharedProxy proxy = ProxyConfig::GetHTTPSProxyOverride();

        if (proxy.isNull())
            result->SetNull();
        else
            result->SetString(proxy->ToString().c_str());
    }

    Host* NetworkBinding::GetHost()
    {
        return this->host;
    }

    void NetworkBinding::_GetFirstMACAddress(const ValueList& args, ValueRef result)
    {
        result->SetString(PlatformUtils::GetFirstMACAddress().c_str());
    }

    void NetworkBinding::_GetFirstIPAddress(const ValueList& args, ValueRef result)
    {
        static std::string address(NetworkBinding::GetFirstIPAddress());
        result->SetString(address.c_str());
    }

    /*static*/
    const std::string& NetworkBinding::GetFirstIPAddress()
    {
        return firstIPv4Address;
    }

    void NetworkBinding::_GetInterfaces(const ValueList& args, ValueRef result)
    {
        result->SetList(interfaceList);
    }
}
