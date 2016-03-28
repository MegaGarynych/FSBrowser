#FSBrowser

##Introduction
I wanted to add a standard environment for all my ESP8266 based projects to be able to configure them via web browser to avoid code editing when I need to change some settings like SSID, password, etc.

I found [John Lassen's WebConfig project](http://www.john-lassen.de/index.php/projects/esp-8266-arduino-ide-webconfig) to almost fit my needs. It uses a simple but powerful web interface with dynamic data using [microAJAX](https://code.google.com/archive/p/microajax/). I've added some dynamic data using [Links2004](https://github.com/Links2004)'s [WebSockets library](https://github.com/Links2004/arduinoWebSockets).

So, I tried to fork it. Original WebConfig project stores web pages on PROGMEM, but I was recently using SPIFFS on ESP8266 and I think it is a good way to store web content. I found I was not the first to think this when I noticed about [FSBrowser](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples/FSBrowser) example in [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer) library. 

This example code has a great bonus. A text editor (based on [ACE](https://ace.c9.io/)) with syntax highligth that can be used to edit html on board directly. I found a [bug](https://github.com/esp8266/Arduino/pull/1771) on it (already sent to github) and, when fixed, noticed how usefull this editor is.

My code is a fork of both projects, and has become a start framework for my future projects.

I only have to add more HTML data and some funtions to show dynamic data in order to adapt it to any other task.

It has integrated NTP syncronization based on my own [NTPClient library](https://github.com/gmag11/NtpClient), that can be configured via Web browser too.

Configuration is stored on SPIFFS too, in a JSON file. So, it recovers config during boot.

####**There is no implemented security or authentication yet. Take this in account when connecting it to public networks or Internet.**

##WiFi connection
I've implemented a way to turn ESP8266 into AP mode so I can set WiFi config when prior WiFi is not available. It is done by setting IO pin 4 to high status during boot. You must ensure it is connected to GND to allow it to connect to a router.

WiFi connection to router is confirmed via LED in pin 0 as soon an IP address is got. Same LED is used to confirm AP mode by flashing 3 times.

##Schematics

![Fritzing breadboard](http://i1077.photobucket.com/albums/w470/gmag111/2016-03-28%2014_19_45_zpsu2ejb71r.png)

##How to use it

*`http://ip_address/admin.html` takes you to main configuration GUI.
*`http://ip_address/edit` contains code editor. Any file stored in SPIFFS may be loaded here and saved using `CTRL+S` command.
*`http://ip_address/` includes an example info page that shows how to get realtime data from ESP.

##TODO

- Network Authentication/Security
- Configuration protection
- HTTPS (in evaluation)
- Integration of editor in admin.html
- Convert code to classes and try to design a library to add all this functionality easily
