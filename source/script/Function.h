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
#pragma once

#include <stdio.h>
namespace Duel6 {

void PrintCallstack(asIScriptContext *ctx);

class Function {

private:
	asIScriptFunction * function;
	asIScriptContext * ctx;
	// Function found in the file with the script
	bool exists = false;
public:
	bool ready();

	// to distinguish passing objects and addresses to/from angelscript scripts in templated setParam() and getReturn()
	struct OBJECT {
		void * obj;
		OBJECT(void * obj) :
				obj(obj) {
		}
		operator void *() {
			return obj;
		}
	};
	struct ADDRESS {
		void * address;
		ADDRESS(void * address) :
				address(address) {
		}
		operator void *() {
			return address;
		}
	};

	virtual ~Function();

	Function(asIScriptModule * module, asIScriptContext * ctx, const char * funcDecl);
	template<typename R, typename T, typename ...A>
	R call(T t, A ... a) {
		ctx->Prepare(function);
		setParams(0, t, a...);

		auto r = ctx->Execute();
		if (r != asEXECUTION_FINISHED) {
			if (r == asEXECUTION_EXCEPTION) {
				const char * sectionName;
				int line, column;
				line = ctx->GetExceptionLineNumber(&column, &sectionName);
				fprintf(stderr, "%s:%d\n  %s: %d: %s\n", __FILE__, __LINE__, sectionName, line, ctx->GetExceptionString());
				PrintCallstack(ctx);
			}
		}

		return getReturn<R>();
	}

	template<typename R, typename T>
	R call(T t) {
		ctx->Prepare(function);
		setParams(0, t);

		auto r = ctx->Execute();
		if (r != asEXECUTION_FINISHED) {
			// The execution didn't complete as expected. Determine what happened.
			if (r == asEXECUTION_EXCEPTION) {
				const char * sectionName;
				int line, column;
				line = ctx->GetExceptionLineNumber(&column, &sectionName);
				fprintf(stderr, "%s:%d\n  %s: %d: %s\n", __FILE__, __LINE__, sectionName, line, ctx->GetExceptionString());
				PrintCallstack(ctx);
			}
		}

		return getReturn<R>();
	}

private:
	template<typename R>
	R getReturn();

	template<typename T, typename ...A>
	void setParams(int argPos, T t, A ... a) {
		setParam(argPos, t);
		setParams<A...>(argPos + 1, a...);
	}

	template<typename T>
	void setParams(int argPos, T t) {
		setParam(argPos, t);
	}

	template<typename T>
	int setParam(asUINT i, T t);
};
}
