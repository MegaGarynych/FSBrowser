#FSBrowser

##Introduction
I wanted to add a standard environment for all my ESP8266 based projects to be able to configure them via web browser to avoid code editing when I need to change some settings like SSID, password, etc.

I found [John Lassen's WebConfig project](http://www.john-lassen.de/index.php/projects/esp-8266-arduino-ide-webconfig) to almost fit my needs. It uses a simple but powerful web interface with dynamic data using [microAJAX](https://code.google.com/archive/p/microajax/).

So, I tried to fork it. Original WebConfig project stores web pages on PROGMEM, but I was recently using SPIFFS on ESP8266 and I think it is a good way to store web content. I found I was not the first to think this when I noticed about [FSBrowser](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples/FSBrowser) example in [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer) library. 

This example code has a great bonus. A text editor (based on [ACE](https://ace.c9.io/)) with syntax highligth that can be used to edit html on board directly. I found a [bug](https://github.com/esp8266/Arduino/pull/1771) on it (already sent to github) and noticed how usefull it is.

This project is a fork of both projects, havind a start framework for my future projects.

I only have to add more HTML data and some funtions to show dynamic data.

It has integrated NTP syncronization based on my own [NTPClient library](https://github.com/gmag11/NtpClient). It can be configured via Web browser too.

Configuration is stored on SPIFFS too, in a JSON file. So, it recovers config during boot.

##WiFi connection
I've implemented a way to turn ESP8266 into AP mode so I can set WiFi config when prior WiFi is not available. It is done by setting IO pin 4 to high status during boot. You must ensure it is connected to GND to allow it to connect to a router.

WiFi connection to router is confirmed via LED in pin 0 as soon an IP address is got. Same LED is used to confirm AP mode by flashing 3 times.

