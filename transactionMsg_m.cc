//
// Generated file, do not edit! Created by opp_msgtool 6.0 from transactionMsg.msg.
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
#include "transactionMsg_m.h"

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

class DoubleVectorDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
    };
  public:
    DoubleVectorDescriptor();
    virtual ~DoubleVectorDescriptor();

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

Register_ClassDescriptor(DoubleVectorDescriptor)

DoubleVectorDescriptor::DoubleVectorDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(DoubleVector)), "")
{
    propertyNames = nullptr;
}

DoubleVectorDescriptor::~DoubleVectorDescriptor()
{
    delete[] propertyNames;
}

bool DoubleVectorDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<DoubleVector *>(obj)!=nullptr;
}

const char **DoubleVectorDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *DoubleVectorDescriptor::getProperty(const char *propertyName) const
{
    if (!strcmp(propertyName, "existingClass")) return "";
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int DoubleVectorDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 0+base->getFieldCount() : 0;
}

unsigned int DoubleVectorDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    return 0;
}

const char *DoubleVectorDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

int DoubleVectorDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->findField(fieldName) : -1;
}

const char *DoubleVectorDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

const char **DoubleVectorDescriptor::getFieldPropertyNames(int field) const
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

const char *DoubleVectorDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int DoubleVectorDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void DoubleVectorDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'DoubleVector'", field);
    }
}

const char *DoubleVectorDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string DoubleVectorDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: return "";
    }
}

void DoubleVectorDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'DoubleVector'", field);
    }
}

omnetpp::cValue DoubleVectorDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'DoubleVector' as cValue -- field index out of range?", field);
    }
}

void DoubleVectorDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'DoubleVector'", field);
    }
}

const char *DoubleVectorDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

omnetpp::any_ptr DoubleVectorDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void DoubleVectorDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    DoubleVector *pp = omnetpp::fromAnyPtr<DoubleVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'DoubleVector'", field);
    }
}

Register_Class(transactionMsg)

transactionMsg::transactionMsg(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

transactionMsg::transactionMsg(const transactionMsg& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

transactionMsg::~transactionMsg()
{
}

transactionMsg& transactionMsg::operator=(const transactionMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void transactionMsg::copy(const transactionMsg& other)
{
    this->amount = other.amount;
    this->timeSent = other.timeSent;
    this->sender = other.sender;
    this->receiver = other.receiver;
    this->priorityClass = other.priorityClass;
    this->transactionId = other.transactionId;
    this->hasTimeOut = other.hasTimeOut;
    this->timeOut = other.timeOut;
    this->htlcIndex = other.htlcIndex;
    this->pathIndex = other.pathIndex;
    this->isAttempted_ = other.isAttempted_;
    this->largerTxnId = other.largerTxnId;
    this->isMarked_ = other.isMarked_;
    this->timeAttempted = other.timeAttempted;
    this->total_amount = other.total_amount;
    this->tx_fee_amount = other.tx_fee_amount;
    this->convolution_flow = other.convolution_flow;
    this->tx_fee_total = other.tx_fee_total;
}

void transactionMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->amount);
    doParsimPacking(b,this->timeSent);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->receiver);
    doParsimPacking(b,this->priorityClass);
    doParsimPacking(b,this->transactionId);
    doParsimPacking(b,this->hasTimeOut);
    doParsimPacking(b,this->timeOut);
    doParsimPacking(b,this->htlcIndex);
    doParsimPacking(b,this->pathIndex);
    doParsimPacking(b,this->isAttempted_);
    doParsimPacking(b,this->largerTxnId);
    doParsimPacking(b,this->isMarked_);
    doParsimPacking(b,this->timeAttempted);
    doParsimPacking(b,this->total_amount);
    doParsimPacking(b,this->tx_fee_amount);
    doParsimPacking(b,this->convolution_flow);
    doParsimPacking(b,this->tx_fee_total);
}

void transactionMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->amount);
    doParsimUnpacking(b,this->timeSent);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->receiver);
    doParsimUnpacking(b,this->priorityClass);
    doParsimUnpacking(b,this->transactionId);
    doParsimUnpacking(b,this->hasTimeOut);
    doParsimUnpacking(b,this->timeOut);
    doParsimUnpacking(b,this->htlcIndex);
    doParsimUnpacking(b,this->pathIndex);
    doParsimUnpacking(b,this->isAttempted_);
    doParsimUnpacking(b,this->largerTxnId);
    doParsimUnpacking(b,this->isMarked_);
    doParsimUnpacking(b,this->timeAttempted);
    doParsimUnpacking(b,this->total_amount);
    doParsimUnpacking(b,this->tx_fee_amount);
    doParsimUnpacking(b,this->convolution_flow);
    doParsimUnpacking(b,this->tx_fee_total);
}

double transactionMsg::getAmount() const
{
    return this->amount;
}

void transactionMsg::setAmount(double amount)
{
    this->amount = amount;
}

double transactionMsg::getTimeSent() const
{
    return this->timeSent;
}

void transactionMsg::setTimeSent(double timeSent)
{
    this->timeSent = timeSent;
}

int transactionMsg::getSender() const
{
    return this->sender;
}

void transactionMsg::setSender(int sender)
{
    this->sender = sender;
}

int transactionMsg::getReceiver() const
{
    return this->receiver;
}

void transactionMsg::setReceiver(int receiver)
{
    this->receiver = receiver;
}

int transactionMsg::getPriorityClass() const
{
    return this->priorityClass;
}

void transactionMsg::setPriorityClass(int priorityClass)
{
    this->priorityClass = priorityClass;
}

int transactionMsg::getTransactionId() const
{
    return this->transactionId;
}

void transactionMsg::setTransactionId(int transactionId)
{
    this->transactionId = transactionId;
}

bool transactionMsg::getHasTimeOut() const
{
    return this->hasTimeOut;
}

void transactionMsg::setHasTimeOut(bool hasTimeOut)
{
    this->hasTimeOut = hasTimeOut;
}

double transactionMsg::getTimeOut() const
{
    return this->timeOut;
}

void transactionMsg::setTimeOut(double timeOut)
{
    this->timeOut = timeOut;
}

int transactionMsg::getHtlcIndex() const
{
    return this->htlcIndex;
}

void transactionMsg::setHtlcIndex(int htlcIndex)
{
    this->htlcIndex = htlcIndex;
}

int transactionMsg::getPathIndex() const
{
    return this->pathIndex;
}

void transactionMsg::setPathIndex(int pathIndex)
{
    this->pathIndex = pathIndex;
}

bool transactionMsg::isAttempted() const
{
    return this->isAttempted_;
}

void transactionMsg::setIsAttempted(bool isAttempted)
{
    this->isAttempted_ = isAttempted;
}

double transactionMsg::getLargerTxnId() const
{
    return this->largerTxnId;
}

void transactionMsg::setLargerTxnId(double largerTxnId)
{
    this->largerTxnId = largerTxnId;
}

bool transactionMsg::isMarked() const
{
    return this->isMarked_;
}

void transactionMsg::setIsMarked(bool isMarked)
{
    this->isMarked_ = isMarked;
}

double transactionMsg::getTimeAttempted() const
{
    return this->timeAttempted;
}

void transactionMsg::setTimeAttempted(double timeAttempted)
{
    this->timeAttempted = timeAttempted;
}

double transactionMsg::getTotal_amount() const
{
    return this->total_amount;
}

void transactionMsg::setTotal_amount(double total_amount)
{
    this->total_amount = total_amount;
}

double transactionMsg::getTx_fee_amount() const
{
    return this->tx_fee_amount;
}

void transactionMsg::setTx_fee_amount(double tx_fee_amount)
{
    this->tx_fee_amount = tx_fee_amount;
}

const DoubleVector& transactionMsg::getConvolution_flow() const
{
    return this->convolution_flow;
}

void transactionMsg::setConvolution_flow(const DoubleVector& convolution_flow)
{
    this->convolution_flow = convolution_flow;
}

double transactionMsg::getTx_fee_total() const
{
    return this->tx_fee_total;
}

void transactionMsg::setTx_fee_total(double tx_fee_total)
{
    this->tx_fee_total = tx_fee_total;
}

class transactionMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_amount,
        FIELD_timeSent,
        FIELD_sender,
        FIELD_receiver,
        FIELD_priorityClass,
        FIELD_transactionId,
        FIELD_hasTimeOut,
        FIELD_timeOut,
        FIELD_htlcIndex,
        FIELD_pathIndex,
        FIELD_isAttempted,
        FIELD_largerTxnId,
        FIELD_isMarked,
        FIELD_timeAttempted,
        FIELD_total_amount,
        FIELD_tx_fee_amount,
        FIELD_convolution_flow,
        FIELD_tx_fee_total,
    };
  public:
    transactionMsgDescriptor();
    virtual ~transactionMsgDescriptor();

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

Register_ClassDescriptor(transactionMsgDescriptor)

transactionMsgDescriptor::transactionMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(transactionMsg)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

transactionMsgDescriptor::~transactionMsgDescriptor()
{
    delete[] propertyNames;
}

bool transactionMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<transactionMsg *>(obj)!=nullptr;
}

const char **transactionMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *transactionMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int transactionMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 18+base->getFieldCount() : 18;
}

unsigned int transactionMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_amount
        FD_ISEDITABLE,    // FIELD_timeSent
        FD_ISEDITABLE,    // FIELD_sender
        FD_ISEDITABLE,    // FIELD_receiver
        FD_ISEDITABLE,    // FIELD_priorityClass
        FD_ISEDITABLE,    // FIELD_transactionId
        FD_ISEDITABLE,    // FIELD_hasTimeOut
        FD_ISEDITABLE,    // FIELD_timeOut
        FD_ISEDITABLE,    // FIELD_htlcIndex
        FD_ISEDITABLE,    // FIELD_pathIndex
        FD_ISEDITABLE,    // FIELD_isAttempted
        FD_ISEDITABLE,    // FIELD_largerTxnId
        FD_ISEDITABLE,    // FIELD_isMarked
        FD_ISEDITABLE,    // FIELD_timeAttempted
        FD_ISEDITABLE,    // FIELD_total_amount
        FD_ISEDITABLE,    // FIELD_tx_fee_amount
        FD_ISCOMPOUND,    // FIELD_convolution_flow
        FD_ISEDITABLE,    // FIELD_tx_fee_total
    };
    return (field >= 0 && field < 18) ? fieldTypeFlags[field] : 0;
}

const char *transactionMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "amount",
        "timeSent",
        "sender",
        "receiver",
        "priorityClass",
        "transactionId",
        "hasTimeOut",
        "timeOut",
        "htlcIndex",
        "pathIndex",
        "isAttempted",
        "largerTxnId",
        "isMarked",
        "timeAttempted",
        "total_amount",
        "tx_fee_amount",
        "convolution_flow",
        "tx_fee_total",
    };
    return (field >= 0 && field < 18) ? fieldNames[field] : nullptr;
}

int transactionMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "amount") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "timeSent") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "sender") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "receiver") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "priorityClass") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "transactionId") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "hasTimeOut") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "timeOut") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "htlcIndex") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "pathIndex") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "isAttempted") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "largerTxnId") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "isMarked") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "timeAttempted") == 0) return baseIndex + 13;
    if (strcmp(fieldName, "total_amount") == 0) return baseIndex + 14;
    if (strcmp(fieldName, "tx_fee_amount") == 0) return baseIndex + 15;
    if (strcmp(fieldName, "convolution_flow") == 0) return baseIndex + 16;
    if (strcmp(fieldName, "tx_fee_total") == 0) return baseIndex + 17;
    return base ? base->findField(fieldName) : -1;
}

const char *transactionMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "double",    // FIELD_amount
        "double",    // FIELD_timeSent
        "int",    // FIELD_sender
        "int",    // FIELD_receiver
        "int",    // FIELD_priorityClass
        "int",    // FIELD_transactionId
        "bool",    // FIELD_hasTimeOut
        "double",    // FIELD_timeOut
        "int",    // FIELD_htlcIndex
        "int",    // FIELD_pathIndex
        "bool",    // FIELD_isAttempted
        "double",    // FIELD_largerTxnId
        "bool",    // FIELD_isMarked
        "double",    // FIELD_timeAttempted
        "double",    // FIELD_total_amount
        "double",    // FIELD_tx_fee_amount
        "DoubleVector",    // FIELD_convolution_flow
        "double",    // FIELD_tx_fee_total
    };
    return (field >= 0 && field < 18) ? fieldTypeStrings[field] : nullptr;
}

const char **transactionMsgDescriptor::getFieldPropertyNames(int field) const
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

const char *transactionMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int transactionMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void transactionMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'transactionMsg'", field);
    }
}

const char *transactionMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string transactionMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_amount: return double2string(pp->getAmount());
        case FIELD_timeSent: return double2string(pp->getTimeSent());
        case FIELD_sender: return long2string(pp->getSender());
        case FIELD_receiver: return long2string(pp->getReceiver());
        case FIELD_priorityClass: return long2string(pp->getPriorityClass());
        case FIELD_transactionId: return long2string(pp->getTransactionId());
        case FIELD_hasTimeOut: return bool2string(pp->getHasTimeOut());
        case FIELD_timeOut: return double2string(pp->getTimeOut());
        case FIELD_htlcIndex: return long2string(pp->getHtlcIndex());
        case FIELD_pathIndex: return long2string(pp->getPathIndex());
        case FIELD_isAttempted: return bool2string(pp->isAttempted());
        case FIELD_largerTxnId: return double2string(pp->getLargerTxnId());
        case FIELD_isMarked: return bool2string(pp->isMarked());
        case FIELD_timeAttempted: return double2string(pp->getTimeAttempted());
        case FIELD_total_amount: return double2string(pp->getTotal_amount());
        case FIELD_tx_fee_amount: return double2string(pp->getTx_fee_amount());
        case FIELD_convolution_flow: return "";
        case FIELD_tx_fee_total: return double2string(pp->getTx_fee_total());
        default: return "";
    }
}

void transactionMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_amount: pp->setAmount(string2double(value)); break;
        case FIELD_timeSent: pp->setTimeSent(string2double(value)); break;
        case FIELD_sender: pp->setSender(string2long(value)); break;
        case FIELD_receiver: pp->setReceiver(string2long(value)); break;
        case FIELD_priorityClass: pp->setPriorityClass(string2long(value)); break;
        case FIELD_transactionId: pp->setTransactionId(string2long(value)); break;
        case FIELD_hasTimeOut: pp->setHasTimeOut(string2bool(value)); break;
        case FIELD_timeOut: pp->setTimeOut(string2double(value)); break;
        case FIELD_htlcIndex: pp->setHtlcIndex(string2long(value)); break;
        case FIELD_pathIndex: pp->setPathIndex(string2long(value)); break;
        case FIELD_isAttempted: pp->setIsAttempted(string2bool(value)); break;
        case FIELD_largerTxnId: pp->setLargerTxnId(string2double(value)); break;
        case FIELD_isMarked: pp->setIsMarked(string2bool(value)); break;
        case FIELD_timeAttempted: pp->setTimeAttempted(string2double(value)); break;
        case FIELD_total_amount: pp->setTotal_amount(string2double(value)); break;
        case FIELD_tx_fee_amount: pp->setTx_fee_amount(string2double(value)); break;
        case FIELD_tx_fee_total: pp->setTx_fee_total(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'transactionMsg'", field);
    }
}

omnetpp::cValue transactionMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_amount: return pp->getAmount();
        case FIELD_timeSent: return pp->getTimeSent();
        case FIELD_sender: return pp->getSender();
        case FIELD_receiver: return pp->getReceiver();
        case FIELD_priorityClass: return pp->getPriorityClass();
        case FIELD_transactionId: return pp->getTransactionId();
        case FIELD_hasTimeOut: return pp->getHasTimeOut();
        case FIELD_timeOut: return pp->getTimeOut();
        case FIELD_htlcIndex: return pp->getHtlcIndex();
        case FIELD_pathIndex: return pp->getPathIndex();
        case FIELD_isAttempted: return pp->isAttempted();
        case FIELD_largerTxnId: return pp->getLargerTxnId();
        case FIELD_isMarked: return pp->isMarked();
        case FIELD_timeAttempted: return pp->getTimeAttempted();
        case FIELD_total_amount: return pp->getTotal_amount();
        case FIELD_tx_fee_amount: return pp->getTx_fee_amount();
        case FIELD_convolution_flow: return omnetpp::toAnyPtr(&pp->getConvolution_flow()); break;
        case FIELD_tx_fee_total: return pp->getTx_fee_total();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'transactionMsg' as cValue -- field index out of range?", field);
    }
}

void transactionMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_amount: pp->setAmount(value.doubleValue()); break;
        case FIELD_timeSent: pp->setTimeSent(value.doubleValue()); break;
        case FIELD_sender: pp->setSender(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_receiver: pp->setReceiver(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_priorityClass: pp->setPriorityClass(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_transactionId: pp->setTransactionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_hasTimeOut: pp->setHasTimeOut(value.boolValue()); break;
        case FIELD_timeOut: pp->setTimeOut(value.doubleValue()); break;
        case FIELD_htlcIndex: pp->setHtlcIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_pathIndex: pp->setPathIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_isAttempted: pp->setIsAttempted(value.boolValue()); break;
        case FIELD_largerTxnId: pp->setLargerTxnId(value.doubleValue()); break;
        case FIELD_isMarked: pp->setIsMarked(value.boolValue()); break;
        case FIELD_timeAttempted: pp->setTimeAttempted(value.doubleValue()); break;
        case FIELD_total_amount: pp->setTotal_amount(value.doubleValue()); break;
        case FIELD_tx_fee_amount: pp->setTx_fee_amount(value.doubleValue()); break;
        case FIELD_tx_fee_total: pp->setTx_fee_total(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'transactionMsg'", field);
    }
}

const char *transactionMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_convolution_flow: return omnetpp::opp_typename(typeid(DoubleVector));
        default: return nullptr;
    };
}

omnetpp::any_ptr transactionMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_convolution_flow: return omnetpp::toAnyPtr(&pp->getConvolution_flow()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void transactionMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    transactionMsg *pp = omnetpp::fromAnyPtr<transactionMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'transactionMsg'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

