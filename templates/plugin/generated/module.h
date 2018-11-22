{# Copyright (c) Pelagicore AB 2016 #}
{% import "qface/qtcpp.j2" as cpp %}
{{ cpp.autogenerated() }}
{% set class = '{0}Module'.format(module.module_name|upper_first) %}

#pragma once

#include <QtCore>
#include <QtQuick>
#include "variantmodel.h"

{% for interface in module.interfaces %}
class {{interface}}Base;
{% endfor %}

{# ***************************************************************** #}
{# *** ENUMERATIONS                                              *** #}
{# ***************************************************************** #}
{% for enum in module.enums %}
{% set class = '{0}'.format(enum) %}
// ********************************************************************
// Enumeration {{class}}
// ********************************************************************

class {{class}} : public QObject {
    Q_OBJECT
public:
    {{class}}(QObject *parent = nullptr)
        : QObject(parent)
        {}
    {{ cpp.enum_decl(enum) }}

    static {{enum}}Enum toEnum(quint8 v, bool *ok);
};

inline QDataStream &operator<<(QDataStream &ds, const {{class}}::{{enum}}Enum &obj)
{
    quint8 val = obj;
    ds << val;
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, {{class}}::{{enum}}Enum &obj) {
    bool ok;
    quint8 val;
    ds >> val;
    obj = {{class}}::toEnum(val, &ok);
    if (!ok) {
        qWarning() << "received an invalid enum value for typeState, value =" << val;
    }
    return ds;
}


{% endfor %}

{# ***************************************************************** #}
{# *** STRUCTS                                                   *** #}
{# ***************************************************************** #}
{% for struct in module.structs %}
{% set class = '{0}'.format(struct) %}
// ********************************************************************
// Struct {{class}}
// ********************************************************************

class {{class}}
{
    Q_GADGET
{% for field in struct.fields %}
    {{ cpp.property(field, notifiable=False)}}
{% endfor %}

public:
    {{class}}();
{% for field in struct.fields %}
    {{cpp.property_setter_decl(field)}}
    {{cpp.property_getter_decl(field)}}
{% endfor %}

    bool operator==(const {{class}} &other) const;
    bool operator!=(const {{class}} &other) const;

private:
{% for field in struct.fields %}
    {{field|qt.returnType}} m_{{field}};
{% endfor %}
};

QDataStream &operator<<(QDataStream &stream, const {{class}} &obj);
QDataStream &operator>>(QDataStream &stream, {{class}} &obj);


Q_DECLARE_METATYPE({{class}})

//
// {{class}} Factory
//

class {{class}}Factory : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE {{class}} create();
};

{% endfor %}

{# ***************************************************************** #}
{# *** INTERFACES                                                *** #}
{# ***************************************************************** #}
{% for interface in module.interfaces %}
{% set class = '{0}Base'.format(interface) %}
//
// Interface {{class}}
//

{% if 'item' in interface.tags %}
class {{class}} : public QQuickItem
{% else %}
class {{class}} : public QObject
{% endif %}
{
    Q_OBJECT
{% for property in interface.properties %}
    {{ cpp.property_decl(property)}}
{% endfor %}

public:
{% if 'item' in interface.tags %}
    explicit {{class}}(QQuickItem *parent = nullptr);
{% else %}
    explicit {{class}}(QObject *parent = nullptr);
{% endif %}
    virtual ~{{class}}();
public Q_SLOTS:
{% for operation in interface.operations %}
    {{ cpp.operation_decl(operation, ending=" = 0;") }}
{% endfor %}
public:
{% for property in interface.properties %}
    virtual {{cpp.property_setter_decl(property, ending=" = 0;")}}
    virtual {{cpp.property_getter_decl(property, ending=" = 0;")}}

{% endfor %}
Q_SIGNALS:
{% for signal in interface.signals %}
    {{ cpp.signal_decl(signal)}}
{% endfor %}
{% for property in interface.properties %}
    {{ cpp.signal_decl(property, 'Changed') }}
{% endfor %}
};

{% if 'factory' in interface.tags %}
{% set class = '{0}Factory'.format(interface) %}
{% set base = '{0}Base'.format(interface) %}
//
// Factory for {{interface}}
//
class {{class}}
{
public:
    static {{class}} *instance();
    {{base}} *create(const QString &id, QObject *parent);
    template<typename T>
    void registerType(const QString &id);
private:
    friend class {{class}}Loader;
    explicit {{class}}();
    using CreatorFunc = {{base}} * (*)(QObject *);
    QHash<QString, CreatorFunc> m_creatorFuncs;
};

template <typename T>
inline void qRegister{{interface}}(const QString &id)
{
    {{class}}::instance()->registerType<T>(id);
}
{% endif %}

{% endfor %}
