// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "LocatedEntity.h"

#include "Domain.h"

#include "Script.h"
#include "AtlasProperties.h"

#include "common/TypeNode.h"
#include "common/PropertyManager.h"
#include "common/operations/Update.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <memory>

using Atlas::Objects::Operation::Update;
using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief Set of attribute names which must not be changed
///
/// The attributes named are special and are modified using high level
/// operations, such as Move, not via Set operations, or assigned by
/// normal means.
std::set<std::string> LocatedEntity::s_immutable = {"id", "parent", "loc", "contains", "objtype"};

/// \brief Singleton accessor for immutables
///
/// The immutable attribute set m_immutables is returned.
const std::set<std::string>& LocatedEntity::immutables()
{
    return s_immutable;
}


/// \brief LocatedEntity constructor
LocatedEntity::LocatedEntity(RouterId id) :
        Router(std::move(id)),
        m_seq(0),
        m_type(nullptr),
        m_flags(0),
        m_parent(nullptr),
        m_contains(nullptr)
{
    m_properties[LocationProperty::property_name].property = std::make_unique<LocationProperty>(*this);
    m_properties[IdProperty::property_name].property = std::make_unique<IdProperty>(getId());
}

LocatedEntity::~LocatedEntity()
{
    m_scriptEntity.clear();
    if (!isDestroyed()) {
        clearProperties();
    }
}

void LocatedEntity::clearProperties()
{
    if (m_type) {
        for (auto& entry : m_type->defaults()) {
            //Only remove if there's no instance specific property.
            if (m_properties.find(entry.first) == m_properties.end()) {
                entry.second->remove(*this, entry.first);
            }
        }
    }

    for (auto& entry : m_properties) {
        entry.second.property->remove(*this, entry.first);
    }
}

void LocatedEntity::setType(const TypeNode* t)
{
    m_type = t;
}

/// \brief Check if this entity has a property with the given name
///
/// @param name Name of attribute to be checked
/// @return true if this entity has an attribute with the name given
/// false otherwise
bool LocatedEntity::hasAttr(const std::string& name) const
{
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        return true;
    }
    if (m_type != nullptr) {
        auto J = m_type->defaults().find(name);
        if (J != m_type->defaults().end()) {
            return true;
        }
    }
    return false;
}

int LocatedEntity::getAttr(const std::string& name,
                           Element& attr) const
{
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second.property->get(attr);
    }
    if (m_type != nullptr) {
        auto J = m_type->defaults().find(name);
        if (J != m_type->defaults().end()) {
            return J->second->get(attr);
        }
    }
    return -1;
}

boost::optional<Atlas::Message::Element> LocatedEntity::getAttr(const std::string& name) const
{
    //The idea here is to gradually move away from the C style method call to a more C++ based one with boost::optional (and even std::optional).
    Atlas::Message::Element attr;
    auto result = getAttr(name, attr);
    if (result == 0) {
        return attr;
    } else {
        return boost::none;
    }
}


int LocatedEntity::getAttrType(const std::string& name,
                               Element& attr,
                               int type) const
{
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second.property->get(attr) || (attr.getType() == type ? 0 : 1);
    }
    if (m_type != nullptr) {
        auto J = m_type->defaults().find(name);
        if (J != m_type->defaults().end()) {
            return J->second->get(attr) || (attr.getType() == type ? 0 : 1);
        }
    }
    return -1;
}

boost::optional<Atlas::Message::Element> LocatedEntity::getAttrType(const std::string& name, int type) const
{
    //The idea here is to gradually move away from the C style method call to a more C++ based one with boost::optional (and even std::optional).

    Atlas::Message::Element attr;
    auto result = getAttrType(name, attr, type);
    if (result == 0) {
        return attr;
    } else {
        return boost::none;
    }
}

PropertyBase* LocatedEntity::setAttrValue(const std::string& name, Element attr)
{
    auto parsedPropertyName = PropertyUtil::parsePropertyModification(name);
    return setAttr(parsedPropertyName.second, Modifier::createModifier(parsedPropertyName.first, std::move(attr)).get());
}

PropertyBase* LocatedEntity::setAttr(const std::string& name, const Modifier* modifier)
{
    if (!PropertyUtil::isValidName(name)) {
        log(WARNING, String::compose("Tried to add a property '%1' to entity '%2', which has an invalid name.", name, describeEntity()));
        return nullptr;
    }

    bool propNeedsInstalling = false;
    PropertyBase* prop;
    Atlas::Message::Element attr;
    // If it is an existing property, just update the value.
    auto I = m_properties.find(name);
    if (I == m_properties.end() || !I->second.property) {
        //Install a new property.
        std::map<std::string, std::unique_ptr<PropertyBase>>::const_iterator J;
        if (m_type && (J = m_type->defaults().find(name)) != m_type->defaults().end()) {
            prop = J->second->copy();
            m_properties[name].property.reset(prop);
        } else {
            // This is an entirely new property, not just a modification of
            // one in defaults, so we need to install it to this Entity.
            auto newProp = createProperty(name);
            prop = newProp.get();
            m_properties[name].property = std::move(newProp);
            propNeedsInstalling = true;
        }
    } else {
        prop = I->second.property.get();
    }

    if (I != m_properties.end() && !I->second.modifiers.empty()) {
        ModifiableProperty& modifiableProperty = I->second;
        //Should we apply any modifiers?
        //If the new value is default we can just apply it directly.
        if (modifier) {
            if (modifier->getType() == ModifierType::Default) {
                modifiableProperty.baseValue = modifier->mValue;
                attr = modifier->mValue;
            } else {
                attr = modifiableProperty.baseValue;
                modifier->process(attr, modifiableProperty.baseValue);
            }
        }
        for (auto& modifierEntry : modifiableProperty.modifiers) {
            modifierEntry.first->process(attr, modifiableProperty.baseValue);
        }
    } else {
        if (modifier) {
            if (modifier->getType() == ModifierType::Default) {
                attr = modifier->mValue;
            } else {
                prop->get(attr);
                modifier->process(attr, attr);
            }
        }
    }
    //By now we should always have a new prop.
    assert(prop != nullptr);
    prop->removeFlags(prop_flag_persistence_clean);
    prop->set(attr);

    //We deferred any installation until after the prop has had its value set.
    if (propNeedsInstalling) {
        prop->install(*this, name);
    }

    // Allow the value to take effect.
    applyProperty(name, *prop);
    return prop;
}

void LocatedEntity::addModifier(const std::string& propertyName, Modifier* modifier, LocatedEntity* affectingEntity)
{
    if (hasFlags(entity_modifiers_not_allowed)) {
        return;
    }
    if (!PropertyUtil::isValidName(propertyName)) {
        log(WARNING, String::compose("Tried to add a modifier for property '%1' to entity '%2', which has an invalid name.", propertyName, describeEntity()));
        return;
    }

    m_activeModifiers[affectingEntity].emplace(propertyName, modifier);

    auto I = m_properties.find(propertyName);
    if (I != m_properties.end()) {
        if (I->second.property && I->second.property->hasFlags(prop_flag_modifiers_not_allowed)) {
            return;
        }
        //We had a local value, check if there are any modifiers.
        if (I->second.modifiers.empty()) {
            //Set the base value from the current
            I->second.property->get(I->second.baseValue);
        }
        I->second.modifiers.emplace_back(modifier, affectingEntity);
        DefaultModifier defaultModifier(I->second.baseValue);
        setAttr(propertyName, &defaultModifier);
    } else {

        //Check if there's also a property in the type, and if so create a copy.
        bool had_property_in_type = false;
        if (m_type) {
            auto typeI = m_type->defaults().find(propertyName);
            if (typeI != m_type->defaults().end()) {
                if (typeI->second->hasFlags(entity_modifiers_not_allowed)) {
                    return;
                }
                //We need to create a new modifier entry.
                auto& modifiableProperty = m_properties[propertyName];
                modifiableProperty.modifiers.emplace_back(modifier, affectingEntity);
                //Copy the default value.
                typeI->second->get(modifiableProperty.baseValue);
                DefaultModifier defaultModifier(modifiableProperty.baseValue);
                //Apply the new value
                setAttr(propertyName, &defaultModifier);
                had_property_in_type = true;
            }
        }
        if (!had_property_in_type) {
            //We need to create a new modifier entry with a new property.
            auto& modifiableProperty = m_properties[propertyName];
            modifiableProperty.modifiers.emplace_back(modifier, affectingEntity);
            //Apply the new value
            setAttr(propertyName, nullptr);
        }
    }
    enqueueUpdateOp();
}

void LocatedEntity::removeModifier(const std::string& propertyName, Modifier* modifier)
{
    for (auto& entry: m_activeModifiers) {
        auto result = entry.second.erase(std::make_pair(propertyName, modifier));
        if (result > 0) {
            break;
        }
    }

    auto propertyI = m_properties.find(propertyName);
    if (propertyI == m_properties.end()) {
        log(WARNING, String::compose("Tried to remove modifier from property %1 which couldn't be found.", propertyName));
        return;
    }

    auto& modifiers = propertyI->second.modifiers;
    for (auto I = modifiers.begin(); I != modifiers.end(); ++I) {
        if (modifier == I->first) {
            modifiers.erase(I);
            //FIXME: If there's no base value we should remove the property, but there's no support in the storage manager for that yet.
            setAttrValue(propertyName, propertyI->second.baseValue);

            enqueueUpdateOp();

            return;
        }
    }
}

/// \brief Get the property object for a given attribute
///
/// @param name name of the attribute for which the property is required.
/// @return a pointer to the property, or zero if the attributes does
/// not exist, or is not stored using a property object.
const PropertyBase* LocatedEntity::getProperty(const std::string& name) const
{
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second.property.get();
    }
    if (m_type != nullptr) {
        auto J = m_type->defaults().find(name);
        if (J != m_type->defaults().end()) {
            return J->second.get();
        }
    }
    return nullptr;
}

PropertyBase* LocatedEntity::modProperty(const std::string& name, const Atlas::Message::Element& def_val)
{
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second.property.get();
    }
    if (m_type != nullptr) {
        auto J = m_type->defaults().find(name);
        if (J != m_type->defaults().end()) {
            // We have a default for this property. Create a new instance
            // property with the same value.
            PropertyBase* new_prop = J->second->copy();
            if (!def_val.isNone()) {
                new_prop->set(def_val);
            }
            J->second->remove(*this, name);
            new_prop->removeFlags(prop_flag_class);
            m_properties[name].property.reset(new_prop);
            new_prop->install(*this, name);
            applyProperty(name, *new_prop);
            return new_prop;
        }
    }
    return nullptr;
}

PropertyBase* LocatedEntity::setProperty(const std::string& name,
                                         std::unique_ptr<PropertyBase> prop)
{
    auto p = prop.get();
    m_properties[name].property = std::move(prop);
    p->install(*this, name);
    return p;
}

void LocatedEntity::installDelegate(int, const std::string&)
{
}

void LocatedEntity::removeDelegate(int, const std::string&)
{
}

/// \brief Called when the container of this entity changes.
///
void LocatedEntity::onContainered(const Ref<LocatedEntity>& new_loc)
{
}

/// \brief Called when the properties of this entity change.
///
void LocatedEntity::onUpdated()
{
}

/// \brief Called when the entity needs to be removed from its context
void LocatedEntity::destroy()
{
    m_scriptEntity.clear();
    m_scripts.clear();

    if (m_parent) {
        m_parent->removeChild(*this);
        m_parent = nullptr;
    }

    clearProperties();

    m_flags.addFlags(entity_destroyed);
    destroyed.emit();
}

Domain* LocatedEntity::getDomain()
{
    return nullptr;
}

const Domain* LocatedEntity::getDomain() const
{
    return nullptr;
}

void LocatedEntity::setDomain(std::unique_ptr<Domain> domain)
{
    //no-op
}

void LocatedEntity::sendWorld(Operation)
{
}

void LocatedEntity::sendWorld(OpVector& res) {
    for (auto& op: res) {
        sendWorld(std::move(op));
    }
}


/// \brief Associate a script with this entity
///
/// The previously associated script is deleted.
/// @param script Pointer to the script to be associated with this entity
void LocatedEntity::setScript(std::unique_ptr<Script> script)
{
    script->attachPropertyCallbacks(*this);
    m_scripts.emplace_back(std::move(script));
}

/// \brief Make this entity a container
///
/// If this entity is not already a contains, set up the necessary
/// storage and property.
void LocatedEntity::makeContainer()
{
    if (!m_contains) {
        m_contains = std::make_unique<LocatedEntitySet>();
        m_properties[ContainsProperty::property_name].property = std::make_unique<ContainsProperty>(*m_contains);
    }
}

/// \brief Change the container of an entity
///
/// @param new_loc The entity which is to become this entity's new
/// container.
void LocatedEntity::changeContainer(const Ref<LocatedEntity>& new_loc)
{
    auto oldLoc = m_parent;
    if (oldLoc) {
        oldLoc->removeChild(*this);
    }
    new_loc->addChild(*this);

    applyProperty(LocationProperty::property_name, *m_properties[LocationProperty::property_name].property);

    onContainered(oldLoc);
}

void LocatedEntity::broadcast(const Atlas::Objects::Operation::RootOperation& op, OpVector& res, Visibility visibility) const
{
    std::set<const LocatedEntity*> receivers;
    collectObservers(receivers);

    for (auto& entity : receivers) {
        if (visibility == Visibility::PRIVATE) {
            //Only send private ops to admins
            if (!entity->hasFlags(entity_admin)) {
                continue;
            }
        } else if (visibility == Visibility::PROTECTED) {
            //Protected ops also goes to the entity itself
            if (!entity->hasFlags(entity_admin) &&
                entity->getIntId() != getIntId()) {
                continue;
            }
        }
        auto newOp = op.copy();
        newOp->setTo(entity->getId());
        newOp->setFrom(getId());
        res.push_back(newOp);
    }
}

void LocatedEntity::collectObservers(std::set<const LocatedEntity*>& receivers) const
{
    if (isPerceptive()) {
        receivers.insert(this);
    }
    const Domain* domain = getDomain();
    if (domain) {
        auto observingEntities = domain->getObservingEntitiesFor(*this);
        receivers.insert(observingEntities.begin(), observingEntities.end());
    }
    if (m_parent) {
        m_parent->collectObserversForChild(*this, receivers);
    }
}

void LocatedEntity::collectObserved(std::set<const LocatedEntity*>& observed) const
{
    const Domain* domain = getDomain();
    if (domain) {
        std::list<LocatedEntity*> observedEntities;
        domain->getVisibleEntitiesFor(*this, observedEntities);
        observed.insert(observedEntities.begin(), observedEntities.end());
    }
}

void LocatedEntity::collectObserversForChild(const LocatedEntity& child, std::set<const LocatedEntity*>& receivers) const
{
    const Domain* domain = getDomain();

    if (isPerceptive()) {
        receivers.insert(this);
    }

    if (domain) {
        auto observingEntities = domain->getObservingEntitiesFor(child);
        receivers.insert(observingEntities.begin(), observingEntities.end());
    }
    if (m_parent) {
        //If this entity have a movement domain, check if the child entity is visible to the parent entity (i.e. it's "exposed outside of the domain"). If not, the broadcast chain stops here.
        if (domain && !domain->isEntityVisibleFor(*m_parent, child)) {
            return;
        }
        m_parent->collectObserversForChild(*this, receivers);
    }
}

void LocatedEntity::processAppearDisappear(std::set<const LocatedEntity*> previousObserving, OpVector& res) const
{
    std::set<const LocatedEntity*> nowObservers;
    collectObservers(nowObservers);
    for (auto entity : nowObservers) {
        auto numberErased = previousObserving.erase(entity);
        if (numberErased == 0) {
            Atlas::Objects::Operation::Appearance appear;
            Atlas::Objects::Entity::Anonymous that_ent;
            that_ent->setId(getId());
            that_ent->setStamp(getSeq());
            appear->setArgs1(that_ent);
            appear->setTo(entity->getId());
            res.push_back(appear);
        }
    }

    for (auto entity : previousObserving) {
        Atlas::Objects::Operation::Disappearance disappear;
        Atlas::Objects::Entity::Anonymous that_ent;
        that_ent->setId(getId());
        that_ent->setStamp(getSeq());
        disappear->setArgs1(that_ent);
        disappear->setTo(entity->getId());
        res.push_back(disappear);
    }
}

void LocatedEntity::applyProperty(const std::string& name, PropertyBase& prop)
{
    // Allow the value to take effect.
    prop.apply(*this);
    prop.addFlags(prop_flag_unsent);
    propertyApplied(name, prop);
    // Mark the Entity as unclean
    m_flags.removeFlags(entity_clean);
}

void LocatedEntity::addChild(LocatedEntity& childEntity)
{
    makeContainer();
    bool was_empty = m_contains->empty();
    m_contains->insert(&childEntity);
    if (was_empty) {
        onUpdated();
    }

    childEntity.m_parent = this;
}

void LocatedEntity::removeChild(LocatedEntity& childEntity)
{
    assert(checkRef() > 0);
    assert(m_contains != nullptr);
    assert(m_contains->count(&childEntity));
    m_contains->erase(&childEntity);
    if (m_contains->empty()) {
        onUpdated();
    }
}

void LocatedEntity::addListener(OperationsListener* listener)
{
    //No-op in LocatedEntity
}

void LocatedEntity::removeListener(OperationsListener* listener)
{
    //No-op in LocatedEntity
}

bool LocatedEntity::isVisibleForOtherEntity(const LocatedEntity& observer) const
{
    //Are we looking at ourselves?
    if (&observer == this) {
        return true;
    }

    //Optimize for the most common case of both entities being direct child of a domain
    if (m_parent != nullptr && observer.m_parent == m_parent && m_parent->getDomain()) {
        // Since both have the same parent, we can check right now for private and protected.
        if (!observer.hasFlags(entity_admin) && (this->hasFlags(entity_visibility_protected) || this->hasFlags(entity_visibility_private))) {
            return false;
        }
        return m_parent->getDomain()->isEntityVisibleFor(observer, *this);
    }

    //First find the domain which contains the observer, as well as if the observer has a domain itself.
    const LocatedEntity* domainEntity = observer.m_parent;
    const LocatedEntity* topObserverEntity = &observer;
    const Domain* observerParentDomain = nullptr;

    while (domainEntity != nullptr) {
        // If the observer is a child of this, and there are no domains in between, viewing is always allowed.
        // This applies even for protected and private domains, at least for now
        if (domainEntity == this) {
            return true;
        }

        observerParentDomain = domainEntity->getDomain();
        if (observerParentDomain) {
            break;
        }
        topObserverEntity = domainEntity;
        domainEntity = domainEntity->m_parent;
    }

    //The parent entity of the observer (possible null), although unlikely.

    auto* observerParentEntity = observer.m_parent;
    //The domain of the observer (possible null).
    const Domain* observerOwnDomain = observer.getDomain();

    //Now walk upwards from the entity being looked at until we reach either the observer's parent domain entity,
    //or the observer itself
    std::vector<const LocatedEntity*> toAncestors;
    toAncestors.reserve(4); //four seems like a suitable number
    const LocatedEntity* ancestorEntity = this;
    const Domain* ancestorDomain = nullptr;

    while (true) {
        toAncestors.push_back(ancestorEntity);

        if (ancestorEntity == &observer) {
            ancestorDomain = observerOwnDomain;
            break;
        }
        if (ancestorEntity == observerParentEntity) {
            ancestorDomain = observerParentDomain;
            break;
        }
        if (ancestorEntity == topObserverEntity) {
            break;
        }
        ancestorEntity = ancestorEntity->m_parent;
        if (ancestorEntity == nullptr) {
            //Could find no common ancestor; can't be seen.
            return false;
        }
    }

    //Now walk back down the toAncestors list, checking if all entities on the way can be seen.
    //Visibility is only checked for the first immediate child of a domain entity, further grandchildren are considered visible if the top one is, until
    //another domain is reached.
    for (auto I = toAncestors.rbegin(); I != toAncestors.rend(); ++I) {
        const LocatedEntity* ancestor = *I;
        if (ancestorDomain) {
            if (!ancestorDomain->isEntityVisibleFor(observer, *ancestor)) {
                return false;
            }
        }
        if (ancestor->hasFlags(entity_visibility_private) && !observer.hasFlags(entity_admin)) {
            return false;
        }

        if (ancestor->hasFlags(entity_visibility_protected) && &observer != ancestor->m_parent && !observer.hasFlags(entity_admin)) {
            return false;
        }

        ancestorDomain = ancestor->getDomain();
    }
    return true;
}

bool LocatedEntity::canReach(const EntityLocation& entityLocation, float extraReach) const
{
    auto reachingEntity = this;
    //Are we reaching for our ourselves?
    if (reachingEntity == entityLocation.m_parent.get()) {
        return true;
    }

    //Is the reaching entity an admin?
    if (reachingEntity->hasFlags(entity_admin)) {
        return true;
    }

    //If either reacher or destination is destroyed then nothing can be reached.
    if (isDestroyed() || entityLocation.m_parent->isDestroyed()) {
        return false;
    }

    double reachDistance = 0;
    auto reachProp = reachingEntity->getPropertyType<double>("reach");
    if (reachProp) {
        reachDistance = reachProp->data();
    }
    reachDistance += extraReach;

    //First find the domain which contains the reacher, as well as if the reacher has a domain itself.
    const LocatedEntity* domainEntity = reachingEntity->m_parent;
    const LocatedEntity* topReachingEntity = reachingEntity;
    const Domain* reacherParentDomain = nullptr;
    const LocatedEntity* reacherDomainEntity = nullptr; //The entity which contains the reacher's domain

    while (domainEntity != nullptr) {
        reacherParentDomain = domainEntity->getDomain();
        if (reacherParentDomain) {
            reacherDomainEntity = domainEntity;
            break;
        }
        topReachingEntity = domainEntity;
        domainEntity = domainEntity->m_parent;
    }

    //Now walk upwards from the entity being reached for until we reach either the reacher's parent domain entity,
    //or the reacher itself
    std::vector<const LocatedEntity*> toAncestors;
    toAncestors.reserve(4);
    auto ancestorEntity = entityLocation.m_parent;

    while (true) {
        if (ancestorEntity == reacherDomainEntity) {
            if (ancestorEntity == entityLocation.m_parent) {
                //We're trying to reach our containing domain entity, handle separately
                return !(reacherParentDomain && !reacherParentDomain->isEntityReachable(*reachingEntity, reachDistance, *reacherDomainEntity, entityLocation.m_pos));
            }
            break;
        }
        if (ancestorEntity == reachingEntity) {
            break;
        }

        toAncestors.push_back(ancestorEntity.get());

        if (ancestorEntity == topReachingEntity) {
            break;
        }
        ancestorEntity = ancestorEntity->m_parent;
        if (ancestorEntity == nullptr) {
            //Could find no common ancestor; can't be reached.
            return false;
        }
    }

    if (toAncestors.empty()) {
        return false;
    }

    //Now walk back down the toAncestors list, checking if all entities on the way can be reached.
    //Reachability is only checked for the first immediate child of a domain entity, further grandchildren are considered reachable if the top one is, until
    //another domain is reached.
    for (auto I = toAncestors.rbegin(); I != toAncestors.rend(); ++I) {
        const LocatedEntity* ancestor = *I;
        auto domain = ancestor->m_parent ? ancestor->m_parent->getDomain() : nullptr;
        if (domain && !domain->isEntityReachable(*reachingEntity, reachDistance, *ancestor, entityLocation.m_pos)) {
            return false;
        }
    }
    return true;
}

/// \brief Read attributes from an Atlas element
///
/// @param ent The Atlas map element containing the attribute values
void LocatedEntity::merge(const MapType& ent)
{
    //Any modifiers that are not "default" should be applied lastly, after default values are applied.
    std::vector<std::pair<std::string, std::unique_ptr<Modifier>>> modifiedAttributes;
    for (auto& entry : ent) {
        const std::string& key = entry.first;
        if (key.empty()) {
            continue;
        }

        auto parsedPropertyName = PropertyUtil::parsePropertyModification(key);
        auto modifier = Modifier::createModifier(parsedPropertyName.first, entry.second);

        if (s_immutable.find(parsedPropertyName.second) != s_immutable.end()) {
            continue;
        }
        if (modifier->getType() == ModifierType::Default) {
            setAttr(key, modifier.get());
        } else {
            modifiedAttributes.emplace_back(std::make_pair(parsedPropertyName.second, std::move(modifier)));
        }
    }

    for (auto& entry: modifiedAttributes) {
        setAttr(entry.first, entry.second.get());
    }
}

std::string LocatedEntity::describeEntity() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

void LocatedEntity::enqueueUpdateOp(OpVector& res) {
    if (!hasFlags(entity_update_broadcast_queued)) {
        Update update;
        update->setTo(getId());
        res.push_back(std::move(update));

        addFlags(entity_update_broadcast_queued);
    }
}

void LocatedEntity::enqueueUpdateOp() {
    if (!hasFlags(entity_update_broadcast_queued)) {
        Update update;
        update->setTo(getId());
        sendWorld(std::move(update));

        addFlags(entity_update_broadcast_queued);
    }
}


std::ostream& operator<<(std::ostream& s, const LocatedEntity& d)
{
    auto name = d.getAttrType("name", Element::TYPE_STRING);
    s << d.getId();
    if (d.m_type) {
        s << "(" << d.m_type->name();
        if (name) {
            s << ",'" << name->String() << "'";
        }
        s << ")";
    } else {
        if (name) {
            s << "('" << name->String() << "')";
        }
    }
    return s;
}
