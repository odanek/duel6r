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

#include <vector>
#include "project.h"

namespace Duel6
{
	namespace
	{
		class WaterVertex
		{
		private:
			Float32 y;
			Vertex& vertex;

		public:
			WaterVertex(Vertex& vertex, Float32 height)
				: vertex(vertex)
			{
				y = vertex.Y - D6_WAVE_HEIGHT;
			}

			Vertex& getVertex()
			{
				return vertex;
			}

			Float32 getY() const
			{
				return y;
			}
		};

		std::vector<WaterVertex> d6WaterVertexList;
		Float32 d6WaterPhase = 0;
	}

	void WATER_Move(float elapsedTime)
	{
		d6WaterPhase += 122 * elapsedTime;
		if (d6WaterPhase >= 360)
		{
			d6WaterPhase -= 360;
		}

		for (WaterVertex& wv : d6WaterVertexList)
		{
			Float32 vertexPhase = d6WaterPhase + 60.0f * wv.getVertex().X;
			Float32 height = D6_Sin((Int32)vertexPhase) * D6_WAVE_HEIGHT;
			wv.getVertex().Y = wv.getY() + height;
		}
	}

	void WATER_Build(void)
	{
		g_app.con->printf(MY_L("APP00083|...Sestavuji water-list\n"));
		d6WaterVertexList.clear();

		for (Vertex& vertex : d6World.getWater().getVertexes())
		{
			if (vertex.Flags == D6_FLAG_FLOW)
			{
				d6WaterVertexList.push_back(WaterVertex(vertex, D6_WAVE_HEIGHT));
			}
		}
	}
}