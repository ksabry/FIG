#pragma once

#include <assert.h>

namespace FIG
{
    template<typename TInherit> 
    struct FieldInitializable;

    template<typename TContainer, int ID>
    struct _Field {};

    template<typename TContainer, typename TField, typename type>
    struct _FieldSetter
    {
        static void set(FieldInitializable<TContainer>*, type)
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
        void Init(TField, TVal val, TArgs... args)
        {
            SetField<TField>(val);
            Init(args...);
        }
        template<typename TField, typename TVal>
        void Init(TField, TVal val)
        {
            SetField<TField>(val);
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

#define ADD_FIELD(type,fieldName,inheritName,innerField) \
    constexpr _Field<inheritName, __LINE__> fieldName;\
    template<typename T>\
    struct _FieldSetter<inheritName, _Field<inheritName, __LINE__>, T>\
    {\
        static void set( FieldInitializable <inheritName> * container, T val)\
        {\
            static_assert(std::is_convertible< T, type >::value, "Invalid type for FontRendererSettings field " #fieldName " (must be " #type ")");\
            inheritName* inherit = (inheritName*)(container);\
            inherit->innerField = val;\
        }\
    }
}