console = script.console

function log(msg)
	console.print(person.name .. " says: " .. msg)
end

function roundStart(context)
	currentDirection = 1;
end

function roundUpdate(context, roundTime)
	local player = context.player
	local position = player.centre
	local level = context.level;

	if currentDirection == 1 then
		player.pressRight()
		local blockRight = level.blockAt(position.x + 0.5, position.y)
		if blockRight == nil or blockRight.wall then
			currentDirection = -1
		end
	else
		player.pressLeft()
		local blockLeft = level.blockAt(position.x - 0.5, position.y)
		if blockLeft == nil or blockLeft.wall then
			currentDirection = 1
		end
	end

	local blockAbove = level.blockAt(position.x, position.y + 1)
	if blockAbove ~= nil and not blockAbove.wall then
		player.pressUp()
	end
end

function roundEnd(context, roundTime)
end
