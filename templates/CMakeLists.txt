{% import "qface/qtcpp.j2" as cpp %}
{{ cpp.autogenerated(prefix="#") }}

project({{project}})

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

set(INSTALL_PATH "${CMAKE_BINARY_DIR}/imports" CACHE STRING "Path where the plugins are deployed")

{% for module in system.modules %}
add_subdirectory({{module|identifier}})
{% endfor %}
