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
	USLeanList < MOAIProp* > mStaticProps;
	USLeanList < MOAIProp* > mDynamicProps;

	bool mDebugDrawEnabled;

	//----------------------------------------------------------------//
	static int		_clear					( lua_State* L );
	static int		_insertStaticProp		( lua_State* L );
	static int		_insertDynamicProp		( lua_State* L );
	static int		_removeProp				( lua_State* L );
	static int		_setCollisionHandler	( lua_State* L );
	static int		_setDebugDrawEnabled	( lua_State* L );

	u32				GetCollisionBounds		( MOAIProp *prop, USRect &boundRect );
	void			ReportCollsion			( MOAIProp *firstProp, MOAIProp *secondProp );
	void			CheckPropAgainstList	( MOAIProp *firstProp, PropIt secondPropIt );
	void			ForceUpdate				();
public:
	
	DECL_LUA_FACTORY ( MOAICollisionSet )
	
	//----------------------------------------------------------------//
	void			Clear					();
	void			InsertStaticProp		( MOAIProp& prop );
	void			InsertDynamicProp		( MOAIProp& prop );
					MOAICollisionSet		();
					~MOAICollisionSet		();
	void			OnUpdate				( float step );
	void			RegisterLuaClass		( MOAILuaState& state );
	void			RegisterLuaFuncs		( MOAILuaState& state );
	void			RemoveProp				( MOAIProp& prop );
	bool			IsDone 					();
	void			DrawDebug				();

};

#endif
