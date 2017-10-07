/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
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

#ifndef DUEL6_CONTEXT_H
#define DUEL6_CONTEXT_H

#include <string>
#include <stack>
#include "Type.h"
#include "SysEvent.h"

namespace Duel6 {
    class Context {
    private:
        static std::stack<Context *> contextStack;
        bool closed;

    public:
        virtual ~Context() {}

        static bool exists() {
            return !contextStack.empty();
        }

        static Context &getCurrent() {
            return *getTopContext();
        }

        virtual bool isCurrent() const final {
            return (this == getTopContext());
        }

        virtual bool is(const Context &context) const final {
            return (this == &context);
        }

        virtual void keyEvent(const KeyPressEvent &event) = 0;

        virtual void textInputEvent(const TextInputEvent &event) = 0;

        virtual void mouseButtonEvent(const MouseButtonEvent &event) = 0;

        virtual void mouseMotionEvent(const MouseMotionEvent &event) = 0;

        virtual void mouseWheelEvent(const MouseWheelEvent &event) = 0;

        virtual void update(Float32 elapsedTime) = 0;

        virtual void render() const = 0;

        virtual bool isClosed() const final {
            return closed;
        }

        static void push(Context &context) {
            Context *lastContext = getTopContext();
            if (lastContext != nullptr) {
                lastContext->beforeClose(&context);
            }

            contextStack.push(&context);
            context.closed = false;
            context.beforeStart(lastContext);
        }

        static void pop() {
            if (exists()) {
                Context &currentContext = getCurrent();
                contextStack.pop();
                Context *prevContext = getTopContext();
                currentContext.beforeClose(prevContext);

                if (prevContext != nullptr) {
                    prevContext->beforeStart(&currentContext);
                }
            }
        }

    protected:
        virtual void close() final {
            closed = true;
        }

        virtual void beforeStart(Context *prevContext) = 0;

        virtual void beforeClose(Context *nextContext) = 0;

        static Context *getTopContext() {
            return (contextStack.size() > 0) ? contextStack.top() : nullptr;
        }
    };
}

#endif