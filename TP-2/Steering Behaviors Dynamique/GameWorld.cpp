#include "GameWorld.h"
#include "Vehicle.h"
#include "constants.h"
#include "Obstacle.h"
#include "2d/Geometry.h"
#include "2d/Wall2D.h"
#include "2d/Transformations.h"
#include "SteeringBehaviors.h"
#include "time/PrecisionTimer.h"
#include "misc/Smoother.h"
#include "ParamLoader.h"
#include "misc/WindowUtils.h"
#include "misc/Stream_Utility_Functions.h"

#include "resource.h"


#include "AgentLeader.h"
#include "AgentPoursuiveur.h"

#include <list>
using std::list;


//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
GameWorld::GameWorld(int cx, int cy) :

	m_cxClient(cx),
	m_cyClient(cy),
	m_bPaused(false),
	m_vCrosshair(Vector2D(cxClient() / 2.0, cxClient() / 2.0)),
	m_bShowWalls(false),
	m_bShowObstacles(false),
	m_bShowPath(false),
	m_bShowWanderCircle(false),
	m_bShowSteeringForce(false),
	m_bShowFeelers(false),
	m_bShowDetectionBox(false),
	m_bShowFPS(true),
	m_dAvFrameTime(0),
	m_pPath(NULL),
	m_bRenderNeighbors(false),
	m_bViewKeys(false),
	m_bShowCellSpaceInfo(false),
	m_bAgentLeader(false),
	m_bAgentLeaderHumain(false),
	m_pAgentLeader(NULL),
	m_pAgentLeaderHumain(NULL)
{

  //setup the spatial subdivision class
  m_pCellSpace = new CellSpacePartition<Vehicle*>((double)cx, (double)cy, Prm.NumCellsX, Prm.NumCellsY, Prm.NumAgents);

  double border = 30;
  m_pPath = new Path(5, border, border, cx-border, cy-border, true); 


  /* .......................................................

  SETUP THE PURSUERS AGENTS

  .......................................................*/

  //Start at one because 0 is the leader
  for (int a = 0; a<Prm.NumAgents; ++a) {

	  //determine a random starting position
	  Vector2D SpawnPos = Vector2D(cx / 2.0 + RandomClamped()*cx / 2.0, cy / 2.0 + RandomClamped()*cy / 2.0);

	  AgentPoursuiveur* pAgentPoursuiveur = new AgentPoursuiveur(this,
		  SpawnPos,                 //initial position
		  RandFloat()*TwoPi,        //start rotation
		  Vector2D(0, 0),            //velocity
		  Prm.VehicleMass,          //mass
		  Prm.MaxSteeringForce,     //max force
		  Prm.MaxSpeed,             //max velocity
		  Prm.MaxTurnRatePerSecond, //max turn rate
		  Prm.VehicleScale,		  //scale
		  AGENTPOURSUIVEUR);

	  //pVehicle->Steering()->FlockingOn();
	  m_Vehicles.push_back(pAgentPoursuiveur);

	  //add it to the cell subdivision
	  m_pCellSpace->AddEntity(pAgentPoursuiveur);
  }

}

//-------------------------------- Destructor ----------------------------
// 
//	We take into account that we have to delete the leaders before the
//	pursuers because we have to empty leader's queue, so if we delete
//	pursuers before, we'll get an error.
//------------------------------------------------------------------------
GameWorld::~GameWorld()
{
  //first we delete leaders
  for (size_t a=0; a<m_Vehicles.size(); a++){
	  if (m_Vehicles[a] == m_pAgentLeader || m_Vehicles[a] == m_pAgentLeaderHumain) {
		  delete m_Vehicles[a];
		  m_Vehicles.erase(m_Vehicles.begin() + a);
	  }
  }
  //then delete all other vehicle
  for (size_t a = 0; a<m_Vehicles.size(); a++) {
	  delete m_Vehicles[a];
  }


  for (unsigned int ob=0; ob<m_Obstacles.size(); ++ob)
  {
    delete m_Obstacles[ob];
  }

  delete m_pCellSpace;
  
  delete m_pPath;
}


//----------------------------- Update -----------------------------------
//------------------------------------------------------------------------
void GameWorld::Update(double time_elapsed)
{ 
  if (m_bPaused) return;

  //create a smoother to smooth the framerate
  const int SampleRate = 10;
  static Smoother<double> FrameRateSmoother(SampleRate, 0.0);

  m_dAvFrameTime = FrameRateSmoother.Update(time_elapsed);
  


  //update the vehicles
  for (unsigned int a = 0; a < m_Vehicles.size(); ++a) {

	  // For every pursuers we calculate the distance to both leaders (if they exist)
	  // and we decide to follow the closer one if the distance is in the viewDistance
	  if(m_Vehicles[a] != m_pAgentLeader && m_Vehicles[a] != m_pAgentLeaderHumain){

		  AgentLeader* leaderToFollow = NULL;

		  if (m_pAgentLeader && m_pAgentLeaderHumain) {
			  Vector2D vectDist1(m_pAgentLeader->Pos() - m_Vehicles[a]->Pos());
			  Vector2D vectDist2(m_pAgentLeaderHumain->Pos() - m_Vehicles[a]->Pos());

			  if (vectDist1.Length() <= vectDist2.Length() && vectDist1.Length() <= Prm.ViewDistance) leaderToFollow = m_pAgentLeader;
			  else if(vectDist1.Length() > vectDist2.Length() && vectDist2.Length() <= Prm.ViewDistance) leaderToFollow = m_pAgentLeaderHumain;
		  }
		  else if (m_pAgentLeader) {
			  Vector2D vectDist(m_pAgentLeader->Pos() - m_Vehicles[a]->Pos());
			  if (vectDist.Length() <= Prm.ViewDistance) leaderToFollow = m_pAgentLeader;
		  }
		  else if (m_pAgentLeaderHumain) {
			  Vector2D vectDist(m_pAgentLeaderHumain->Pos() - m_Vehicles[a]->Pos());
			  if (vectDist.Length() <= Prm.ViewDistance) leaderToFollow = m_pAgentLeaderHumain;
		  }

		  if(leaderToFollow){
			  if (((AgentPoursuiveur*)m_Vehicles[a])->getLeaderOfMyQueue() != leaderToFollow) {
				  leaderToFollow->addAgentPoursuiveur((AgentPoursuiveur*)m_Vehicles[a]);
			  }
		  }

	  }

    // We did not use the following method because it wasn't detecting correctly the neighbors
	// This was tested only for m_pAgentLeader

	/*if (m_Vehicles[a] != m_pAgentLeader) {
		CellSpace()->CalculateNeighbors(m_Vehicles[a]->Pos(), Prm.ViewDistance);
		for (BaseGameEntity* pV = CellSpace()->begin(); !CellSpace()->end(); pV = CellSpace()->next()) {
			if (pV == m_pAgentLeader) {
				if (((AgentPoursuiveur*)m_Vehicles[a])->getLeaderOfMyQueue() != m_pAgentLeader) {
					m_pAgentLeader->addAgentPoursuiveur((AgentPoursuiveur*)m_Vehicles[a]);
				}
					  
			}
		}
	}*/

    m_Vehicles[a]->Update(time_elapsed);
  }
}
  

//--------------------------- CreateWalls --------------------------------
//
//  creates some walls that form an enclosure for the steering agents.
//  used to demonstrate several of the steering behaviors
//------------------------------------------------------------------------
void GameWorld::CreateWalls()
{
  //create the walls  
  double bordersize = 20.0;
  double CornerSize = 0.2;
  double vDist = m_cyClient-2*bordersize;
  double hDist = m_cxClient-2*bordersize;
  
  const int NumWallVerts = 8;

  Vector2D walls[NumWallVerts] = {Vector2D(hDist*CornerSize+bordersize, bordersize),
                                   Vector2D(m_cxClient-bordersize-hDist*CornerSize, bordersize),
                                   Vector2D(m_cxClient-bordersize, bordersize+vDist*CornerSize),
                                   Vector2D(m_cxClient-bordersize, m_cyClient-bordersize-vDist*CornerSize),
                                         
                                   Vector2D(m_cxClient-bordersize-hDist*CornerSize, m_cyClient-bordersize),
                                   Vector2D(hDist*CornerSize+bordersize, m_cyClient-bordersize),
                                   Vector2D(bordersize, m_cyClient-bordersize-vDist*CornerSize),
                                   Vector2D(bordersize, bordersize+vDist*CornerSize)};
  
  for (int w=0; w<NumWallVerts-1; ++w)
  {
    m_Walls.push_back(Wall2D(walls[w], walls[w+1]));
  }

  m_Walls.push_back(Wall2D(walls[NumWallVerts-1], walls[0]));
}


//--------------------------- CreateObstacles -----------------------------
//
//  Sets up the vector of obstacles with random positions and sizes. Makes
//  sure the obstacles do not overlap
//------------------------------------------------------------------------
void GameWorld::CreateObstacles()
{
    //create a number of randomly sized tiddlywinks
  for (int o=0; o<Prm.NumObstacles; ++o)
  {   
    bool bOverlapped = true;

    //keep creating tiddlywinks until we find one that doesn't overlap
    //any others.Sometimes this can get into an endless loop because the
    //obstacle has nowhere to fit. We test for this case and exit accordingly

    int NumTrys = 0; int NumAllowableTrys = 2000;

    while (bOverlapped)
    {
      NumTrys++;

      if (NumTrys > NumAllowableTrys) return;
      
      int radius = RandInt((int)Prm.MinObstacleRadius, (int)Prm.MaxObstacleRadius);

      const int border                 = 10;
      const int MinGapBetweenObstacles = 20;

      Obstacle* ob = new Obstacle(RandInt(radius+border, m_cxClient-radius-border),
                                  RandInt(radius+border, m_cyClient-radius-30-border),
                                  radius);

      if (!Overlapped(ob, m_Obstacles, MinGapBetweenObstacles))
      {
        //its not overlapped so we can add it
        m_Obstacles.push_back(ob);

        bOverlapped = false;
      }

      else
      {
        delete ob;
      }
    }
  }
}


//------------------------- Set Crosshair ------------------------------------
//
//  The user can set the position of the crosshair by right clicking the
//  mouse. This method makes sure the click is not inside any enabled
//  Obstacles and sets the position appropriately
//------------------------------------------------------------------------
void GameWorld::SetCrosshair(POINTS p)
{
  Vector2D ProposedPosition((double)p.x, (double)p.y);

  //make sure it's not inside an obstacle
  for (ObIt curOb = m_Obstacles.begin(); curOb != m_Obstacles.end(); ++curOb)
  {
    if (PointInCircle((*curOb)->Pos(), (*curOb)->BRadius(), ProposedPosition))
    {
      return;
    }

  }
  m_vCrosshair.x = (double)p.x;
  m_vCrosshair.y = (double)p.y;
}

//------------------------- addPursuer --------------------------------
//
//	Add a pursuer to the world
//------------------------------------------------------------------------

void GameWorld::addPursuer() {
	Vector2D SpawnPos = Vector2D(m_cxClient / 2.0 + RandomClamped()*m_cxClient / 2.0, m_cyClient / 2.0 + RandomClamped()*m_cyClient / 2.0);

	AgentPoursuiveur* pAgentPoursuiveur = new AgentPoursuiveur(this,
		SpawnPos,                 //initial position
		RandFloat()*TwoPi,        //start rotation
		Vector2D(0, 0),            //velocity
		Prm.VehicleMass,          //mass
		Prm.MaxSteeringForce,     //max force
		Prm.MaxSpeed,             //max velocity
		Prm.MaxTurnRatePerSecond, //max turn rate
		Prm.VehicleScale,		  //scale
		AGENTPOURSUIVEUR);

	//pVehicle->Steering()->FlockingOn();
	m_Vehicles.push_back(pAgentPoursuiveur);

	//add it to the cell subdivision
	m_pCellSpace->AddEntity(pAgentPoursuiveur);
}

//------------------------- removePursuer --------------------------------
// 
//	Remove a pursuer from the world
//------------------------------------------------------------------------

void GameWorld::removePursuer() {
		for (int a = m_Vehicles.size() - 1; a >= 0; a--) {
			if (m_Vehicles[a] != m_pAgentLeader && m_Vehicles[a] != m_pAgentLeaderHumain) {
				delete (AgentPoursuiveur*)m_Vehicles[a];
				m_Vehicles.erase(m_Vehicles.begin() + a);
				break;
			}
		}
}


//------------------------- HandleKeyPresses -----------------------------
void GameWorld::HandleKeyPresses(WPARAM wParam)
{

  switch(wParam)
  {
  case 'U':
    {
      delete m_pPath;
      double border = 60;
      m_pPath = new Path(RandInt(3, 7), border, border, cxClient()-border, cyClient()-border, true); 
      m_bShowPath = true; 
      for (unsigned int i=0; i<m_Vehicles.size(); ++i)
      {
        m_Vehicles[i]->Steering()->SetPath(m_pPath->GetPath());
      }
    }
    break;

    case 'P':
      
      TogglePause(); break;

    case 'O':

      ToggleRenderNeighbors(); break;

    case 'I':

      {
        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->ToggleSmoothing();
        }

      }

      break;

    case 'Y':

       m_bShowObstacles = !m_bShowObstacles;

        if (!m_bShowObstacles)
        {
          m_Obstacles.clear();

          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_Vehicles[i]->Steering()->ObstacleAvoidanceOff();
          }
        }
        else
        {
          CreateObstacles();

          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_Vehicles[i]->Steering()->ObstacleAvoidanceOn();
          }
        }
        break;

	case 38: //up arrow
		{
		if(m_pAgentLeaderHumain) m_pAgentLeaderHumain->accelerate();
		}
	break;

	case 40: //down arrow
	{
		if(m_pAgentLeaderHumain) m_pAgentLeaderHumain->decelerate();
	}
	break;

	case 37: //left arrow
		{
		if (m_pAgentLeaderHumain) m_pAgentLeaderHumain->turnLeft();
		}
	break;

	case 39: //right arrow
		{
		if (m_pAgentLeaderHumain) m_pAgentLeaderHumain->turnRight();
		}
	break;

	case 107: // plus pave num
	{
		addPursuer();
	}
	break;

	case 109: //moins pave num
	{
		removePursuer();
	}
	break;
  }//end switch
}



//-------------------------- HandleMenuItems -----------------------------
void GameWorld::HandleMenuItems(WPARAM wParam, HWND hwnd)
{
  switch(wParam)
  {
    case ID_OB_OBSTACLES:

        m_bShowObstacles = !m_bShowObstacles;

        if (!m_bShowObstacles)
        {
          m_Obstacles.clear();

          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_Vehicles[i]->Steering()->ObstacleAvoidanceOff();
          }

          //uncheck the menu
         ChangeMenuState(hwnd, ID_OB_OBSTACLES, MFS_UNCHECKED);
        }
        else
        {
          CreateObstacles();

          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_Vehicles[i]->Steering()->ObstacleAvoidanceOn();
          }

          //check the menu
          ChangeMenuState(hwnd, ID_OB_OBSTACLES, MFS_CHECKED);
        }

       break;

    case ID_OB_WALLS:

      m_bShowWalls = !m_bShowWalls;

      if (m_bShowWalls)
      {
        CreateWalls();

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->WallAvoidanceOn();
        }

        //check the menu
         ChangeMenuState(hwnd, ID_OB_WALLS, MFS_CHECKED);
      }

      else
      {
        m_Walls.clear();

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->WallAvoidanceOff();
        }

        //uncheck the menu
         ChangeMenuState(hwnd, ID_OB_WALLS, MFS_UNCHECKED);
      }

      break;


    case IDR_PARTITIONING:
      {
        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->ToggleSpacePartitioningOnOff();
        }

        //if toggled on, empty the cell space and then re-add all the 
        //vehicles
        if (m_Vehicles[0]->Steering()->isSpacePartitioningOn())
        {
          m_pCellSpace->EmptyCells();
       
          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_pCellSpace->AddEntity(m_Vehicles[i]);
          }

          ChangeMenuState(hwnd, IDR_PARTITIONING, MFS_CHECKED);
        }
        else
        {
          ChangeMenuState(hwnd, IDR_PARTITIONING, MFS_UNCHECKED);
          ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_UNCHECKED);
          m_bShowCellSpaceInfo = false;

        }
      }

      break;

    case IDM_PARTITION_VIEW_NEIGHBORS:
      {

        m_bShowCellSpaceInfo = !m_bShowCellSpaceInfo;
        
        if (m_bShowCellSpaceInfo)
        {
          ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_CHECKED);

          if (!m_Vehicles[0]->Steering()->isSpacePartitioningOn())
          {
            SendMessage(hwnd, WM_COMMAND, IDR_PARTITIONING, NULL);
          }
        }
        else
        {
          ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_UNCHECKED);
        }
      }
      break;
        

    case IDR_WEIGHTED_SUM:
      {
        ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_CHECKED);
        ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_UNCHECKED);
        ChangeMenuState(hwnd, IDR_DITHERED, MFS_UNCHECKED);

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::weighted_average);
        }
      }

      break;

    case IDR_PRIORITIZED:
      {
        ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_UNCHECKED);
        ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_CHECKED);
        ChangeMenuState(hwnd, IDR_DITHERED, MFS_UNCHECKED);

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::prioritized);
        }
      }

      break;

    case IDR_DITHERED:
      {
        ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_UNCHECKED);
        ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_UNCHECKED);
        ChangeMenuState(hwnd, IDR_DITHERED, MFS_CHECKED);

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::dithered);
        }
      }

      break;


      case ID_VIEW_KEYS:
      {
        ToggleViewKeys();

        CheckMenuItemAppropriately(hwnd, ID_VIEW_KEYS, m_bViewKeys);
      }

      break;

      case ID_VIEW_FPS:
      {
        ToggleShowFPS();

        CheckMenuItemAppropriately(hwnd, ID_VIEW_FPS, RenderFPS());
      }

      break;

      case ID_MENU_SMOOTHING:
      {
        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->ToggleSmoothing();
        }

        CheckMenuItemAppropriately(hwnd, ID_MENU_SMOOTHING, m_Vehicles[0]->isSmoothingOn());
      }

      break;

	  case ID_AGENT_LEADER:

		if (!m_bAgentLeader) {
			m_bAgentLeader = true;

			/* .......................................................

			SETUP THE LEADER

			.......................................................*/

			Vector2D SpawnPosHuman = Vector2D(m_cxClient / 2.0, m_cyClient / 2.0);

			m_pAgentLeader = new AgentLeader(this,
				SpawnPosHuman,                 //initial position
				RandFloat()*TwoPi,        //start rotation
				Vector2D(0, 0),            //velocity
				Prm.VehicleMass,          //mass
				Prm.MaxSteeringForce,     //max force
				Prm.MaxSpeed,             //max velocity
				Prm.MaxTurnRatePerSecond, //max turn rate
				Prm.VehicleScale,         //scale
				AGENTLEADER,
				SIMPLE_QUEUE);

			
			m_Vehicles.push_back(m_pAgentLeader);
			//add it to the cell subdivision
			m_pCellSpace->AddEntity(m_pAgentLeader);

		}
		else {

			m_bAgentLeader = false;

			for (size_t i = 0; i < m_Vehicles.size(); i++) {
				if (m_Vehicles[i] == m_pAgentLeader) {
					delete m_pAgentLeader;
					m_pAgentLeader = NULL;
					m_Vehicles.erase(m_Vehicles.begin() + i);
				}
			}

		}

		break;

	  case ID_AGENT_LEADER_HUMAIN: 

		if (!m_bAgentLeaderHumain) {
			m_bAgentLeaderHumain = true;

			/* .......................................................

			SETUP THE HUMAN LEADER

			.......................................................*/

			Vector2D SpawnPosHuman = Vector2D(m_cxClient / 2.0, m_cyClient / 2.0);

			m_pAgentLeaderHumain = new AgentLeaderHumain(this,
				SpawnPosHuman,                 //initial position
				RandFloat()*TwoPi,        //start rotation
				Vector2D(0, 0),            //velocity
				Prm.VehicleMass,          //mass
				Prm.MaxSteeringForce,     //max force
				Prm.MaxSpeed,             //max velocity
				Prm.MaxTurnRatePerSecond, //max turn rate
				Prm.VehicleScale,         //scale
				AGENTLEADERHUMAIN,
				FLOCKING_V_QUEUE);

			m_Vehicles.push_back(m_pAgentLeaderHumain);
			//add it to the cell subdivision
			m_pCellSpace->AddEntity(m_pAgentLeaderHumain);

		}
		else {

			m_bAgentLeaderHumain = false;

			for (size_t i = 0; i < m_Vehicles.size(); i++){
				if (m_Vehicles[i] == m_pAgentLeaderHumain) {
					delete m_pAgentLeaderHumain;
					m_pAgentLeaderHumain = NULL;
					m_Vehicles.erase(m_Vehicles.begin() + i);
				}
			}
			
		}

		break;

	  case ID_ADD_PURSUER:
	  {
		  addPursuer();
	  }

	  break;

	  case ID_REMOVE_PURSUER:
	  {
		  removePursuer();
	  }

	  break;

  }//end switch
}


//------------------------------ Render ----------------------------------
//------------------------------------------------------------------------
void GameWorld::Render()
{
  gdi->TransparentText();

  //render any walls
  gdi->BlackPen();
  for (unsigned int w=0; w<m_Walls.size(); ++w)
  {
    m_Walls[w].Render(true);  //true flag shows normals
  }

  //render any obstacles
  gdi->BlackPen();
  
  for (unsigned int ob=0; ob<m_Obstacles.size(); ++ob)
  {
    gdi->Circle(m_Obstacles[ob]->Pos(), m_Obstacles[ob]->BRadius());
  }

  //render the agents
  for (unsigned int a=0; a<m_Vehicles.size(); ++a)
  {
    m_Vehicles[a]->Render();  
    
    //render cell partitioning stuff
    if (m_bShowCellSpaceInfo && a==0)
    {
      gdi->HollowBrush();
      InvertedAABBox2D box(m_Vehicles[a]->Pos() - Vector2D(Prm.ViewDistance, Prm.ViewDistance),
                           m_Vehicles[a]->Pos() + Vector2D(Prm.ViewDistance, Prm.ViewDistance));
      box.Render();

      gdi->RedPen();
      CellSpace()->CalculateNeighbors(m_Vehicles[a]->Pos(), Prm.ViewDistance);
      for (BaseGameEntity* pV = CellSpace()->begin();!CellSpace()->end();pV = CellSpace()->next())
      {
        gdi->Circle(pV->Pos(), pV->BRadius());
      }
      
      gdi->GreenPen();
      gdi->Circle(m_Vehicles[a]->Pos(), Prm.ViewDistance);
    }
  }  

//#define CROSSHAIR
#ifdef CROSSHAIR
  //and finally the crosshair
  gdi->RedPen();
  gdi->Circle(m_vCrosshair, 4);
  gdi->Line(m_vCrosshair.x - 8, m_vCrosshair.y, m_vCrosshair.x + 8, m_vCrosshair.y);
  gdi->Line(m_vCrosshair.x, m_vCrosshair.y - 8, m_vCrosshair.x, m_vCrosshair.y + 8);
  gdi->TextAtPos(5, cyClient() - 20, "Click to move crosshair");
#endif


  //gdi->TextAtPos(cxClient() -120, cyClient() - 20, "Press R to reset");

  gdi->TextColor(Cgdi::grey);
  if (RenderPath())
  {
     gdi->TextAtPos((int)(cxClient()/2.0f - 80), cyClient() - 20, "Press 'U' for random path");

     m_pPath->Render();
  }

  if (RenderFPS())
  {
    gdi->TextColor(Cgdi::grey);
    gdi->TextAtPos(5, cyClient() - 20, ttos(1.0 / m_dAvFrameTime));
  } 

  if (m_bShowCellSpaceInfo)
  {
    m_pCellSpace->RenderCells();
  }

}
