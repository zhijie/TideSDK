/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

describe("Platform Tests", {
  test_interfaces: function () {
    var interfaces = Ti.Platform.interfaces;
    value_of(Ti.Platform.interfaces)
      .should_be_array();
    value_of(Ti.Platform.interfaces.length > 0)
      .should_be_true();

    for (var i = 0; i < Ti.Platform.interfaces.length; i++) {
      var iface = Ti.Platform.interfaces[i];
      value_of(iface.name)
        .should_be_string();
      value_of(iface.address)
        .should_be_string();
      value_of(iface.displayName)
        .should_be_string();

      Ti.API.debug("interface " + iface.name + ", " + iface.address + ", " + iface.displayName);
    }
  },

  test_platform_properties: function () {
    value_of(Ti.Platform.name)
      .should_be_string();
    value_of(Ti.Platform.ostype)
      .should_be_string();
    value_of(Ti.Platform.version)
      .should_be_string();
    value_of(Ti.Platform.architecture)
      .should_be_string();
    value_of(Ti.Platform.address)
      .should_be_string();
    value_of(Ti.Platform.id)
      .should_be_string();
    value_of(Ti.Platform.macaddress)
      .should_be_string();
    value_of(Ti.Platform.processorCount)
      .should_be_number();
    value_of(Ti.Platform.username)
      .should_be_string();
    value_of(Ti.Platform.createUUID)
      .should_be_function();

    Ti.API.debug("Ti.Platform.name " + Ti.Platform.name);
    Ti.API.debug("Ti.Platform.ostype " + Ti.Platform.ostype);
    Ti.API.debug("Ti.Platform.version " + Ti.Platform.version);
    Ti.API.debug("Ti.Platform.architecture " + Ti.Platform.architecture);
    Ti.API.debug("Ti.Platform.address " + Ti.Platform.address);
    Ti.API.debug("Ti.Platform.id " + Ti.Platform.id);
    Ti.API.debug("Ti.Platform.macaddress " + Ti.Platform.macaddress);
    Ti.API.debug("Ti.Platform.processorCount " + Ti.Platform.processorCount);
    Ti.API.debug("Ti.Platform.username " + Ti.Platform.username);

    // TODO add more specific tests to insure we report the correct platform.
    if (Ti.platform == "osx") {
      value_of(Ti.Platform.name)
        .should_not_be("Windows NT");
    } else if (Ti.platform == "linux") {
      value_of(Ti.Platform.name)
        .should_not_be("Windows NT");
    } else if (Ti.platform == "win32") {
      value_of(Ti.Platform.name)
        .should_be("Windows NT");
    } else {
      // this is a delibrate error.  we should never get here.
      // but just in case we do, log it.
      value_of(Ti.Platform.name)
        .should_not_be("unknown");
    }
  },

  test_unique_uuid: function () {
    var uuids = [];
    for (var i = 0; i < 100; i++) {
      var uuid = Ti.Platform.createUUID();
      value_of(uuid in uuids)
        .should_be_false();
      uuids.push(uuid);
    }
  }
});
