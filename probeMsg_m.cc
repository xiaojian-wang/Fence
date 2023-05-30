//
// Generated file, do not edit! Created by opp_msgtool 6.0 from probeMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "probeMsg_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(probeMsg)

probeMsg::probeMsg(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

probeMsg::probeMsg(const probeMsg& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

probeMsg::~probeMsg()
{
}

probeMsg& probeMsg::operator=(const probeMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void probeMsg::copy(const probeMsg& other)
{
    this->pathIndex = other.pathIndex;
    this->sender = other.sender;
    this->receiver = other.receiver;
    this->isReversed_ = other.isReversed_;
    this->pathBalances = other.pathBalances;
    this->path = other.path;
    this->transactionId = other.transactionId;
}

void probeMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->pathIndex);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->receiver);
    doParsimPacking(b,this->isReversed_);
    doParsimPacking(b,this->pathBalances);
    doParsimPacking(b,this->path);
    doParsimPacking(b,this->transactionId);
}

void probeMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->pathIndex);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->receiver);
    doParsimUnpacking(b,this->isReversed_);
    doParsimUnpacking(b,this->pathBalances);
    doParsimUnpacking(b,this->path);
    doParsimUnpacking(b,this->transactionId);
}

int probeMsg::getPathIndex() const
{
    return this->pathIndex;
}

void probeMsg::setPathIndex(int pathIndex)
{
    this->pathIndex = pathIndex;
}

int probeMsg::getSender() const
{
    return this->sender;
}

void probeMsg::setSender(int sender)
{
    this->sender = sender;
}

int probeMsg::getReceiver() const
{
    return this->receiver;
}

void probeMsg::setReceiver(int receiver)
{
    this->receiver = receiver;
}

bool probeMsg::isReversed() const
{
    return this->isReversed_;
}

void probeMsg::setIsReversed(bool isReversed)
{
    this->isReversed_ = isReversed;
}

const DoubleVector& probeMsg::getPathBalances() const
{
    return this->pathBalances;
}

void probeMsg::setPathBalances(const DoubleVector& pathBalances)
{
    this->pathBalances = pathBalances;
}

const IntVector& probeMsg::getPath() const
{
    return this->path;
}

void probeMsg::setPath(const IntVector& path)
{
    this->path = path;
}

int probeMsg::getTransactionId() const
{
    return this->transactionId;
}

void probeMsg::setTransactionId(int transactionId)
{
    this->transactionId = transactionId;
}

class probeMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_pathIndex,
        FIELD_sender,
        FIELD_receiver,
        FIELD_isReversed,
        FIELD_pathBalances,
        FIELD_path,
        FIELD_transactionId,
    };
  public:
    probeMsgDescriptor();
    virtual ~probeMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(probeMsgDescriptor)

probeMsgDescriptor::probeMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(probeMsg)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

probeMsgDescriptor::~probeMsgDescriptor()
{
    delete[] propertyNames;
}

bool probeMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<probeMsg *>(obj)!=nullptr;
}

const char **probeMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *probeMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int probeMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 7+base->getFieldCount() : 7;
}

unsigned int probeMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_pathIndex
        FD_ISEDITABLE,    // FIELD_sender
        FD_ISEDITABLE,    // FIELD_receiver
        FD_ISEDITABLE,    // FIELD_isReversed
        FD_ISCOMPOUND,    // FIELD_pathBalances
        FD_ISCOMPOUND,    // FIELD_path
        FD_ISEDITABLE,    // FIELD_transactionId
    };
    return (field >= 0 && field < 7) ? fieldTypeFlags[field] : 0;
}

const char *probeMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "pathIndex",
        "sender",
        "receiver",
        "isReversed",
        "pathBalances",
        "path",
        "transactionId",
    };
    return (field >= 0 && field < 7) ? fieldNames[field] : nullptr;
}

int probeMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "pathIndex") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "sender") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "receiver") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "isReversed") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "pathBalances") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "path") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "transactionId") == 0) return baseIndex + 6;
    return base ? base->findField(fieldName) : -1;
}

const char *probeMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_pathIndex
        "int",    // FIELD_sender
        "int",    // FIELD_receiver
        "bool",    // FIELD_isReversed
        "DoubleVector",    // FIELD_pathBalances
        "IntVector",    // FIELD_path
        "int",    // FIELD_transactionId
    };
    return (field >= 0 && field < 7) ? fieldTypeStrings[field] : nullptr;
}

const char **probeMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *probeMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int probeMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void probeMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'probeMsg'", field);
    }
}

const char *probeMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string probeMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        case FIELD_pathIndex: return long2string(pp->getPathIndex());
        case FIELD_sender: return long2string(pp->getSender());
        case FIELD_receiver: return long2string(pp->getReceiver());
        case FIELD_isReversed: return bool2string(pp->isReversed());
        case FIELD_pathBalances: return "";
        case FIELD_path: return "";
        case FIELD_transactionId: return long2string(pp->getTransactionId());
        default: return "";
    }
}

void probeMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        case FIELD_pathIndex: pp->setPathIndex(string2long(value)); break;
        case FIELD_sender: pp->setSender(string2long(value)); break;
        case FIELD_receiver: pp->setReceiver(string2long(value)); break;
        case FIELD_isReversed: pp->setIsReversed(string2bool(value)); break;
        case FIELD_transactionId: pp->setTransactionId(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'probeMsg'", field);
    }
}

omnetpp::cValue probeMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        case FIELD_pathIndex: return pp->getPathIndex();
        case FIELD_sender: return pp->getSender();
        case FIELD_receiver: return pp->getReceiver();
        case FIELD_isReversed: return pp->isReversed();
        case FIELD_pathBalances: return omnetpp::toAnyPtr(&pp->getPathBalances()); break;
        case FIELD_path: return omnetpp::toAnyPtr(&pp->getPath()); break;
        case FIELD_transactionId: return pp->getTransactionId();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'probeMsg' as cValue -- field index out of range?", field);
    }
}

void probeMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        case FIELD_pathIndex: pp->setPathIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sender: pp->setSender(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_receiver: pp->setReceiver(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_isReversed: pp->setIsReversed(value.boolValue()); break;
        case FIELD_transactionId: pp->setTransactionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'probeMsg'", field);
    }
}

const char *probeMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_pathBalances: return omnetpp::opp_typename(typeid(DoubleVector));
        case FIELD_path: return omnetpp::opp_typename(typeid(IntVector));
        default: return nullptr;
    };
}

omnetpp::any_ptr probeMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        case FIELD_pathBalances: return omnetpp::toAnyPtr(&pp->getPathBalances()); break;
        case FIELD_path: return omnetpp::toAnyPtr(&pp->getPath()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void probeMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    probeMsg *pp = omnetpp::fromAnyPtr<probeMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'probeMsg'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

