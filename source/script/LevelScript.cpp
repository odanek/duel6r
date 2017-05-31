/*
* Copyright (c) 2017, Frantisek Veverka
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Frantisek Veverka nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <angelscript.h>
#include "LevelScript.h"

namespace Duel6 {

template<typename R>
R getReturn();

template<>
void Function::getReturn() {
	return;
}

template<>
Function::ADDRESS Function::getReturn() {
	return ctx->GetReturnAddress();
}
template<>
asBYTE Function::getReturn() {
	return ctx->GetReturnByte();
}
template<>
double Function::getReturn() {
	return ctx->GetReturnDouble();
}
template<>
asDWORD Function::getReturn() {
	return ctx->GetReturnDWord();
}

template<>
float Function::getReturn() {
	return ctx->GetReturnFloat();
}
template<>
Function::OBJECT Function::getReturn() {
	return ctx->GetReturnObject();
}
template<>
asQWORD Function::getReturn() {
	return ctx->GetReturnQWord();
}
template<>
asWORD Function::getReturn() {
	return ctx->GetReturnWord();
}

template<>
int Function::setParam(asUINT arg, Function::ADDRESS addr) {
	// SetArgAddress can be used for both references and handles. For handles it will not automatically increment the object reference.
	// Usually you'll want to use SetArgAddress for references,
	// and SetArgObject for handles and objects sent by value.
	return ctx->SetArgAddress(arg, addr);
}

template<>
int Function::setParam(asUINT arg, int value) {
	return ctx->SetArgAddress(arg, &value);
}

template<>
int Function::setParam(asUINT arg, asBYTE value) {
	return ctx->SetArgByte(arg, value);
}

template<>
int Function::setParam(asUINT arg, double value) {
	return ctx->SetArgDouble(arg, value);
}

template<>
int Function::setParam(asUINT arg, asDWORD value) {
	return ctx->SetArgDWord(arg, value);
}
template<>
int Function::setParam(asUINT arg, float value) {
	return ctx->SetArgFloat(arg, value);
}
template<>
int Function::setParam(asUINT arg, Function::OBJECT obj) {
	// SetArgObject can be used for handles, references to objects, and objects sent by value.
	// For handles it will automatically increment the reference, and for objects sent by value
	// it will make a copy of the object. Usually you'll want to use SetArgAddress for references,
	// and SetArgObject for handles and objects sent by value.
	return ctx->SetArgObject(arg, obj);
}
template<>
int Function::setParam(asUINT arg, asQWORD value) {
	return ctx->SetArgQWord(arg, value);
}
template<>
int Function::setParam(asUINT arg, asWORD value) {
	return ctx->SetArgWord(arg, value);
}

Function::~Function() {
	if(exists){
		function->Release();
	}
	ctx->Release();
}
}
