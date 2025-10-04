cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(bpm180_beacon_gatt)
target_sources(app PRIVATE src/main.c src/temperature_sensor_service.c)
