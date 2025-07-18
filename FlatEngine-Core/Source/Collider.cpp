#include "Collider.h"
#include "FlatEngine.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Transform.h"

#include <cmath>


namespace FlatEngine
{
	void BoxBoxRayCastCallback(RayCast* rayCast, Collider* collidedWith)
	{		
	}


	Collider::Collider(long myID, long parentID)
	{		
		SetID(myID);
		SetParentID(parentID);
		
		m_b_isComposite = false;
		m_activeOffset = Vector2(0, 0);
		m_previousPosition = Vector2(0, 0);
		m_centerGrid = Vector2(0, 0);
		m_previousGridStep = 0;
		m_nextCenterGrid = Vector2(0, 0);
		m_centerCoord = Vector2(0, 0);
		m_nextCenterCoord = Vector2(0, 0);
		m_rotation = 0;
		m_activeRadiusScreen = 0;
		m_activeRadiusGrid = 0;
		m_activeLayer = 0;
		m_b_isColliding = false;
		m_b_isContinuous = true;
		m_b_isStatic = false;
		m_b_isSolid = true;
		m_b_showActiveRadius = false;

		m_b_isCollidingRight = false;
		m_b_isCollidingLeft = false;
		m_b_isCollidingBottom = false;
		m_b_isCollidingTop = false;

		m_b_isCollidingTopRight = false;
		m_b_isCollidingTopLeft = false;
		m_b_isCollidingBottomRight = false;
		m_b_isCollidingBottomLeft = false;

		m_b_rightCollisionStatic = false;
		m_b_leftCollisionStatic = false;
		m_b_bottomCollisionStatic = false;
		m_b_topCollisionStatic = false;
		m_b_bottomLeftCollisionStatic = false;
		m_b_bottomRightCollisionStatic = false;

		m_b_rightCollisionSolid = false;
		m_b_leftCollisionSolid = false;
		m_b_bottomCollisionSolid = false;
		m_b_topCollisionSolid = false;
		m_b_bottomLeftCollisionSolid = false;
		m_b_bottomRightCollisionSolid = false;
		m_b_topLeftCollisionSolid = false;
		m_b_topRightCollisionSolid = false;

		m_rightCollision = 0;
		m_leftCollision = 0;
		m_bottomCollision = 0;
		m_topCollision = 0;

		m_leftCollidedPosition = Vector2(0,0);
		m_rightCollidedPosition = Vector2(0, 0);
		m_bottomCollidedPosition = Vector2(0, 0);
		m_topCollidedPosition = Vector2(0, 0);
		m_topRightCollidedPosition = Vector2(0, 0);
		m_bottomRightCollidedPosition = Vector2(0, 0);
		m_topLeftCollidedPosition = Vector2(0, 0);
		m_bottomLeftCollidedPosition = Vector2(0, 0);
	}

	Collider::~Collider()
	{
	}

	bool Collider::CheckForCollision(Collider* collider1, Collider* collider2)
	{
		bool b_colliding = false;
		Vector2 collider1Center = collider1->GetCenterGrid();
		Vector2 collider2Center = collider2->GetCenterGrid();		

		// Calculate center distance with pythag
		float rise = std::abs(collider1Center.y - collider2Center.y);
		float run = std::abs(collider1Center.x - collider2Center.x);
		float centerDistance = std::sqrt((rise * rise) + (run * run));

		// If they are close enough to check for collision ( actually colliding if they are both CircleColliders )
		if (centerDistance < collider1->GetActiveRadiusGrid() + collider2->GetActiveRadiusGrid())
		{
			// Both BoxColliders
			if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "BoxCollider")
			{
				BoxCollider* boxCol1 = static_cast<BoxCollider*>(collider1);
				BoxCollider* boxCol2 = static_cast<BoxCollider*>(collider2);

				b_colliding = CheckForCollisionBoxBoxSAT(boxCol1, boxCol2);
			}
			// First CircleCollider second BoxCollider
			else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "BoxCollider")
			{
				CircleCollider* circleCol = static_cast<CircleCollider*>(collider1);
				BoxCollider* boxCol = static_cast<BoxCollider*>(collider2);

				b_colliding = Collider::CheckForCollisionBoxCircle(boxCol, circleCol);
			}
			// First BoxCollider second CircleCollider
			else if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "CircleCollider")
			{
				BoxCollider* boxCol = static_cast<BoxCollider*>(collider1);
				CircleCollider* circleCol = static_cast<CircleCollider*>(collider2);

				b_colliding = Collider::CheckForCollisionBoxCircle(boxCol, circleCol);
			}
			// Both CircleColliders ( already true if made if past activeRadius check )
			else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "CircleCollider")
			{
				BoxCollider* circleCol1 = static_cast<BoxCollider*>(collider1);
				BoxCollider* circleCol2 = static_cast<BoxCollider*>(collider2);

				b_colliding = true;
			}
		}

		if (b_colliding)
		{			
			collider1->SetColliding(true);
			collider2->SetColliding(true);

			Vector2 box1Direction = collider1->GetParent()->GetRigidBody()->GetVelocity();
			Vector2 box2Direction = collider2->GetParent()->GetRigidBody()->GetVelocity();
			Vector2 box1CastDir = box1Direction - box2Direction;
			Vector2 box2CastDir = box2Direction - box1Direction;
			float nearestVertexDistance = 100;
			Vector2 box1HitPos = Vector2();
			Vector2 box2HitPos = Vector2();

			// Add colliding objects
			if (!collider1->m_b_isComposite)
			{
				if (collider1->AddCollidingObject(collider2) && collider1->GetType() == T_BoxCollider)
				{
					for (int i = 0; i < 4; i++)
					{
						Vector2 cornerCastFrom = static_cast<BoxCollider*>(collider1)->GetNextCorners()[i];

						RayCast rayCast = CastRay(cornerCastFrom, box1CastDir, 0.01f, 5, BoxBoxRayCastCallback, collider1->GetParentID());
						Vector2 collisionPoint = rayCast.GetPoint();
						Collider* collidedWith = rayCast.GetCollidedWith();

						if (collidedWith != nullptr && collidedWith->GetID() == collider2->GetID())
						{
							float distance = (collisionPoint - cornerCastFrom).GetMagnitude();
							if (distance < nearestVertexDistance)
							{
								nearestVertexDistance = distance;
								box1HitPos = cornerCastFrom;
								box2HitPos = collisionPoint;
							}
						}						
					}
				}


				// For Collider events - Fire OnActiveCollision while there is a collision happening
				if (collider1->GetType() == ComponentTypes::T_BoxCollider)
				{
					CallLuaCollisionFunction(collider1->GetParent(), collider2, LuaEventFunction::OnBoxCollision);
				}
				else if (collider1->GetType() == ComponentTypes::T_CircleCollider)
				{
					CallLuaCollisionFunction(collider1->GetParent(), collider2, LuaEventFunction::OnCircleCollision);
				}
			}
			else
			{
				CompositeCollider* compositeCollider = collider1->GetParent()->GetCompositeCollider();
				compositeCollider->AddCollidingObject(collider2);
				//compositeCollider->OnActiveCollision(collider1->GetParent(), collider2->GetParent());
			}

			if (!collider2->m_b_isComposite)
			{
				if (collider2->AddCollidingObject(collider1) && collider2->GetType() == T_BoxCollider)
				{		
					for (int i = 0; i < 4; i++)
					{
						Vector2 cornerCastFrom = static_cast<BoxCollider*>(collider2)->GetNextCorners()[i];

						RayCast rayCast = CastRay(cornerCastFrom, box2CastDir, 0.01f, 5, BoxBoxRayCastCallback, collider2->GetParentID());
						Vector2 collisionPoint = rayCast.GetPoint();
						Collider* collidedWith = rayCast.GetCollidedWith();
	
						if (collidedWith != nullptr && collidedWith->GetID() == collider1->GetID())
						{								
							float distance = (collisionPoint - cornerCastFrom).GetMagnitude();
							if (distance < nearestVertexDistance)
							{
								nearestVertexDistance = distance;
								box1HitPos = collisionPoint;
								box2HitPos = cornerCastFrom;
							}
						}
					}
				}

				// For Collider events - Fire OnActiveCollision while there is a collision happening
				if (collider2->GetType() == ComponentTypes::T_BoxCollider)
				{
					CallLuaCollisionFunction(collider2->GetParent(), collider1, LuaEventFunction::OnBoxCollision);
				}
				else if (collider2->GetType() == ComponentTypes::T_CircleCollider)
				{
					CallLuaCollisionFunction(collider2->GetParent(), collider1, LuaEventFunction::OnCircleCollision);
				}
			}
			else
			{
				CompositeCollider* compositeCollider = collider2->GetParent()->GetCompositeCollider();
				compositeCollider->AddCollidingObject(collider1);
				//compositeCollider->OnActiveCollision(collider2->GetParent(), collider1->GetParent());
			}

			{
				DrawLineInScene(box1HitPos, box2HitPos + Vector2(.1f, 0), GetColor("red"), 4);
				Vector2 normalToCollision = (collider1Center - collider2Center).Normalize();
				Vector2 projBox1DirOntoNormal = normalToCollision * ((normalToCollision.Dot(box1Direction)) / (normalToCollision.Dot(normalToCollision)));
				Vector2 flippedProjection = projBox1DirOntoNormal * (-1);
				Vector2 zHat = box1Direction - projBox1DirOntoNormal;
				Vector2 mirroredBox1Direction = flippedProjection + zHat;
				collider1->GetParent()->GetRigidBody()->SetPendingForces(Vector2(0,0));

				// Calculate actual force after impact (conervation of momentum)


				// Calculate torque and force... Right now only velocity of each object are the only thing considered, not rotational forces that could move
				// other objects.. box1Direction and box2Direction are velocities but we also need rotation speed so spinning objects can move other objects
				Vector2 forceDirection = mirroredBox1Direction * 3;
				Vector2 impactToCenter = (collider1->GetParent()->GetTransform()->GetTruePosition() - box1HitPos).Normalize();
				float forceInDirectionOfCenter = forceDirection.Dot(impactToCenter);
				float torque = forceDirection.GetMagnitude() - forceInDirectionOfCenter;
				float direction = 1;
				if (forceDirection.CrossKResult(impactToCenter) > 0)
				{
					direction = -1;
				}
				collider1->GetParent()->GetRigidBody()->AddTorque(torque, direction);
				collider1->GetParent()->GetRigidBody()->AddForce(impactToCenter, forceInDirectionOfCenter);
			}
			{
				// Calculate direction vector's reflection across the line of collision separation (where it should bounce to)
				Vector2 normalToCollision = (collider1Center - collider2Center).Normalize();
				Vector2 projBox2DirOntoNormal = normalToCollision * ((normalToCollision.Dot(box2Direction)) / (normalToCollision.Dot(normalToCollision)));
				Vector2 flippedProjection = projBox2DirOntoNormal * (-1);
				Vector2 zHat = box2Direction - projBox2DirOntoNormal;
				Vector2 mirroredBox2Direction = flippedProjection + zHat;
				collider2->GetParent()->GetRigidBody()->SetPendingForces(Vector2(0,0));
				//collider2->GetParent()->GetRigidBody()->AddForce

				// Calculate actual force after impact (conervation of momentum)


				// Calculate torque and force
				Vector2 forceDirection = mirroredBox2Direction * 3;
				Vector2 impactToCenter = (collider2->GetParent()->GetTransform()->GetTruePosition() - box2HitPos).Normalize();
				float forceInDirectionOfCenter = forceDirection.Dot(impactToCenter);
				float torque = forceDirection.GetMagnitude() - forceInDirectionOfCenter;
				float direction = 1;
				if (forceDirection.CrossKResult(impactToCenter) > 0)
				{
					direction = -1;
				}
				collider2->GetParent()->GetRigidBody()->AddTorque(torque, direction);
				collider2->GetParent()->GetRigidBody()->AddForce(impactToCenter, forceInDirectionOfCenter);
			}
		}

		return b_colliding;
	}

	bool Collider::IsPointProjectedInside(Vector2 starting, Vector2 ending, Vector2 point, bool& b_sameDirection, bool& b_oppositeDirection)
	{
		Vector2 insideVector = Vector2(ending.x - starting.x, ending.y - starting.y);
		Vector2 testVector = Vector2(point.x - starting.x, point.y - starting.y);
		float dotProduct = (insideVector.Dot(testVector)) / (insideVector.Dot(insideVector));
		Vector2 projectedVector = Vector2(dotProduct * insideVector.x, dotProduct * insideVector.y);

		if ((projectedVector.x * insideVector.x >= 0) && (projectedVector.y * insideVector.y >= 0))
		{
			b_sameDirection = true;
		}
		else
		{
			b_oppositeDirection = true;
		}

		float insideVectorMagnitude = std::sqrt((insideVector.x * insideVector.x) + (insideVector.y * insideVector.y));
		float projMagnitude = std::sqrt((projectedVector.x * projectedVector.x) + (projectedVector.y * projectedVector.y));

		return (projMagnitude <= insideVectorMagnitude && (projectedVector.x * insideVector.x >= 0) && (projectedVector.y * insideVector.y >= 0));
	}

	bool Collider::CheckForCollisionBoxCircle(BoxCollider* boxCol, CircleCollider* circleCol)
	{
		bool b_colliding = false;
		Vector2 circleCenterGrid = circleCol->GetNextCenterGrid(); // Get next center so no overlap happens this frame
		Vector2 boxCenterGrid = boxCol->GetNextCenterGrid();       // -- ^^^
		Vector2 circlePos = circleCol->GetParent()->GetTransform()->GetTruePosition();
		Vector2 boxPos = boxCol->GetParent()->GetTransform()->GetTruePosition();
		float boxHalfWidth = boxCol->GetActiveWidth() / 2;
		float boxHalfHeight = boxCol->GetActiveHeight() / 2;
		float circleActiveRadius = circleCol->GetActiveRadiusGrid();

		float A_TopEdge = circleCol->m_nextActiveTop;
		float A_RightEdge = circleCol->m_nextActiveRight;
		float A_BottomEdge = circleCol->m_nextActiveBottom;
		float A_LeftEdge = circleCol->m_nextActiveLeft;

		float B_TopEdge = boxCol->m_nextActiveTop;
		float B_RightEdge = boxCol->m_nextActiveRight;
		float B_BottomEdge = boxCol->m_nextActiveBottom;
		float B_LeftEdge = boxCol->m_nextActiveLeft;

		float yFromCol;
		float xFromCol;

		// Check how colliders are oriented in relation to each other //

		// check for direct left/right/top/bottom collisions
		// Circle left - Box right
		if (circleCenterGrid.x < boxCenterGrid.x && circleCenterGrid.y < B_TopEdge && circleCenterGrid.y > B_BottomEdge && (B_LeftEdge - circleCenterGrid.x < circleActiveRadius))
		{
			b_colliding = true;
			if (circleCol->IsSolid() && boxCol->IsSolid())
			{
				circleCol->m_b_isCollidingRight = true;
				circleCol->m_b_rightCollisionStatic = boxCol->IsStatic();
				circleCol->m_b_rightCollisionSolid = boxCol->IsSolid();
				boxCol->m_b_isCollidingLeft = true;
				boxCol->m_b_leftCollisionStatic = circleCol->IsStatic();
				boxCol->m_b_leftCollisionSolid = circleCol->IsSolid();
				circleCol->m_rightCollision = B_LeftEdge;
				boxCol->m_leftCollision = A_RightEdge;

				// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
				circleCol->m_rightCollidedPosition = Vector2(B_LeftEdge - circleActiveRadius + 0.001f, circlePos.y);
				boxCol->m_leftCollidedPosition = Vector2(A_RightEdge + boxHalfWidth - 0.001f, boxPos.y);
			}
		}
		// Circle right - Box left
		else if (circleCenterGrid.x > boxCenterGrid.x && circleCenterGrid.y < B_TopEdge && circleCenterGrid.y > B_BottomEdge && (circleCenterGrid.x - B_RightEdge < circleActiveRadius))
		{
			b_colliding = true;
			if (circleCol->IsSolid() && boxCol->IsSolid())
			{
				circleCol->m_b_isCollidingLeft = true;
				circleCol->m_b_leftCollisionStatic = boxCol->IsStatic();
				circleCol->m_b_leftCollisionSolid = boxCol->IsSolid();
				boxCol->m_b_isCollidingRight = true;
				boxCol->m_b_rightCollisionStatic = circleCol->IsStatic();
				boxCol->m_b_rightCollisionSolid = circleCol->IsSolid();
				circleCol->m_leftCollision = B_RightEdge;
				boxCol->m_rightCollision = A_LeftEdge;

				// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
				circleCol->m_leftCollidedPosition = Vector2(B_RightEdge + circleActiveRadius - 0.001f, circlePos.y);
				boxCol->m_rightCollidedPosition = Vector2(A_LeftEdge - boxHalfWidth + 0.001f, boxPos.y);
			}
		}
		// Circle Top - Box Bottom
		else if (circleCenterGrid.y > boxCenterGrid.y && circleCenterGrid.x < B_RightEdge && circleCenterGrid.x > B_LeftEdge && (circleCenterGrid.y - B_TopEdge < circleActiveRadius))
		{
			b_colliding = true;
			if (circleCol->IsSolid() && boxCol->IsSolid())
			{
				circleCol->m_b_isCollidingBottom = true;
				circleCol->m_b_bottomCollisionStatic = boxCol->IsStatic();
				circleCol->m_b_bottomCollisionSolid = boxCol->IsSolid();
				boxCol->m_b_isCollidingTop = true;
				boxCol->m_b_topCollisionStatic = circleCol->IsStatic();
				boxCol->m_b_topCollisionSolid = circleCol->IsSolid();
				circleCol->m_bottomCollision = B_TopEdge;
				boxCol->m_topCollision = A_BottomEdge;

				// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
				boxCol->m_topCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
				circleCol->m_bottomCollidedPosition = Vector2(circlePos.x, B_TopEdge + circleActiveRadius - 0.001f);
			}
		}
		// Circle Bottom - Box Top
		else if (circleCenterGrid.y < boxCenterGrid.y && circleCenterGrid.x < B_RightEdge && circleCenterGrid.x > B_LeftEdge && (B_BottomEdge - circleCenterGrid.y < circleActiveRadius))
		{
			b_colliding = true;
			if (circleCol->IsSolid() && boxCol->IsSolid())
			{
				circleCol->m_b_isCollidingTop = true;
				circleCol->m_b_topCollisionStatic = boxCol->IsStatic();
				circleCol->m_b_topCollisionSolid = boxCol->IsSolid();
				boxCol->m_b_isCollidingBottom = true;
				boxCol->m_b_bottomCollisionStatic = circleCol->IsStatic();
				boxCol->m_b_bottomCollisionSolid = circleCol->IsSolid();
				circleCol->m_topCollision = B_BottomEdge;
				boxCol->m_bottomCollision = A_TopEdge;

				// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
				boxCol->m_bottomCollidedPosition = Vector2(boxPos.x, A_TopEdge + boxHalfHeight - 0.001f);
				circleCol->m_topCollidedPosition = Vector2(circlePos.x, B_BottomEdge - circleActiveRadius + 0.001f);
			}
		}
		// Check for all other (corner) collisions
		// 
		// if circleCol is to the right of boxCol
		else if (circleCenterGrid.x > boxCenterGrid.x)
		{
			// if circleCol is below boxCol
			if (circleCenterGrid.y < boxCenterGrid.y)
			{
				// Calculate distance from corner with pythag
				float rise = std::abs(B_BottomEdge - circleCenterGrid.y);
				float run = std::abs(B_RightEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					b_colliding = true;
					if (circleCol->IsSolid() && boxCol->IsSolid())
					{
						float leftRightOverlap = B_RightEdge - A_LeftEdge;
						float topBottomOverlap = A_TopEdge - B_BottomEdge;

						// Circle is approaching from the right (keep y pos, calculate x pos)
						if (leftRightOverlap < topBottomOverlap)
						{
							circleCol->m_b_isCollidingLeft = true;
							circleCol->m_b_isCollidingTopLeft = true;
							circleCol->m_b_topLeftCollisionSolid = boxCol->IsSolid(); // new
							circleCol->m_b_leftCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_leftCollisionSolid = boxCol->IsSolid();
							boxCol->m_b_isCollidingRight = true;
							boxCol->m_b_rightCollisionStatic = circleCol->IsStatic();
							boxCol->m_b_rightCollisionSolid = boxCol->IsSolid();
							circleCol->m_leftCollision = B_RightEdge;
							boxCol->m_rightCollision = A_LeftEdge;

							// We know y and r, get x with pythag
							yFromCol = circleCenterGrid.y - B_BottomEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0) // no square roots of negatives
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							//boxCol->bottomRightCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
							circleCol->m_topLeftCollidedPosition = Vector2(B_RightEdge + xFromCol - 0.001f, circlePos.y);
						}
						// Circle is approaching from the bottom (keep x pos, calculate y pos)
						else {
							circleCol->m_b_isCollidingTop = true;
							circleCol->m_b_isCollidingTopLeft = true;
							circleCol->m_b_topLeftCollisionSolid = boxCol->IsSolid();
							circleCol->m_b_topCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_topCollisionSolid = boxCol->IsSolid();
							boxCol->m_b_isCollidingBottom = true;
							boxCol->m_b_isCollidingBottomRight = true;
							boxCol->m_b_bottomCollisionStatic = circleCol->IsStatic();
							boxCol->m_b_bottomCollisionSolid = circleCol->IsSolid();
							circleCol->m_topCollision = B_BottomEdge;
							boxCol->m_bottomCollision = A_TopEdge;

							// We know x and r, get y with pythag
							xFromCol = circleCenterGrid.x - B_RightEdge;

							if (-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								yFromCol = std::sqrt(-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								yFromCol = 0;

							//boxCol->bottomRightCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
							circleCol->m_topLeftCollidedPosition = Vector2(circlePos.x, B_BottomEdge - yFromCol - 0.001f);
						}
					}
				}
			}
			// if circleCol is above boxCol
			else if (circleCenterGrid.y > boxCenterGrid.y)
			{
				float rise = std::abs(B_TopEdge - circleCenterGrid.y);
				float run = std::abs(B_RightEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					b_colliding = true;
					if (circleCol->IsSolid() && boxCol->IsSolid())
					{
						float leftRightOverlap = B_RightEdge - A_LeftEdge;
						float topBottomOverlap = B_TopEdge - A_BottomEdge;

						// Circle is approaching from the right (keep y pos, calculate x pos)
						if (leftRightOverlap < topBottomOverlap)
						{
							circleCol->m_b_isCollidingLeft = true;
							circleCol->m_b_isCollidingBottomLeft = true;
							circleCol->m_b_bottomLeftCollisionSolid = boxCol->IsSolid();
							circleCol->m_b_leftCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_bottomLeftCollisionStatic = boxCol->IsStatic(); // New
							circleCol->m_b_leftCollisionSolid = boxCol->IsSolid();
							boxCol->m_b_isCollidingRight = true;
							boxCol->m_b_isCollidingTopRight = true;
							boxCol->m_b_rightCollisionStatic = circleCol->IsStatic();
							boxCol->m_b_rightCollisionSolid = circleCol->IsSolid();
							circleCol->m_leftCollision = B_RightEdge;
							boxCol->m_rightCollision = A_LeftEdge;

							yFromCol = circleCenterGrid.y - B_TopEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							//FlatGui::PushWindowStyles();
							//ImGui::Begin("Scene View", 0, 16 | 8);
							//FlatGui::PopWindowStyles();
							//DrawLine(Vector2(circleCol->GetCenterCoord().x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector2(circleCol->GetCenterCoord().x + (xFromCol - (circlePos.x - B_RightEdge)) * FlatGui::sceneViewGridStep.x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector4(0, 0, 0, 1), 1, ImGui::GetWindowDrawList());
							//DrawLine(circleCol->GetCenterCoord(), Vector2(circleCol->GetCenterCoord().x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector4(0, 0, 0, 1), 1, ImGui::GetWindowDrawList()); // Vertical
							//ImGui::End();

							//boxCol->topRightCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
							/*circleCol->bottomLeftCollidedPosition = Vector2(circlePos.x, B_TopEdge + yFromCol - 0.001f);*/
							circleCol->m_bottomLeftCollidedPosition = Vector2(circleCenterGrid.x + (xFromCol - (circleCenterGrid.x - B_RightEdge)), circleCenterGrid.y);
						}
						// Circle is approaching from the top (keep x value, calculate y value)
						else {
							circleCol->m_b_isCollidingBottom = true;
							circleCol->m_b_isCollidingBottomLeft = true;
							circleCol->m_b_bottomCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_bottomLeftCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_bottomLeftCollisionSolid = boxCol->IsSolid();
							circleCol->m_b_bottomCollisionSolid = boxCol->IsSolid();
							boxCol->m_b_isCollidingTop = true;
							boxCol->m_b_topCollisionStatic = circleCol->IsStatic();
							boxCol->m_b_topCollisionSolid = circleCol->IsSolid();
							circleCol->m_bottomCollision = B_TopEdge;
							boxCol->m_topCollision = A_BottomEdge;

							yFromCol = circleCenterGrid.y - B_TopEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							/*FlatGui::PushWindowStyles();
							ImGui::Begin("Scene View", 0, 16 | 8);
							FlatGui::PopWindowStyles();
							DrawLine(Vector2(circleCol->GetCenterCoord().x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector2(circleCol->GetCenterCoord().x + (xFromCol - (circlePos.x - B_RightEdge)) * FlatGui::sceneViewGridStep.x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector4(0, 0, 0, 1), 1, ImGui::GetWindowDrawList());
							DrawLine(circleCol->GetCenterCoord(), Vector2(circleCol->GetCenterCoord().x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector4(0, 0, 0, 1), 1, ImGui::GetWindowDrawList());*/ // Vertical
							//ImGui::End();

							//boxCol->topRightCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
							/*circleCol->bottomLeftCollidedPosition = Vector2(circlePos.x, B_TopEdge + yFromCol - 0.001f);*/
							circleCol->m_bottomLeftCollidedPosition = Vector2(circleCenterGrid.x + (xFromCol - (circleCenterGrid.x - B_RightEdge)), circleCenterGrid.y);
						}
					}
				}
			}
		}
		//LogFloat(xFromCol, "Final X From Coll: ");
		// if circleCol is to the left of boxCol
		else if (circleCenterGrid.x < boxCenterGrid.x)
		{
			// if circleCol is below boxCol
			if (circleCenterGrid.y < boxCenterGrid.y)
			{
				float rise = std::abs(B_BottomEdge - circleCenterGrid.y);
				float run = std::abs(B_LeftEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					b_colliding = true;
					if (circleCol->IsSolid() && boxCol->IsSolid())
					{
						float leftRightOverlap = A_RightEdge - B_LeftEdge;
						float topBottomOverlap = A_TopEdge - B_BottomEdge;

						// Circle is approaching from the left (keep y value, calculate x value)
						if (leftRightOverlap < topBottomOverlap)
						{
							circleCol->m_b_isCollidingRight = true;
							circleCol->m_b_isCollidingTopRight = true;
							circleCol->m_b_topRightCollisionSolid = boxCol->IsSolid(); // new
							circleCol->m_b_rightCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_rightCollisionSolid = boxCol->IsSolid();
							boxCol->m_b_isCollidingLeft = true;
							boxCol->m_b_leftCollisionStatic = circleCol->IsStatic();
							boxCol->m_b_leftCollisionSolid = circleCol->IsSolid();
							circleCol->m_rightCollision = B_LeftEdge;
							boxCol->m_leftCollision = A_RightEdge;

							yFromCol = circleCenterGrid.y - B_BottomEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							circleCol->m_topRightCollidedPosition = Vector2(B_LeftEdge - xFromCol - 0.001f, circlePos.y);
						}
						// Circle is approaching from the bottom (keep x value, calculate y value)
						else {
							circleCol->m_b_isCollidingTop = true;
							circleCol->m_b_isCollidingTopRight = true;
							circleCol->m_b_topRightCollisionSolid = boxCol->IsSolid();
							circleCol->m_b_topCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_topCollisionSolid = boxCol->IsSolid();
							boxCol->m_b_isCollidingBottom = true;
							boxCol->m_b_bottomCollisionStatic = circleCol->IsStatic();
							boxCol->m_b_bottomCollisionSolid = circleCol->IsSolid();
							circleCol->m_topCollision = B_BottomEdge;
							boxCol->m_bottomCollision = A_TopEdge;

							xFromCol = B_LeftEdge - circleCenterGrid.x;

							if (-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								yFromCol = std::sqrt(-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								yFromCol = 0;

							circleCol->m_topRightCollidedPosition = Vector2(circlePos.x, B_BottomEdge - yFromCol - 0.001f);
						}
					}
				}
			}
			// if circleCol is above boxCol
			else if (circleCenterGrid.y > boxCenterGrid.y)
			{
				float rise = std::abs(B_TopEdge - circleCenterGrid.y);
				float run = std::abs(B_LeftEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					b_colliding = true;
					if (circleCol->IsSolid() && boxCol->IsSolid())
					{
						float leftRightOverlap = A_RightEdge - B_LeftEdge;
						float topBottomOverlap = B_TopEdge - A_BottomEdge;

						// Circle is approaching from the left (keep y value, calculate x value)
						if (leftRightOverlap < topBottomOverlap)
						{
							circleCol->m_b_isCollidingRight = true;
							circleCol->m_b_isCollidingBottomRight = true;
							circleCol->m_b_bottomRightCollisionSolid = boxCol->IsSolid(); // new
							circleCol->m_b_rightCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_rightCollisionSolid = boxCol->IsSolid();
							boxCol->m_b_isCollidingLeft = true;
							boxCol->m_b_isCollidingTopLeft = true;
							boxCol->m_b_leftCollisionStatic = circleCol->IsStatic();
							boxCol->m_b_leftCollisionSolid = circleCol->IsSolid();
							circleCol->m_rightCollision = B_LeftEdge;
							boxCol->m_leftCollision = A_RightEdge;

							yFromCol = circleCenterGrid.y - B_TopEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							circleCol->m_bottomRightCollidedPosition = Vector2(B_LeftEdge - xFromCol - 0.001f, circlePos.y);
						}
						// Circle is approaching from the top (keep x value, calculate y value)
						else {
							circleCol->m_b_isCollidingBottom = true;
							circleCol->m_b_isCollidingBottomRight = true;
							circleCol->m_b_bottomRightCollisionSolid = boxCol->IsSolid();
							circleCol->m_b_bottomCollisionStatic = boxCol->IsStatic();
							circleCol->m_b_bottomCollisionSolid = boxCol->IsSolid();
							boxCol->m_b_isCollidingTop = true;
							boxCol->m_b_isCollidingTopLeft = true;
							boxCol->m_b_topCollisionStatic = circleCol->IsStatic();
							boxCol->m_b_topCollisionSolid = circleCol->IsSolid();
							circleCol->m_bottomCollision = B_TopEdge;
							boxCol->m_topCollision = A_BottomEdge;

							xFromCol = B_LeftEdge - circleCenterGrid.x;

							if (-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								yFromCol = std::sqrt(-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								yFromCol = 0;

							circleCol->m_bottomRightCollidedPosition = Vector2(circlePos.x, B_TopEdge + yFromCol - 0.001f);
						}
					}
				}
			}
		}

		return b_colliding;
	}

	bool Collider::CheckForCollisionBoxBox(BoxCollider* boxCol1, BoxCollider* boxCol2)
	{
		bool b_colliding = false;
		Transform* box1Transform = boxCol1->GetParent()->GetTransform();
		Transform* box2Transform = boxCol2->GetParent()->GetTransform();
		Vector2 box1Scale = box1Transform->GetScale();
		Vector2 box2Scale = box2Transform->GetScale();
		Vector2 collider1CenterGrid = boxCol1->GetCenterGrid();
		Vector2 collider2CenterGrid = boxCol2->GetCenterGrid();
		Vector2 col1Pos = boxCol1->GetParent()->GetTransform()->GetPosition();
		Vector2 col2Pos = boxCol2->GetParent()->GetTransform()->GetPosition();
		Vector2 col1Offset = boxCol1->GetActiveOffset();
		Vector2 col2Offset = boxCol2->GetActiveOffset();
		float box1HalfHeight = boxCol1->GetActiveHeight() / 2 * box1Scale.y;
		float box1HalfWidth = boxCol1->GetActiveWidth() / 2 * box1Scale.x;
		float box2HalfHeight = boxCol2->GetActiveHeight() / 2 * box2Scale.y;
		float box2HalfWidth = boxCol2->GetActiveWidth() / 2 * box2Scale.x;

		float A_TopEdge = boxCol1->m_nextActiveTop;
		float A_RightEdge = boxCol1->m_nextActiveRight;
		float A_BottomEdge = boxCol1->m_nextActiveBottom;
		float A_LeftEdge = boxCol1->m_nextActiveLeft;

		float B_TopEdge = boxCol2->m_nextActiveTop;
		float B_RightEdge = boxCol2->m_nextActiveRight;
		float B_BottomEdge = boxCol2->m_nextActiveBottom;
		float B_LeftEdge = boxCol2->m_nextActiveLeft;

		// Check for collision
		b_colliding = ((A_LeftEdge < B_RightEdge) && (A_RightEdge > B_LeftEdge) && (A_BottomEdge < B_TopEdge) && (A_TopEdge > B_BottomEdge));

		// Get collision details
		if (b_colliding)
		{
			// Check which direction the collision is happening from //
			// 
			// if boxCol1 is to the right of boxCol2
			if (collider1CenterGrid.x > collider2CenterGrid.x)
			{
				// if boxCol1 is below boxCol2
				if (collider1CenterGrid.y < collider2CenterGrid.y)
				{
					float leftRightOverlap = B_RightEdge - A_LeftEdge;
					float topBottomOverlap = A_TopEdge - B_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingLeft = true;
							boxCol1->m_b_leftCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_leftCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingRight = true;
							boxCol2->m_b_rightCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_rightCollisionSolid = boxCol2->IsSolid();
							boxCol1->m_leftCollision = B_RightEdge;
							boxCol2->m_rightCollision = A_LeftEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth - col1Offset.x - 0.001f, col1Pos.y);
							boxCol2->m_rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth - col2Offset.x + 0.001f, col2Pos.y);
						}
					}
					// Top/Bottom
					else {
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingTop = true;
							boxCol1->m_b_topCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_topCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingBottom = true;
							boxCol2->m_b_bottomCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_bottomCollisionSolid = boxCol1->IsSolid();
							boxCol1->m_topCollision = B_BottomEdge;
							boxCol2->m_bottomCollision = A_TopEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight - col1Offset.y + 0.001f);
							boxCol2->m_bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - col2Offset.y - 0.001f);
						}
					}
				}
				// if boxCol1 is above boxCol2
				else if (collider1CenterGrid.y > collider2CenterGrid.y)
				{
					float leftRightOverlap = B_RightEdge - A_LeftEdge;
					float topBottomOverlap = B_TopEdge - A_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingLeft = true;
							boxCol1->m_b_leftCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_leftCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingRight = true;
							boxCol2->m_b_rightCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_rightCollisionSolid = boxCol1->IsSolid();
							boxCol1->m_leftCollision = B_RightEdge;
							boxCol2->m_rightCollision = A_LeftEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth - col1Offset.x - 0.001f, col1Pos.y);
							boxCol2->m_rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth - col2Offset.x + 0.001f, col2Pos.y);
						}
					}
					// Top/Bottom
					else {
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingBottom = true;
							boxCol1->m_b_bottomCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_bottomCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingTop = true;
							boxCol2->m_b_topCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_topCollisionSolid = boxCol1->IsSolid();
							boxCol1->m_bottomCollision = B_TopEdge;
							boxCol2->m_topCollision = A_BottomEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - col1Offset.y - 0.001f);
							boxCol2->m_topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight - col2Offset.y + 0.001f);
						}
					}
				}
				// if both are at the same y Pos the it's a left/right collision
				else
				{
					if (boxCol1->IsSolid() && boxCol2->IsSolid())
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingLeft = true;
							boxCol1->m_b_leftCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_leftCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingRight = true;
							boxCol2->m_b_rightCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_rightCollisionSolid = boxCol1->IsSolid();
							boxCol1->m_leftCollision = B_RightEdge;
							boxCol2->m_rightCollision = A_LeftEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth + col1Offset.x - 0.001f, col1Pos.y);
							boxCol2->m_rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth - col2Offset.x + 0.001f, col2Pos.y);
						}
					}
				}
			}
			// if boxCol1 is to the left of boxCol2
			else if (collider1CenterGrid.x < collider2CenterGrid.x)
			{
				// if boxCol1 is below boxCol2
				if (collider1CenterGrid.y < collider2CenterGrid.y)
				{
					float leftRightOverlap = A_RightEdge - B_LeftEdge;
					float topBottomOverlap = A_TopEdge - B_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingRight = true;
							boxCol1->m_b_rightCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_rightCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingLeft = true;
							boxCol2->m_b_leftCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_leftCollisionSolid = boxCol1->IsSolid();
							boxCol1->m_rightCollision = B_LeftEdge;
							boxCol2->m_leftCollision = A_RightEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth - col1Offset.x + 0.001f, col1Pos.y);
							boxCol2->m_leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - col2Offset.x - 0.001f, col2Pos.y);
						}
					}
					// Top/Bottom
					else {
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingTop = true;
							boxCol1->m_b_topCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_topCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingBottom = true;
							boxCol2->m_b_bottomCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_bottomCollisionSolid = boxCol1->IsSolid();
							boxCol1->m_topCollision = B_BottomEdge;
							boxCol2->m_bottomCollision = A_TopEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight - col1Offset.y + 0.001f);
							boxCol2->m_bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - col2Offset.y - 0.001f);
						}
					}
				}
				else if (collider1CenterGrid.y > collider2CenterGrid.y)
				{
					float leftRightOverlap = A_RightEdge - B_LeftEdge;
					float topBottomOverlap = B_TopEdge - A_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingRight = true;
							boxCol1->m_b_rightCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_rightCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingLeft = true;
							boxCol2->m_b_leftCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_leftCollisionSolid = boxCol1->IsSolid();
							boxCol1->m_rightCollision = B_LeftEdge;
							boxCol2->m_leftCollision = A_RightEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth - col1Offset.x + 0.001f, col1Pos.y);
							boxCol2->m_leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - col2Offset.x - 0.001f, col2Pos.y);
						}
					}
					// Top/Bottom
					else {
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->m_b_isCollidingBottom = true;
							boxCol1->m_b_bottomCollisionStatic = boxCol2->IsStatic();
							boxCol1->m_b_bottomCollisionSolid = boxCol2->IsSolid();
							boxCol2->m_b_isCollidingTop = true;
							boxCol2->m_b_topCollisionStatic = boxCol1->IsStatic();
							boxCol2->m_b_topCollisionSolid = boxCol1->IsSolid();
							boxCol1->m_bottomCollision = B_TopEdge;
							boxCol2->m_topCollision = A_BottomEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->m_bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - col1Offset.y - 0.001f);
							boxCol2->m_topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight - col2Offset.y + 0.001f);
						}
					}
				}
				// if both are at the same y Pos the it's a left/right collision
				else
				{
					if (boxCol1->IsSolid() && boxCol2->IsSolid())
					{
						boxCol1->m_b_isCollidingRight = true;
						boxCol1->m_b_rightCollisionStatic = boxCol2->IsStatic();
						boxCol1->m_b_rightCollisionSolid = boxCol2->IsSolid();
						boxCol2->m_b_isCollidingLeft = true;
						boxCol2->m_b_leftCollisionStatic = boxCol1->IsStatic();
						boxCol2->m_b_leftCollisionSolid = boxCol1->IsSolid();
						boxCol1->m_rightCollision = B_LeftEdge;
						boxCol2->m_leftCollision = A_RightEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->m_rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth - col1Offset.x + 0.001f, col1Pos.y);
						boxCol2->m_leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - col2Offset.x - 0.001f, col2Pos.y);
					}
				}
			}
			// Both boxes are at the same x Pos
			else
			{
				// if boxCol1 is below boxCol2
				if (collider1CenterGrid.y < collider2CenterGrid.y)
				{
					if (boxCol1->IsSolid() && boxCol2->IsSolid())
					{
						boxCol1->m_b_isCollidingTop = true;
						boxCol1->m_b_topCollisionStatic = boxCol2->IsStatic();
						boxCol1->m_b_topCollisionSolid = boxCol2->IsSolid();
						boxCol2->m_b_isCollidingBottom = true;
						boxCol2->m_b_bottomCollisionStatic = boxCol1->IsStatic();
						boxCol2->m_b_bottomCollisionSolid = boxCol1->IsSolid();
						boxCol1->m_topCollision = B_BottomEdge;
						boxCol2->m_bottomCollision = A_TopEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->m_topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight - col1Offset.y + 0.001f);
						boxCol2->m_bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - col2Offset.y - 0.001f);
					}
				}
				// if boxCol1 is above boxCol2
				else
				{
					if (boxCol1->IsSolid() && boxCol2->IsSolid())
					{
						boxCol1->m_b_isCollidingBottom = true;
						boxCol1->m_b_bottomCollisionStatic = boxCol2->IsStatic();
						boxCol1->m_b_bottomCollisionSolid = boxCol2->IsSolid();
						boxCol2->m_b_isCollidingTop = true;
						boxCol2->m_b_topCollisionStatic = boxCol1->IsStatic();
						boxCol2->m_b_topCollisionSolid = boxCol1->IsSolid();
						boxCol1->m_bottomCollision = B_TopEdge;
						boxCol2->m_topCollision = A_BottomEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->m_bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - col1Offset.y - 0.001f);
						boxCol2->m_topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight - col2Offset.y + 0.001f);
					}
				}
			}
		}

		return b_colliding;
	}

	bool Collider::CheckForCollisionBoxBoxSAT(BoxCollider* boxCol1, BoxCollider* boxCol2)
	{
		int insideCounter = 0;
		bool b_noneInBox1 = false;
		bool b_noneInBox2 = false;
		bool b_sameDirection = false;
		bool b_oppositeDirection = false;

		Vector2* box1Corners = boxCol1->GetNextCorners();
		Vector2* box2Corners = boxCol2->GetNextCorners();

		for (int i = 0; i < 4; i++)
		{
			if (IsPointProjectedInside(box1Corners[0], box1Corners[1], box2Corners[i], b_sameDirection, b_oppositeDirection))
			{
				insideCounter++;
			}
		}
		if (b_sameDirection && b_oppositeDirection)
		{
			insideCounter++;
		}
		if (insideCounter == 0)
		{
			b_noneInBox1 = true;
		}
		insideCounter = 0;
		b_sameDirection = false;
		b_oppositeDirection = false;

		if (!b_noneInBox1)
		{
			for (int i = 0; i < 4; i++)
			{
				if (IsPointProjectedInside(box1Corners[1], box1Corners[2], box2Corners[i], b_sameDirection, b_oppositeDirection))
				{
					insideCounter++;
				}
			}
			if (b_sameDirection && b_oppositeDirection)
			{
				insideCounter++;
			}
			if (insideCounter == 0)
			{
				b_noneInBox1 = true;
			}

			insideCounter = 0;
			b_sameDirection = false;
			b_oppositeDirection = false;
		}

		for (int i = 0; i < 4; i++)
		{
			if (IsPointProjectedInside(box2Corners[0], box2Corners[1], box1Corners[i], b_sameDirection, b_oppositeDirection))
			{
				insideCounter++;
			}
		}
		if (b_sameDirection && b_oppositeDirection)
		{
			insideCounter++;			
		}
		if (insideCounter == 0)
		{
			b_noneInBox2 = true;
		}
		insideCounter = 0;
		b_sameDirection = false;
		b_oppositeDirection = false;

		if (!b_noneInBox2)
		{
			for (int i = 0; i < 4; i++)
			{
				if (IsPointProjectedInside(box2Corners[1], box2Corners[2], box1Corners[i], b_sameDirection, b_oppositeDirection))
				{
					insideCounter++;
				}
			}
			if (b_sameDirection && b_oppositeDirection)
			{
				insideCounter++;
			}
			if (insideCounter == 0)
			{
				b_noneInBox2 = true;
			}
		}

		return !(b_noneInBox1 || b_noneInBox2);
	}

	bool Collider::CheckForCollisionBoxRayCastSAT(BoxCollider* boxCol, RayCast* rayCast)
	{
		bool b_colliding = true;
		bool b_sameDirection = false;
		bool b_oppositeDirection = false;

		Vector2* boxCorners = boxCol->GetCorners();
		Vector2 castPoint = rayCast->GetPoint();

		if (!IsPointProjectedInside(boxCorners[0], boxCorners[1], castPoint, b_sameDirection, b_oppositeDirection))
		{
			b_colliding = false;
		}
		if (!IsPointProjectedInside(boxCorners[1], boxCorners[2], castPoint, b_sameDirection, b_oppositeDirection))
		{
			b_colliding = false;
		}

		if (b_colliding)
		{
			rayCast->OnHit(boxCol);
		}

		return b_colliding;
	}

	bool Collider::IsColliding()
	{
		return m_b_isColliding;
	}

	void Collider::SetColliding(bool b_isColliding)
	{
		m_b_isColliding = b_isColliding;
	}

	void Collider::UpdatePreviousPosition()
	{
		Transform* transform = GetParent()->GetTransform();
		m_previousPosition = transform->GetTruePosition();
	}

	Vector2 Collider::GetPreviousPosition()
	{
		return m_previousPosition;
	}

	void Collider::SetPreviousPosition(Vector2 prevPos)
	{
		m_previousPosition = prevPos;
	}

	Vector2 Collider::GetPreviousCenterPoint()
	{
		return m_previousCenterPoint;
	}

	void Collider::SetPreviousCenterPoint(Vector2 centerPoint)
	{
		m_previousCenterPoint = centerPoint;
	}

	float Collider::GetPreviousGridStep()
	{
		return m_previousGridStep;
	}

	void Collider::SetPreviousGridStep(float prevGridStep)
	{
		m_previousGridStep = prevGridStep;
	}

	bool Collider::HasMoved()
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetPosition();

		return (m_previousPosition.x != position.x || m_previousPosition.y != position.y);
	}

	// Returns true if successfully added for the first time
	bool Collider::AddCollidingObject(Collider* collidedWith)
	{		
		for (GameObject* object : m_collidingObjects)
		{	
			if (object->GetID() == collidedWith->GetParent()->GetID())
			{
				return false;
			}
		}		
		m_collidingObjects.push_back(collidedWith->GetParent());

		for (GameObject *object : m_collidingLastFrame)
		{
			// Leave function if the object has already fired OnCollisionEnter() (in the lastframe);
			if (object->GetID() == collidedWith->GetParent()->GetID())
			{
				return false;
			}
		}

		// else fire it now. (upon initially adding the object to m_collidingObjects for the first time)
		if (static_cast<Component*>(this)->GetType() == ComponentTypes::T_BoxCollider)
		{			
			CallLuaCollisionFunction(GetParent(), collidedWith, LuaEventFunction::OnBoxCollisionEnter);
		}
		if (static_cast<Component*>(this)->GetType() == ComponentTypes::T_CircleCollider)
		{			
			CallLuaCollisionFunction(GetParent(), collidedWith, LuaEventFunction::OnCircleCollisionEnter);
		}

		return true;
	}

	std::vector<GameObject*> Collider::GetCollidingObjects()
	{
		return m_collidingObjects;
	}

	void Collider::ClearCollidingObjects()
	{
		// Check which objects have left collision state since last frame
		for (GameObject *collidedLastFrame : m_collidingLastFrame)
		{		
			bool b_objectStillColliding = false;

			for (GameObject *collidedThisFrame : m_collidingObjects)
			{
				if (collidedLastFrame->GetID() == collidedThisFrame->GetID())
				{
					b_objectStillColliding = true;
				}
			}

			// Fire OnLeave if not colliding
			if (!b_objectStillColliding)
			{
				for (BoxCollider* boxCollider : collidedLastFrame->GetBoxColliders())
				{
					CallLuaCollisionFunction(GetParent(), boxCollider, LuaEventFunction::OnBoxCollisionLeave);
				}
				for (CircleCollider* circleCollider : collidedLastFrame->GetCircleColliders())
				{
					CallLuaCollisionFunction(GetParent(), circleCollider, LuaEventFunction::OnCircleCollisionLeave);
				}
			}
		}

		// Save colliding objects for next frame
		m_collidingLastFrame = m_collidingObjects;
		m_collidingObjects.clear();
	}

	void Collider::SetActiveOffset(Vector2 offset)
	{
		m_activeOffset = offset;
	}

	void Collider::SetActiveLayer(int layer)
	{
		if (layer >= 0)
		{
			m_activeLayer = layer;
		}
		else
		{
			LogError("Collider active layer must be an integer greater than 0.");
		}
	}

	int Collider::GetActiveLayer()
	{
		return m_activeLayer;
	}

	void Collider::SetActiveRadiusScreen(float radius)
	{
		m_activeRadiusScreen = radius;
	}

	Vector2 Collider::GetActiveOffset()
	{
		return m_activeOffset;
	}

	float Collider::GetActiveRadiusScreen()
	{
		return m_activeRadiusScreen;
	}

	void Collider::SetActiveRadiusGrid(float radius)
	{
		m_activeRadiusGrid = radius;
	}

	float Collider::GetActiveRadiusGrid()
	{
		return m_activeRadiusGrid;
	}

	void  Collider::SetShowActiveRadius(bool b_showActiveRadius)
	{
		m_b_showActiveRadius = b_showActiveRadius;
	}

	bool  Collider::GetShowActiveRadius()
	{
		return m_b_showActiveRadius;
	}

	void Collider::ResetCollisions()
	{
		ClearCollidingObjects();
		SetColliding(false);

		m_b_isCollidingRight = false;
		m_b_isCollidingLeft = false;
		m_b_isCollidingBottom = false;
		m_b_isCollidingTop = false;

		m_b_isCollidingTopRight = false;
		m_b_isCollidingTopLeft = false;
		m_b_isCollidingBottomRight = false;
		m_b_isCollidingBottomLeft = false;

		m_b_rightCollisionStatic = false;
		m_b_leftCollisionStatic = false;
		m_b_bottomCollisionStatic = false;
		m_b_topCollisionStatic = false;
		m_b_bottomLeftCollisionStatic = false;
		m_b_bottomRightCollisionStatic = false;

		m_b_rightCollisionSolid = false;
		m_b_leftCollisionSolid = false;
		m_b_bottomCollisionSolid = false;
		m_b_topCollisionSolid = false;
		m_b_bottomLeftCollisionSolid = false;
		m_b_bottomRightCollisionSolid = false;
		m_b_topLeftCollisionSolid = false;
		m_b_topRightCollisionSolid = false;

		m_rightCollision = 0;
		m_leftCollision = 0;
		m_bottomCollision = 0;
		m_topCollision = 0;

		m_leftCollidedPosition = Vector2(0, 0);
		m_rightCollidedPosition = Vector2(0, 0);
		m_bottomCollidedPosition = Vector2(0, 0);
		m_topCollidedPosition = Vector2(0, 0);
		m_topRightCollidedPosition = Vector2(0, 0);
		m_bottomRightCollidedPosition = Vector2(0, 0);
		m_topLeftCollidedPosition = Vector2(0, 0);
		m_bottomLeftCollidedPosition = Vector2(0, 0);
	}

	void Collider::SetCenterGrid(Vector2 newCenter)
	{
		m_centerGrid = newCenter;
	}

	Vector2 Collider::GetCenterGrid()
	{
		return m_centerGrid;
	}

	void Collider::SetCenterCoord(Vector2 newCenter)
	{
		m_centerCoord = newCenter;
	}

	Vector2 Collider::GetCenterCoord()
	{
		return m_centerCoord;
	}

	void Collider::SetNextCenterGrid(Vector2 nextCenter)
	{
		m_nextCenterGrid = nextCenter;
	}

	Vector2 Collider::GetNextCenterGrid()
	{
		return m_nextCenterGrid;
	}

	void Collider::SetNextCenterCoord(Vector2 nextCenter)
	{
		m_nextCenterCoord = nextCenter;
	}

	Vector2 Collider::GetNextCenterCoord()
	{
		return m_nextCenterCoord;
	}

	void Collider::SetIsContinuous(bool b_isContinuous)
	{
		m_b_isContinuous = b_isContinuous;
	}

	bool Collider::IsContinuous()
	{
		return m_b_isContinuous;
	}

	void Collider::SetIsStatic(bool b_isStatic)
	{
		m_b_isStatic = b_isStatic;
	}

	bool Collider::IsStatic()
	{
		return m_b_isStatic;
	}

	void Collider::SetIsSolid(bool b_isSolid)
	{
		m_b_isSolid = b_isSolid;
	}

	bool Collider::IsSolid()
	{
		return m_b_isSolid;
	}

	void Collider::SetRotation(float newRotation)
	{
		m_rotation = newRotation;
	}

	void Collider::UpdateRotation()
	{
		m_rotation = GetParent()->GetTransform()->GetRotation();
	}

	float Collider::GetRotation()
	{
		return GetParent()->GetTransform()->GetRotation();;
	}

	void Collider::SetIsComposite(bool b_isComposite)
	{
		m_b_isComposite = b_isComposite;
	}

	bool Collider::IsComposite()
	{
		return m_b_isComposite;
	}
}
