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

/*
Projekt: Konzole
Popis: Zpracovani promenych
*/

#include <sstream>
#include "console.h"

namespace Duel6 {
    void Console::registerVariable(const std::string &name, Uint32 flags, Variable::Pointer &&ptr) {
        verifyRegistration(name, CON_Lang("Variable registration"), ptr == nullptr);

        // Sort lexicographically
        Size position = 0;
        for (const VarRecord &var : vars) {
            if (var.getName().compare(name) > 0) {
                break;
            }
            ++position;
        }

        vars.emplace(vars.begin() + position, name, flags, std::forward<Variable::Pointer>(ptr));

        if (hasFlag(RegInfoFlag)) {
            print(CON_Format(CON_Lang("Variable registration: \"{0}\" has been successful\n")) << name);
        }
    }

    Console::VarRecord *Console::findVar(const std::string &name) {
        for (VarRecord &var : vars) {
            if (var.getName() == name) {
                return &var;
            }
        }

        return nullptr;
    }

    void Console::varCmd(VarRecord &var, Arguments &args) {
        Size c = args.length();

        if (c > 2) {
            print(CON_Lang("CONSTR0041|Variables : Usage variable_name [new_value]\n"));
            return;
        }

        if (c == 1) {
            var.printInfo(*this);
        } else {
            if (!(var.hasFlag(Variable::ReadOnlyFlag))) {
                var.setValue(args.get(1));
            } else {
                print(CON_Format(CON_Lang("Variable \"{0}\" is read-only\n")) << var.getName());
            }
        }
    }

    void Console::VarRecord::printInfo(Console &console) const {
        const char *flagstr = "ra";
        int i, f = 1;

        for (i = 0; i < 2; i++, f <<= 1) {
            if (hasFlag(f)) {
                console.print(std::string(1, flagstr[i]));
            } else {
                console.print("-");
            }
        }

        console.print(
                CON_Format(CON_Lang(" {0} \"{1}\" with value {2}\n")) << var->getTypeName() << name << getValue());
    }

    std::string Console::IntVariable::getValue() const {
        return std::to_string(value);
    }

    void Console::IntVariable::setValue(const std::string &val) {
        value = std::stoi(val);
    }

    std::string Console::IntVariable::getTypeName() const {
        return "int";
    }

    std::string Console::BoolVariable::getValue() const {
        return value ? "true" : "false";
    }

    void Console::BoolVariable::setValue(const std::string &val) {
        value = (val == "true");
    }

    std::string Console::BoolVariable::getTypeName() const {
        return "bool";
    }

    std::string Console::FloatVariable::getValue() const {
        return std::to_string(value);
    }

    void Console::FloatVariable::setValue(const std::string &val) {
        value = std::stof(val);
    }

    std::string Console::FloatVariable::getTypeName() const {
        return "float";
    }

    template<>
    std::unique_ptr<Console::Variable> Console::Variable::from(Int32 &val) {
        return std::make_unique<Console::IntVariable>(val);
    }

    template<>
    std::unique_ptr<Console::Variable> Console::Variable::from(Float32 &val) {
        return std::make_unique<Console::FloatVariable>(val);
    }

    template<>
    std::unique_ptr<Console::Variable> Console::Variable::from(bool &val) {
        return std::make_unique<Console::BoolVariable>(val);
    }
}