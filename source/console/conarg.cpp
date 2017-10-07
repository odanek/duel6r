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
Popis: Zpracovani argumentu a prikazoveho bufferu
*/

#include <stdio.h>
#include "console.h"

namespace Duel6 {
    std::string Console::expandLine(const std::string &line) {
        std::string output;
        bool quotes = false;
        auto current = line.begin(), lineEnd = line.end();

        while (current != lineEnd) {
            if (!quotes && current + 1 != lineEnd && current[0] == '$' && current[1] == '{') {
                auto varEnd = current + 2;
                while (varEnd != lineEnd && *varEnd != '}') {
                    ++varEnd;
                }

                if (varEnd == lineEnd) {
                    // No closing brace -> no variables to expand
                    output.append(line.substr(current - line.begin()));
                    return output;
                } else {
                    // Try to expand variable
                    auto varBegin = current + 2;
                    const VarRecord *var = findVar(line.substr(varBegin - line.begin(), varEnd - varBegin));
                    if (var != nullptr) {
                        output.append(var->getValue());
                        current = varEnd + 1;
                        continue;
                    }
                }
            }

            if (*current == '"') {
                quotes = !quotes;
            }

            output.push_back(*current);
            ++current;
        }

        return output;
    }

    std::string::const_iterator
    Console::nextToken(const std::string &line, std::string::const_iterator &begin, std::string::const_iterator &end) {
        auto lineEnd = line.end();

        while (begin != lineEnd) {
            if (begin[0] > ' ') {
                break;
            }

            ++begin;
        }

        if (begin == lineEnd) {
            end = begin;
            return end;
        }

        if (begin + 1 != lineEnd && begin[0] == '/' && begin[1] == '/') {
            // The rest is a comment, we are done
            begin = end = lineEnd;
            return end;
        }

        bool quotes = (*begin == '"');
        char sentinel = (quotes ? '"' : ' ');
        end = begin + 1;

        while (end != lineEnd && *end != sentinel) {
            ++end;
        }

        if (quotes) {
            begin = begin + 1;

            if (end == lineEnd) {
                // This is error, unclosed quotes
                return end;
            } else if (end + 1 != lineEnd && end[1] != ' ') {
                // This is also error, closing quote is not followed by space
            }

            return end + 1;
        }

        return end;
    }

    void Console::tokenizeLine(const std::string &line, Arguments &args) {
        args.clear();

        auto current = line.begin();
        while (current != line.end()) {
            std::string::const_iterator begin = current, end;
            current = nextToken(line, begin, end);

            if (begin != line.end()) {
                args.add(line.substr(begin - line.begin(), end - begin));
            }
        }
    }

    void Console::executeSingleLine(const std::string &line) {
        if (line.empty())
            return;

        if (hasFlag(ExpandFlag)) {
            tokenizeLine(expandLine(line), arguments);
        } else {
            tokenizeLine(line, arguments);
        }

        if (arguments.length() > 0) {
            const CommandRecord *p = findCommand(arguments.get(0));
            if (p != nullptr) {
                p->getCommand()(*this, arguments);
                return;
            }

            const AliasRecord *a = findAlias(arguments.get(0));
            if (a != nullptr) {
                if (++aliasloop == CON_MAX_ALIAS_REC) {
                    print(CON_Lang(
                                  "Console error : Alias recursion overflow, remaining alias commands will be ignored\n"));
                } else if (aliasloop < CON_MAX_ALIAS_REC) {
                    prependCommands(a->getCommand());
                }
                return;
            }

            VarRecord *v = findVar(arguments.get(0));
            if (v != nullptr) {
                varCmd(*v, arguments);
                return;
            }

            print(CON_Format(CON_Lang("Unknown command : \"{0}\"\n")) << arguments.get(0));
        }
    }

    void Console::splitCommandsIntoLines(const std::string &commands, std::vector<std::string> &lines) {
        auto lineBegin = commands.begin(), lineEnd = lineBegin;
        bool quotes = false;

        while (lineEnd != commands.end()) {
            if (*lineEnd == '"') {
                quotes = !quotes;
            }

            if (*lineEnd == '\n' || *lineEnd == '\r' || (!quotes && *lineEnd == ';')) {
                if (lineBegin == lineEnd) {
                    ++lineBegin;
                } else {
                    lines.push_back(commands.substr(lineBegin - commands.begin(), lineEnd - lineBegin));
                    lineBegin = lineEnd + 1;
                }
                lineEnd = lineBegin;
            } else {
                ++lineEnd;
            }
        }

        if (lineBegin != lineEnd) {
            lines.push_back(commands.substr(lineBegin - commands.begin(), lineEnd - lineBegin));
        }
    }

    Console &Console::appendCommands(const std::string &commands) {
        std::vector<std::string> lines;
        splitCommandsIntoLines(commands, lines);
        cbuf.insert(cbuf.end(), lines.begin(), lines.end());
        return *this;
    }

    Console &Console::prependCommands(const std::string &commands) {
        std::vector<std::string> lines;
        splitCommandsIntoLines(commands, lines);
        cbuf.insert(cbuf.begin(), lines.begin(), lines.end());
        return *this;
    }

    void Console::execute() {
        while (!cbuf.empty()) {
            std::string line = cbuf.front();
            cbuf.pop_front();

            executeSingleLine(line);
        }
    }

    void Console::exec(const std::string &commands) {
        appendCommands(commands);
        execute();
    }
}