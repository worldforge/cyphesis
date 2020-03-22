

//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#ifndef STUB_TypeNode_TypeNode
#define STUB_TypeNode_TypeNode
   TypeNode::TypeNode(std::string name)
    : m_name(std::move(name)), m_parent(nullptr)
  {

  }

   TypeNode::TypeNode(std::string name, const Atlas::Objects::Root& obj)
       : m_name(std::move(name)),
       m_privateDescription(obj),
       m_protectedDescription(obj),
       m_publicDescription(obj),
        m_parent(nullptr)
   {

  }
#endif //STUB_TypeNode_TypeNode

#ifndef STUB_TypeNode_updateProperties
#define STUB_TypeNode_updateProperties
TypeNode::PropertiesUpdate TypeNode::updateProperties(const Atlas::Message::MapType& attributes, const PropertyManager& propertyManager)
{
    return TypeNode::PropertiesUpdate();
}
#endif //STUB_TypeNode_updateProperties

#ifndef STUB_TypeNode_description
#define STUB_TypeNode_description
Atlas::Objects::Root& TypeNode::description(Visibility visibility)
{
    switch (visibility) {
        case Visibility::PROTECTED:
            return m_protectedDescription;
        case Visibility::PRIVATE:
            return m_privateDescription;
        case Visibility::PUBLIC:
        default:
            return m_publicDescription;
    }
}

const Atlas::Objects::Root& TypeNode::description(Visibility visibility) const
{
    switch (visibility) {
        case Visibility::PROTECTED:
            return m_protectedDescription;
        case Visibility::PRIVATE:
            return m_privateDescription;
        case Visibility::PUBLIC:
        default:
            return m_publicDescription;
    }
}

#endif //STUB_TypeNode_description
