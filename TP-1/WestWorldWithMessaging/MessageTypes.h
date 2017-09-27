#ifndef MESSAGE_TYPES
#define MESSAGE_TYPES

#include <string>

enum message_type
{
  Msg_HiHoneyImHome,
  Msg_StewReady,
  Msg_BarFlyWantsToFight,
  Msg_BarFlyLostFight,
  Msg_MinerFightsBarFly
};


inline std::string MsgToStr(int msg)
{
  switch (msg)
  {
  case Msg_HiHoneyImHome:
    
    return "HiHoneyImHome"; 

  case Msg_StewReady:
    
    return "StewReady";

  case Msg_BarFlyWantsToFight:

	return "BarFlyWantsToFight";

  case Msg_BarFlyLostFight:

	  return "Msg_BarFlyLostFight";

  case Msg_MinerFightsBarFly:

	return "Msg_MinerFightsBarFly";

  default:

    return "Not recognized!";
  }
}

#endif