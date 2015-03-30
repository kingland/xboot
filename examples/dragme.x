main.lua                                                                                            000664  001750  001750  00000003601 12506201376 012105  0                                                                                                    ustar 00jjj                             jjj                             000000  000000                                                                                                                                                                         local stage = application:getStage()
local width, height = application:getScreenSize()

local function onMouseDown(self, e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = -1
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

local function onMouseMove(self, e)
	if self.touchid == -1 then	
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

local function onMouseUp(self, e)
	if self.touchid == -1 then
		self.touchid = nil
		e:stopPropagation()
	end
end

local function onTouchBegin(self, e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

local function onTouchMove(self, e)
	if self.touchid == e.info.id then
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

local function onTouchEnd(self, e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

for i = 1, 5 do
	local shape = DisplayShape.new(100, 50)
		:setLineWidth(6)
		:rectangle(0, 0, 100, 50)
		:setSourceColor(1, 0, 0, 0.5)
		:fillPreserve()
		:setSourceColor(0, 0, 0)
		:stroke()
		:setPosition(math.random(0, width - 100), math.random(0, height - 50))

	shape:addEventListener(Event.MOUSE_DOWN, onMouseDown)
	shape:addEventListener(Event.MOUSE_MOVE, onMouseMove)
	shape:addEventListener(Event.MOUSE_UP, onMouseUp)
	shape:addEventListener(Event.TOUCH_BEGIN, onTouchBegin)
	shape:addEventListener(Event.TOUCH_MOVE, onTouchMove)
	shape:addEventListener(Event.TOUCH_END, onTouchEnd)

	stage:addChild(shape)
end

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               