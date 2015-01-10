#ifndef LAYER2_DECODE_MACROS_HPP
#define LAYER2_DECODE_MACROS_HPP

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")

#define SET_BOOL(obj, s, v) (obj)->Set(NanNew<String>((s)), NanNew<Boolean>((v)))
#define SET_INT(obj, s, v) (obj)->Set(NanNew<String>((s)), NanNew<Integer>((v)))
#define SET_STR(obj, s, v) (obj)->Set(NanNew<String>((s)), NanNew<String>((v)))
#define SET_NULL(obj, s) (obj)->Set(NanNew<String>((s)), NanNull())

#endif
