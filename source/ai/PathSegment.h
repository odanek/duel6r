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
#include <sys/types.h>
#include <new>
class PathSegment{
public:
	uint id = 0;
	uint l = 0;
	uint r = 0;

	// TODO signed int for having the ground level y == -1
	int y = 0;
	PathSegment(){}

	PathSegment(uint id, uint l, uint r, int y): id(id), l(l), r(r), y(y){}

	static PathSegment* factory();

	static PathSegment* factory(uint id, uint l, uint r, int y){
		return new PathSegment(id, l, r, y);
	}
	// For scripting
	static void constructor(uint id, uint l, uint r, int y, void * self){
		new(self) PathSegment(id, l, r, y);
	}

	void addRef(){
		refCounter++;
	}
	void release(){
		if(--refCounter == 0) delete this;
	}
private:
	uint refCounter = 1;
};