
#include <BaseObject.h>
#include <BaseEntity.h>
#include <VectorHolder.h>

/*! \fn BaseEntity::BaseEntity()
 *  \brief Default constructor for BaseEntity
 */
BaseEntity::BaseEntity()
{
  m_position         = StandardVector3::ZERO;
  m_velocity         = StandardVector3::ZERO;
  m_acceleration     = StandardVector3::ZERO;
  m_angularVelocity  = StandardVector3::ZERO;
  m_orientation      = StandardMatrix3::ZERO;

}

/*! \fn BaseEntity::~BaseEntity()
 *  \brief Destructor
 */
BaseEntity::~BaseEntity()
{

}

/*! \fn BaseEntity::BaseEntity(const BaseEntity & rhs)
 *  \brief Copy Constructor
 */
BaseEntity::BaseEntity(const BaseEntity & rhs)
{
  m_position         = rhs.m_position;
  m_velocity         = rhs.m_velocity;
  m_acceleration     = rhs.m_acceleration;
  m_angularVelocity  = rhs.m_angularVelocity;
  m_orientation      = rhs.m_orientation;

}

/*! \fn BaseEntity::operator=(const BaseEntity & rhs)
 *  \brief assignment operator
 */
BaseEntity & BaseEntity::operator=(const BaseEntity & rhs)
{
  m_position         = rhs.m_position;
  m_velocity         = rhs.m_velocity;
  m_acceleration     = rhs.m_acceleration;
  m_angularVelocity  = rhs.m_angularVelocity;
  m_orientation      = rhs.m_orientation;

  return *this;
}

void BaseEntity::OnUpdate(unsigned int dt)
{

}
 
void BaseEntity::SetController(unsigned int id)
{
  controller_id = id;
}


void BaseEntity::Initialize()
{

}

void BaseEntity::SetNamedValue(const char * name, const char * value)
{
  cout << "BaseEntity::SetNamedValue - name: " << name << ", value " << value << endl;
  // handle BaseEntity specific values

  if (0)
    {
      //TODO
    }
  else
    {
      BaseObject::SetNamedValue(name, value);
    }
}

void BaseEntity::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "BaseEntity::SetNamedObject - name: " << name << ", value " << value << endl;
  // handle BaseEntity specific objects
  if (0)
    {
      // TODO
    }
  if (strcmp(name, "POSITION_VECTOR") == 0)
    {
      VectorHolder * holder = (VectorHolder*)value;
      m_position = holder->m_Vector;
    }
  else if (strcmp(name, "VELOCITY_VECTOR") == 0)
    {
      VectorHolder * holder = (VectorHolder*)value;
      m_velocity = holder->m_Vector;

    }
  // this doesn't exist
  //  else if (strcmp(name, "DIRECTION_VECTOR") == 0)
  //    {
  //      VectorHolder * holder = (VectorHolder*)value;
  //      m_direction = holder->m_Vector;
  //    }
  // otherwise call base class
  else
    {
      BaseObject::SetNamedObject(name, value);
    }
}
 
void BaseEntity::SetValue(const char * value)
{

}

void BaseEntity::SetTokenizedValue(int token, const char * value)
{
  cout << "BaseEntity::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // handle BaseEntity specific values

      // otherwise call base class
    default:
      BaseObject::SetTokenizedValue(token, value);
    }
}

BaseObject * BaseEntity::CreateNamedObject(const char * name)
{
  if (strcmp(name, "POSITION_VECTOR") == 0 ||
      strcmp(name, "VELOCITY_VECTOR"))
    {
      VectorHolder * object = new VectorHolder;
      return object;
    }
  else 
    return BaseObject::CreateNamedObject(name);
}

void BaseEntity::dump()
{
  cout << "Dumping BaseEntity" << endl;
  BaseObject::dump();
}
