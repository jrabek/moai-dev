// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moaicore/MOAILogMessages.h>
#include <moaicore/MOAICollisionSet.h>
#include <moaicore/MOAIProp.h>
#include <moaicore/MOAIGridSpace.h>
#include <moaicore/MOAIGrid.h>

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
/**	@name	insertStaticProp
	@text	Inserts a static prop into the collision set.
			Static props are not checked against each other.

	@in		MOAICollisionSet self
	@in		MOAIProp prop
	@out	nil
*/
int MOAICollisionSet::_insertStaticProp ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionSet, "UU" )

	MOAIProp* prop = state.GetLuaObject < MOAIProp >( 2, true );
	if ( !prop ) return 0;

	self->InsertStaticProp ( *prop );
	prop->ScheduleUpdate ();

	return 0;
}

//----------------------------------------------------------------//
/**	@name	insertDynamicProp
	@text	Inserts a dynamic prop into the collision set.
			Dynamic props are checked against static and dynamic props.
	
	@in		MOAICollisionSet self
	@in		MOAIProp prop
	@out	nil
*/
int MOAICollisionSet::_insertDynamicProp ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionSet, "UU" )

	MOAIProp* prop = state.GetLuaObject < MOAIProp >( 2, true );
	if ( !prop ) return 0;

	self->InsertDynamicProp ( *prop );
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
	PropIt propIt = this->mStaticProps.Head ();
	while ( propIt ) {
		MOAIProp* prop = propIt->Data ();
		propIt = propIt->Next ();
		this->RemoveProp(*prop);
	}

	propIt = this->mDynamicProps.Head ();
		while ( propIt ) {
			MOAIProp* prop = propIt->Data ();
			propIt = propIt->Next ();
			this->RemoveProp(*prop);
	}
}

//----------------------------------------------------------------//
void MOAICollisionSet::InsertStaticProp ( MOAIProp& prop ) {

	if ( prop.mCollisionSet == this ) return;

	if ( prop.mCollisionSet ) {
		prop.mCollisionSet->RemoveProp ( prop );
	}

	this->mStaticProps.PushBack ( prop.mLinkInCollisionSet );
	prop.mCollisionSet = this;

	this->LuaRetain ( &prop );

	prop.ScheduleUpdate ();
}

//----------------------------------------------------------------//
void MOAICollisionSet::InsertDynamicProp ( MOAIProp& prop ) {

	if ( prop.mCollisionSet == this ) return;

	if ( prop.mCollisionSet ) {
		prop.mCollisionSet->RemoveProp ( prop );
	}

	this->mDynamicProps.PushBack ( prop.mLinkInCollisionSet );
	prop.mCollisionSet = this;
	
	this->LuaRetain ( &prop );
	
	prop.ScheduleUpdate ();
}

//----------------------------------------------------------------//
MOAICollisionSet::MOAICollisionSet () :
	mDebugDrawEnabled(false)
{
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
		{ "insertStaticProp",			_insertStaticProp },
		{ "insertDynamicProp",			_insertDynamicProp },
		{ "removeProp",					_removeProp },
		{ "setCollisionHandler",		_setCollisionHandler },
		{ "setDebugDrawEnabled",		_setDebugDrawEnabled },
		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAICollisionSet::RemoveProp( MOAIProp& prop ) {

	if ( prop.mCollisionSet != this ) return;

	this->mStaticProps.Remove ( prop.mLinkInCollisionSet );
	this->mDynamicProps.Remove ( prop.mLinkInCollisionSet );
	prop.mCollisionSet = 0;

	this->LuaRelease ( &prop );

}

u32 MOAICollisionSet::GetCollisionBounds(MOAIProp *prop, USRect &boundRect)
{
	USAffine3D localtoWorld = prop->GetLocalToWorldMtx ();
	USBox bounds;
	u32 status = prop->GetPropBounds(bounds);
	if(status == MOAIProp::BOUNDS_GLOBAL || status == MOAIProp::BOUNDS_EMPTY)
		return status;

	boundRect = bounds.GetRect(USBox::PLANE_XY);
	localtoWorld.Transform ( boundRect );

	return status;
}

void MOAICollisionSet::ReportCollsion(MOAIProp *firstProp, MOAIProp *secondProp)
{
	if(this->mCollisionHandler) {
		MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();
		if ( this->PushLocal ( state, this->mCollisionHandler )) {

			firstProp->PushLuaUserdata(state);
			secondProp->PushLuaUserdata(state);
			state.DebugCall ( 2, 0 );
		}
	}
}

void MOAICollisionSet::CheckPropAgainstList(MOAIProp *firstProp, PropIt secondPropIt ) {

	USRect firstPropBounds;

	if(MOAIProp::BOUNDS_OK != GetCollisionBounds(firstProp, firstPropBounds))
		return;

	if(firstPropBounds.Area() == 0)
		return;

	while ( secondPropIt ) {
		MOAIProp* secondProp = secondPropIt->Data ();

		USRect secondPropBounds;

		secondPropIt = secondPropIt->Next ();

		secondProp->ForceUpdate();

		if(MOAIProp::BOUNDS_OK != GetCollisionBounds(secondProp, secondPropBounds))
			continue;

		if(secondPropBounds.Area() == 0)
			continue;

		MOAIGrid *propGrid = secondProp->mGrid;


		if( !propGrid ) {
			/*
			MOAIPrint("Checking props. (%f, %f, %f, %f) and (%f, %f, %f, %f)\n",
					firstPropBounds.mXMin, firstPropBounds.mYMin,
					firstPropBounds.mXMax, firstPropBounds.mYMax,
					secondPropBounds.mXMin, secondPropBounds.mYMin,
					secondPropBounds.mXMax, secondPropBounds.mYMax);
			*/

			/* TODO: To handle at a finer level of detail, CollsionShape would need to be used
			 * which would require full polygon collision checking
			 */

			if(firstPropBounds.Overlap(secondPropBounds))
			{
				/*
				MOAIPrint("Overlapping props. (%f, %f, %f, %f) and (%f, %f, %f, %f)\n",
						firstPropBounds.mXMin, firstPropBounds.mYMin,
						firstPropBounds.mXMax, firstPropBounds.mYMax,
						secondPropBounds.mXMin, secondPropBounds.mYMin,
						secondPropBounds.mXMax, secondPropBounds.mYMax);
				*/
				ReportCollsion(firstProp, secondProp);

			}
		} else {
			for(int x = 0; x < propGrid->GetWidth(); x++) {
				for(int y = 0; y < propGrid->GetWidth(); y++) {
					if(propGrid->GetTile(x, y)) {
						MOAICellCoord coord(x,y);
						secondPropBounds = propGrid->GetCellRect(coord);
						if(firstPropBounds.Overlap(secondPropBounds))
						{
							/*
							MOAIPrint("Overlapping props. (%f, %f, %f, %f) and (%f, %f, %f, %f)\n",
									firstPropBounds.mXMin, firstPropBounds.mYMin,
									firstPropBounds.mXMax, firstPropBounds.mYMax,
									secondPropBounds.mXMin, secondPropBounds.mYMin,
									secondPropBounds.mXMax, secondPropBounds.mYMax);
							*/
							ReportCollsion(firstProp, secondProp);

						}
					}
				}
			}

		}
	}
}

void MOAICollisionSet::ForceUpdate () {
	PropIt propIt = this->mDynamicProps.Head ();

	while ( propIt ) {
		MOAIProp* prop = propIt->Data ();
		propIt = propIt->Next ();
		prop->ForceUpdate();
	}
}

//----------------------------------------------------------------//
void MOAICollisionSet::OnUpdate ( float step ) {
	// Currently an n^2 algorithm to check for collisions
	// For large numbers of props, need to implement n log n

	// If we don't force an update the prop bounds could be wrong
	// leading to especially weird behavior (e.g. right after two props
	// are created in different parts of the screen they can collide
	// since their positions have not been updated)
	ForceUpdate();

	PropIt firstPropIt = this->mDynamicProps.Head ();

	while ( firstPropIt ) {
		PropIt secondPropIt = firstPropIt->Next ();
		MOAIProp* firstProp = firstPropIt->Data ();

		firstPropIt = firstPropIt->Next ();

		CheckPropAgainstList(firstProp, secondPropIt);

		PropIt staticPropIt = mStaticProps.Head ();

		CheckPropAgainstList(firstProp, staticPropIt);

	}

}

//----------------------------------------------------------------//
bool MOAICollisionSet::IsDone () {
	return this->mDynamicProps.Count() == 0;
}

//----------------------------------------------------------------//
void MOAICollisionSet::DrawDebug() {

	if(this->mDebugDrawEnabled) {
		// prop->DrawDebug ( result->mSubPrimID );
		// TODO: need to keep track of the list of colliding props
	}

}
