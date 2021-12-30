// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Config

const char *config[] = {
    "WIFI_SSID", "WIFI_PASSWORD", "LATITUDE", "LONGITUDE", "SPOTNIK_CALL", "http://192.168.1.1:3000/" 
};

/*
Note that it's possible to configure many configurations, like this
const char *config[] = {
    "WIFI_SSID_1", "WIFI_PASSWORD_1", "LATITUDE_1", "LONGITUDE_1", "SPOTNIK_CALL_1", "http://IP_1:3000/",
    "WIFI_SSID_2", "WIFI_PASSWORD_2", "LATITUDE_2", "LONGITUDE_2", "SPOTNIK_CALL_2", "http://IP_2:3000/", 
    "WIFI_SSID_3", "WIFI_PASSWORD_3", "LATITUDE_3", "LONGITUDE_3", "SPOTNIK_CALL_3", "http://IP_3:3000/" 
};
*/

// Qrz

const char *qrz[] = {
    "F1EVM",    "0x0000ff",
    "F4HWN",    "0xffffff",
    "F5NLG",    "0xff0000",
};

/*
This list is only useful if you have an M5GO (for GREY or BASIC) or M5GO2 (for CORE2) module. 
It allows you to have led alerts according to the callsign currently being transmitted. 
Each callsign is associated with a color encoded in RGB format.
*/