#pragma once

#include <assert.h>
#include <type_traits>

namespace FIG
{
    template<typename TInherit> 
    struct FieldInitializable;

    template<typename TField, typename type>
    struct _FieldValuePair;

    template<typename TContainer, int ID>
    struct _Field
    {
        template<typename TVal>
        _FieldValuePair<_Field<TContainer, ID>, TVal> operator=(const TVal other) const
        {
            return _FieldValuePair<_Field<TContainer, ID>, TVal>(*this, other);
        }
    };

    struct _FieldValuePair_Base {};

    template<typename TField, typename TVal>
    struct _FieldValuePair : _FieldValuePair_Base
    {
    public:
        _FieldValuePair(TField field, TVal value)
            : field(field), value(value)
        {
        }
        TField field;
        TVal value;
    };

    template<typename TContainer, typename TField, typename TVal>
    struct _FieldSetter
    {
        static void set(FieldInitializable<TContainer>*, TVal)
        {
            static_assert(false, "Unrecognized field in FontRendererSettings initialization");
        }
    };

    template<typename TInherit>
    struct FieldInitializable
    {
    public:
        template<typename... TArgs>
        void Init(TArgs... args)
        {
            static_assert(false, "Malformed arguments to FieldInitializable; must be in field, value pairs");
        }
        template<typename TField, typename TVal, typename... TArgs>
        void Init(_FieldValuePair<TField, TVal> pair, TArgs... args)
        {
            SetField<TField>(pair.value);
            Init(args...);
        }
        template<typename TField, typename TVal>
        void Init(_FieldValuePair<TField, TVal> pair)
        {
            SetField<TField>(pair.value);
        };
        template<>
        void Init<>()
        {
        };

        template<typename TField, typename TVal>
        void SetField(TVal val)
        {
            _FieldSetter<TInherit, TField, TVal>::set(this, val);
        }
    };

#define ADD_EXISTING_FIELD(type,fieldName,inheritName,innerField) \
    template<typename TVal>\
    struct _FieldSetter<inheritName, _Field<_##fieldName##_TYPE, _##fieldName##_ID>, TVal>\
    {\
        static void set( FieldInitializable <inheritName> * container, TVal val)\
        {\
            static_assert(std::is_convertible< TVal, type >::value, "Invalid type for FontRendererSettings field " #fieldName " (must be " #type ")");\
            inheritName* inherit = (inheritName*)(container);\
            inherit->innerField = (type)val;\
        }\
    }

#define ADD_FIELD(type,fieldName,inheritName,innerField) \
    constexpr int _##fieldName##_ID = __LINE__; \
    typedef inheritName _##fieldName##_TYPE; \
    constexpr _Field<_##fieldName##_TYPE, _##fieldName##_ID> fieldName;\
    ADD_EXISTING_FIELD(type,fieldName,inheritName,innerField)
}