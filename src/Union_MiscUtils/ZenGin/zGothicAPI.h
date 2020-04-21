// Supported with union (c) 2018 Union team

#ifndef __ZGOTHIC_API_H__
#define __ZGOTHIC_API_H__


// gothic api for Gothic I v1.08k_mod
#include "Gothic_I_Classic/API/zEngine.h"
#ifdef __G1
#include "Gothic_I_Classic/vtable_Ver0.h"
#endif


// gothic api for Gothic Sequel v1.12f (bin)
#include "Gothic_I_Addon/API/zEngine.h"
#ifdef __G1A
#include "Gothic_I_Addon/vtable_Ver1.h"
#endif


// gothic api for Gothic II classic v1.30
#include "Gothic_II_Classic/API/zEngine.h"
#ifdef __G2
#include "Gothic_II_Classic/vtable_Ver2.h"
#endif


// gothic api for Gothic II NOTR v2.6 (fix)
#include "Gothic_II_Addon/API/zEngine.h"
#ifdef __G2A
#include "Gothic_II_Addon/vtable_Ver3.h"
#endif


#endif // __ZGOTHIC_API_H__