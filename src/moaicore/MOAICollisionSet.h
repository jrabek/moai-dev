// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICOLLISIONSET_H
#define	MOAICOLLISIONSET_H

#include <moaicore/MOAILua.h>
#include <moaicore/MOAIAction.h>

//================================================================//
// MOAICollisionSet
//================================================================//
/**	@name	MOAICollisionSet
	@text	Class for detecting overlap of MOAIProps
	
*/
class MOAICollisionSet :
	public MOAIAction {
private:

	//TODO: Following line may not be needed
	friend class MOAIProp;

	MOAILuaLocal	mCollisionHandler;

	typedef USLeanList < MOAIProp* >::Iterator PropIt;
	USLeanList < MOAIProp* > mProps;

	bool mDebugDrawEnabled;

	//----------------------------------------------------------------//
	static int		_clear					( lua_State* L );
	static int		_insertProp				( lua_State* L );
	static int		_removeProp				( lua_State* L );
	static int		_setCollisionHandler	( lua_State* L );
	static int		_setDebugDrawEnabled	( lua_State* L );

	u32 GetCollisionBounds(MOAIProp *prop, USRect &boundRect);

public:
	
	DECL_LUA_FACTORY ( MOAICollisionSet )
	
	//----------------------------------------------------------------//
	void			Clear					();
	void			InsertProp				( MOAIProp& prop );
					MOAICollisionSet			();
					~MOAICollisionSet			();
	void			OnUpdate				( float step );
	void			RegisterLuaClass		( MOAILuaState& state );
	void			RegisterLuaFuncs		( MOAILuaState& state );
	void			RemoveProp				( MOAIProp& prop );
	bool			IsDone 					();
	void			DrawDebug				();

};

#endif
