// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moaicore/MOAILogMessages.h>
#include <moaicore/MOAICollisionSet.h>
#include <moaicore/MOAIProp.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@name	clear
	@text	Remove all props from the set.
	
	@in		MOAICollisionSet self
	@out	nil
*/
int MOAICollisionSet::_clear ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionSet, "U" )

	self->Clear ();
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	insertProp
	@text	Inserts a prop into the collision set.
	
	@in		MOAIPartition self
	@in		MOAIProp prop
	@out	nil
*/
int MOAICollisionSet::_insertProp ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionSet, "UU" )

	MOAIProp* prop = state.GetLuaObject < MOAIProp >( 2, true );
	if ( !prop ) return 0;

	self->InsertProp ( *prop );
	prop->ScheduleUpdate ();

	return 0;
}

//----------------------------------------------------------------//
/**	@name	removeProp
	@text	Removes a prop from the collision set.
	
	@in		MOAICollisionSet self
	@in		MOAIProp prop
	@out	nil
*/
int MOAICollisionSet::_removeProp ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionSet, "UU" )

	MOAIProp* prop = state.GetLuaObject < MOAIProp >( 2, true );
	if ( !prop ) return 0;

	self->RemoveProp ( *prop );
	prop->ScheduleUpdate ();

	return 0;
}

int MOAICollisionSet::_setCollisionHandler	( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionSet, "UF" )

	self->SetLocal ( state, 2, self->mCollisionHandler );

	return 0;
}

//----------------------------------------------------------------//
/**	@name	setDebugDrawEnabled
	@text	enable/disable debug drawing.

	@in		MOAICollisionSet self
	@in		number bEnable
	@out	nil
*/
int MOAICollisionSet::_setDebugDrawEnabled ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionSet, "U" )

	self->mDebugDrawEnabled = state.GetValue < bool >( 2, false );

	return 0;
}

//================================================================//
// MOAICollisionSet
//================================================================//


//----------------------------------------------------------------//
void MOAICollisionSet::Clear () {
	PropIt propIt = this->mProps.Head ();
	while ( propIt ) {
		MOAIProp* prop = propIt->Data ();
		propIt = propIt->Next ();
		RemoveProp(*prop);
	}
}

//----------------------------------------------------------------//
void MOAICollisionSet::InsertProp ( MOAIProp& prop ) {

	if ( prop.mCollisionSet == this ) return;

	if ( prop.mCollisionSet ) {
		prop.mCollisionSet->RemoveProp ( prop );
	}

	this->mProps.PushBack ( prop.mLinkInCollisionSet );
	prop.mCollisionSet = this;
	
	this->LuaRetain ( &prop );
	
	prop.ScheduleUpdate ();
}

//----------------------------------------------------------------//
MOAICollisionSet::MOAICollisionSet () {
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIAction )
	RTTI_END
}

//----------------------------------------------------------------//
MOAICollisionSet::~MOAICollisionSet () {
	this->Clear ();
}

//----------------------------------------------------------------//
void MOAICollisionSet::RegisterLuaClass ( MOAILuaState& state ) {
	MOAIAction::RegisterLuaClass ( state );
}

//----------------------------------------------------------------//
void MOAICollisionSet::RegisterLuaFuncs ( MOAILuaState& state ) {
	
	MOAIAction::RegisterLuaFuncs ( state );

	luaL_Reg regTable [] = {
		{ "clear",						_clear },
		{ "insertProp",					_insertProp },
		{ "removeProp",					_removeProp },
		{ "setCollisionHandler",		_setCollisionHandler },
		{ "setDebugDrawEnabled",		_setDebugDrawEnabled },
		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAICollisionSet::RemoveProp ( MOAIProp& prop ) {

	if ( prop.mCollisionSet != this ) return;

	this->mProps.Remove ( prop.mLinkInCollisionSet );
	prop.mCollisionSet = 0;

	this->LuaRelease ( &prop );

}

//----------------------------------------------------------------//
void MOAICollisionSet::OnUpdate ( float step ) {
	// Currently an n^2 algorithm to check for collisions
	// For large numbers of props, need to implement n log n

	PropIt firstPropIt = this->mProps.Head ();

	// MOAIPrint("MOAICollisionSet::OnUpdate(%f)\n", step);

	while ( firstPropIt ) {
		PropIt secondPropIt = firstPropIt->Next ();
		MOAIProp* firstProp = firstPropIt->Data ();
		USBox firstPropBounds;

		firstPropBounds = firstProp->GetBounds ();

		firstPropIt = firstPropIt->Next ();

		if(firstPropBounds.IsPoint())
			continue;

		while ( secondPropIt ) {
			MOAIProp* secondProp = secondPropIt->Data ();
			USBox secondPropBounds;

			secondPropBounds  = secondProp->GetBounds ();

			secondPropIt = secondPropIt->Next ();

			if(secondPropBounds.IsPoint())
				continue;

/*
			MOAIPrint("Checking props. (%f, %f, %f, %f) and (%f, %f, %f, %f)\n",
					firstPropBounds.mMin.mX, firstPropBounds.mMin.mY,
					firstPropBounds.mMax.mX, firstPropBounds.mMax.mX,
					secondPropBounds.mMin.mX, secondPropBounds.mMin.mY,
					secondPropBounds.mMax.mX, secondPropBounds.mMax.mX);
*/

			/*	To handle rotated rectangular props, we need to do something like this (ugly?) */

			/*
				USAffine3D localtoWorld = firstProp->GetLocaltoWorldMtx ();
				GetPropBounds(firstPropBounds);
				USRect firstPropRect = firstPropBounds.getRect(PLANE_XY);
				localtoWorld.Transform ( firstPropRect );
				...same for second prop
				firstPropRect.Overlap(secondPropRect)
			 */

			/* To handle at a finer level of detail, CollsionShape would need to be used
			 * which would require full polygon collision checking
			 */

			if(firstPropBounds.Overlap(secondPropBounds))
			{
/*
				MOAIPrint("Overlapping props. (%f, %f, %f, %f) and (%f, %f, %f, %f)\n",
						firstPropBounds.mMin.mX, firstPropBounds.mMin.mY,
						firstPropBounds.mMax.mX, firstPropBounds.mMax.mX,
						secondPropBounds.mMin.mX, secondPropBounds.mMin.mY,
						secondPropBounds.mMax.mX, secondPropBounds.mMax.mX);
*/

				MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();
				if ( this->PushLocal ( state, this->mCollisionHandler )) {

					firstProp->PushLuaUserdata(state);
					secondProp->PushLuaUserdata(state);
					state.DebugCall ( 2, 0 );
				}
			}
		}
	}

}

//----------------------------------------------------------------//
bool MOAICollisionSet::IsDone () {
	return this->mProps.Count() == 0;
}

//----------------------------------------------------------------//
void MOAICollisionSet::DrawDebug() {

	if(this->mDebugDrawEnabled) {
		// prop->DrawDebug ( result->mSubPrimID );
		// TODO: need to keep track of the list of colliding props
	}

}
