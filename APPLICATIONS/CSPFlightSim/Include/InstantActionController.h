#ifndef _INSTANTACTIONCONTROLLER_H__
#define _INSTANTACTIONCONTROLLER_H__


class BaseObject;
class Message;


/**
 * class InstantActionController - Describe me!
 *
 * @author unknown
 */
class InstantActionController : public BaseController
{
 public:

 public:
  InstantActionController();
  virtual ~InstantActionController();

  virtual void OnMessage(const Message *);
  virtual void OnUpdate(unsigned int dt);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();


 protected:
  int countPlanesInRange();
  int countTanksInRange();
  void generateNewEnemyPlanes();
  void generateNewEnemyTanks();
  void removeEnemyPlanesOutOfRange();
  void removeEnemyTanksOutOfRange();


};

#endif
