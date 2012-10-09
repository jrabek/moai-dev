----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 320, 480 )

MOAIDebugLines.setStyle ( MOAIDebugLines.PROP_MODEL_BOUNDS, 2, 1, 1, 1 )
MOAIDebugLines.setStyle ( MOAIDebugLines.PROP_WORLD_BOUNDS, 2, 0.75, 0.75, 0.75 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, -480 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

gfxQuad = MOAIGfxQuad2D.new ()
gfxQuad:setTexture ( "moai.png" )
gfxQuad:setRect ( -32, -32, 32, 32 )
gfxQuad:setUVRect ( 0, 0, 1, 1 )

prop1 = MOAIProp2D.new ()
prop1:setDeck ( gfxQuad )
prop1:setLoc ( -128, -128 )
ease1 = prop1:moveLoc ( 256, 256, 5 , MOAIEaseType.LINEAR)
layer:insertProp ( prop1 )

prop2 = MOAIProp2D.new ()
prop2:setDeck ( gfxQuad )
prop2:setRot ( 45 )
prop2:setLoc ( 0, 0 )
layer:insertProp ( prop2 )

r = 0
g = 0
b = 0

function collisionHandler(prop1, prop2)
	print("Collision!")

	prop1:setColor ( 0.5 + r, b, 0, 0 )
	prop2:setColor ( r, 0.5+g, 0.25+b, 0 )
	
	ease1:stop()
	prop1:setLoc ( -128, -128 )
	ease1 = prop1:moveLoc ( 256, 256, 5 , MOAIEaseType.LINEAR)
	
	r = r + 0.2
	g = g + 0.2
	b = b + 0.2
	
end

prop1:setBounds(-16, -16, 0, 16, 16, 0)

collisionSet = MOAICollisionSet.new ()
collisionSet:setCollisionHandler(collisionHandler)
collisionSet:insertDynamicProp(prop1)
collisionSet:insertDynamicProp(prop2)
collisionSet:start()
