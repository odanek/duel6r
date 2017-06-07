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

#include "LevelScript.h"

namespace Duel6 {

LevelScript::LevelScript(asIScriptModule * module, asIScriptContext * ctx):
		Script(module, ctx),
		mapLoadedFn(module, ctx, SIGNATURE_MAP_LOADED),
		playerThinkFn(module, ctx, SIGNATURE_PLAYER_THINK),
		roundUpdateFn(module, ctx, SIGNATURE_ROUND_UPDATE) {

}

void LevelScript::mapLoaded(Level & level) {
	if (mapLoadedFn.ready()) {
		mapLoadedFn.call<void, Function::ADDRESS>(&level);
	}
}

void LevelScript::playerThink(Player & player, unsigned int id) {
	if (playerThinkFn.ready()) {
		playerThinkFn.call<void, Function::ADDRESS, int>(&player, id);
	}
}

void LevelScript::roundUpdate(Round & round, float elapsedTime, Uint32 frame) {
	if (roundUpdateFn.ready()) {
		roundUpdateFn.call<void, Function::ADDRESS, float, asDWORD>(&round, elapsedTime, frame);
	}
}

}
