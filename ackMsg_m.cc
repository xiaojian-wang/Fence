//
// Generated file, do not edit! Created by opp_msgtool 6.0 from ackMsg.msg.
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
#include "ackMsg_m.h"

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

Register_Class(ackMsg)

ackMsg::ackMsg(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

ackMsg::ackMsg(const ackMsg& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

ackMsg::~ackMsg()
{
}

ackMsg& ackMsg::operator=(const ackMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void ackMsg::copy(const ackMsg& other)
{
    this->transactionId = other.transactionId;
    this->receiver = other.receiver;
    this->htlcIndex = other.htlcIndex;
    this->pathIndex = other.pathIndex;
    this->timeSent = other.timeSent;
    this->isSuccess_ = other.isSuccess_;
    this->failedHopNum = other.failedHopNum;
    this->secret = other.secret;
    this->amount = other.amount;
    this->priorityClass = other.priorityClass;
    this->hasTimeOut = other.hasTimeOut;
    this->timeOut = other.timeOut;
    this->largerTxnId = other.largerTxnId;
    this->isMarked_ = other.isMarked_;
    this->timeAttempted = other.timeAttempted;
    this->tx_fee_amount = other.tx_fee_amount;
}

void ackMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->transactionId);
    doParsimPacking(b,this->receiver);
    doParsimPacking(b,this->htlcIndex);
    doParsimPacking(b,this->pathIndex);
    doParsimPacking(b,this->timeSent);
    doParsimPacking(b,this->isSuccess_);
    doParsimPacking(b,this->failedHopNum);
    doParsimPacking(b,this->secret);
    doParsimPacking(b,this->amount);
    doParsimPacking(b,this->priorityClass);
    doParsimPacking(b,this->hasTimeOut);
    doParsimPacking(b,this->timeOut);
    doParsimPacking(b,this->largerTxnId);
    doParsimPacking(b,this->isMarked_);
    doParsimPacking(b,this->timeAttempted);
    doParsimPacking(b,this->tx_fee_amount);
}

void ackMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->transactionId);
    doParsimUnpacking(b,this->receiver);
    doParsimUnpacking(b,this->htlcIndex);
    doParsimUnpacking(b,this->pathIndex);
    doParsimUnpacking(b,this->timeSent);
    doParsimUnpacking(b,this->isSuccess_);
    doParsimUnpacking(b,this->failedHopNum);
    doParsimUnpacking(b,this->secret);
    doParsimUnpacking(b,this->amount);
    doParsimUnpacking(b,this->priorityClass);
    doParsimUnpacking(b,this->hasTimeOut);
    doParsimUnpacking(b,this->timeOut);
    doParsimUnpacking(b,this->largerTxnId);
    doParsimUnpacking(b,this->isMarked_);
    doParsimUnpacking(b,this->timeAttempted);
    doParsimUnpacking(b,this->tx_fee_amount);
}

int ackMsg::getTransactionId() const
{
    return this->transactionId;
}

void ackMsg::setTransactionId(int transactionId)
{
    this->transactionId = transactionId;
}

int ackMsg::getReceiver() const
{
    return this->receiver;
}

void ackMsg::setReceiver(int receiver)
{
    this->receiver = receiver;
}

int ackMsg::getHtlcIndex() const
{
    return this->htlcIndex;
}

void ackMsg::setHtlcIndex(int htlcIndex)
{
    this->htlcIndex = htlcIndex;
}

int ackMsg::getPathIndex() const
{
    return this->pathIndex;
}

void ackMsg::setPathIndex(int pathIndex)
{
    this->pathIndex = pathIndex;
}

double ackMsg::getTimeSent() const
{
    return this->timeSent;
}

void ackMsg::setTimeSent(double timeSent)
{
    this->timeSent = timeSent;
}

bool ackMsg::isSuccess() const
{
    return this->isSuccess_;
}

void ackMsg::setIsSuccess(bool isSuccess)
{
    this->isSuccess_ = isSuccess;
}

int ackMsg::getFailedHopNum() const
{
    return this->failedHopNum;
}

void ackMsg::setFailedHopNum(int failedHopNum)
{
    this->failedHopNum = failedHopNum;
}

const char * ackMsg::getSecret() const
{
    return this->secret.c_str();
}

void ackMsg::setSecret(const char * secret)
{
    this->secret = secret;
}

double ackMsg::getAmount() const
{
    return this->amount;
}

void ackMsg::setAmount(double amount)
{
    this->amount = amount;
}

int ackMsg::getPriorityClass() const
{
    return this->priorityClass;
}

void ackMsg::setPriorityClass(int priorityClass)
{
    this->priorityClass = priorityClass;
}

bool ackMsg::getHasTimeOut() const
{
    return this->hasTimeOut;
}

void ackMsg::setHasTimeOut(bool hasTimeOut)
{
    this->hasTimeOut = hasTimeOut;
}

double ackMsg::getTimeOut() const
{
    return this->timeOut;
}

void ackMsg::setTimeOut(double timeOut)
{
    this->timeOut = timeOut;
}

double ackMsg::getLargerTxnId() const
{
    return this->largerTxnId;
}

void ackMsg::setLargerTxnId(double largerTxnId)
{
    this->largerTxnId = largerTxnId;
}

bool ackMsg::isMarked() const
{
    return this->isMarked_;
}

void ackMsg::setIsMarked(bool isMarked)
{
    this->isMarked_ = isMarked;
}

double ackMsg::getTimeAttempted() const
{
    return this->timeAttempted;
}

void ackMsg::setTimeAttempted(double timeAttempted)
{
    this->timeAttempted = timeAttempted;
}

double ackMsg::getTx_fee_amount() const
{
    return this->tx_fee_amount;
}

void ackMsg::setTx_fee_amount(double tx_fee_amount)
{
    this->tx_fee_amount = tx_fee_amount;
}

class ackMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_transactionId,
        FIELD_receiver,
        FIELD_htlcIndex,
        FIELD_pathIndex,
        FIELD_timeSent,
        FIELD_isSuccess,
        FIELD_failedHopNum,
        FIELD_secret,
        FIELD_amount,
        FIELD_priorityClass,
        FIELD_hasTimeOut,
        FIELD_timeOut,
        FIELD_largerTxnId,
        FIELD_isMarked,
        FIELD_timeAttempted,
        FIELD_tx_fee_amount,
    };
  public:
    ackMsgDescriptor();
    virtual ~ackMsgDescriptor();

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

Register_ClassDescriptor(ackMsgDescriptor)

ackMsgDescriptor::ackMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ackMsg)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

ackMsgDescriptor::~ackMsgDescriptor()
{
    delete[] propertyNames;
}

bool ackMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ackMsg *>(obj)!=nullptr;
}

const char **ackMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ackMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ackMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 16+base->getFieldCount() : 16;
}

unsigned int ackMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_transactionId
        FD_ISEDITABLE,    // FIELD_receiver
        FD_ISEDITABLE,    // FIELD_htlcIndex
        FD_ISEDITABLE,    // FIELD_pathIndex
        FD_ISEDITABLE,    // FIELD_timeSent
        FD_ISEDITABLE,    // FIELD_isSuccess
        FD_ISEDITABLE,    // FIELD_failedHopNum
        FD_ISEDITABLE,    // FIELD_secret
        FD_ISEDITABLE,    // FIELD_amount
        FD_ISEDITABLE,    // FIELD_priorityClass
        FD_ISEDITABLE,    // FIELD_hasTimeOut
        FD_ISEDITABLE,    // FIELD_timeOut
        FD_ISEDITABLE,    // FIELD_largerTxnId
        FD_ISEDITABLE,    // FIELD_isMarked
        FD_ISEDITABLE,    // FIELD_timeAttempted
        FD_ISEDITABLE,    // FIELD_tx_fee_amount
    };
    return (field >= 0 && field < 16) ? fieldTypeFlags[field] : 0;
}

const char *ackMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "transactionId",
        "receiver",
        "htlcIndex",
        "pathIndex",
        "timeSent",
        "isSuccess",
        "failedHopNum",
        "secret",
        "amount",
        "priorityClass",
        "hasTimeOut",
        "timeOut",
        "largerTxnId",
        "isMarked",
        "timeAttempted",
        "tx_fee_amount",
    };
    return (field >= 0 && field < 16) ? fieldNames[field] : nullptr;
}

int ackMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "transactionId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "receiver") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "htlcIndex") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "pathIndex") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "timeSent") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "isSuccess") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "failedHopNum") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "secret") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "amount") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "priorityClass") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "hasTimeOut") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "timeOut") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "largerTxnId") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "isMarked") == 0) return baseIndex + 13;
    if (strcmp(fieldName, "timeAttempted") == 0) return baseIndex + 14;
    if (strcmp(fieldName, "tx_fee_amount") == 0) return baseIndex + 15;
    return base ? base->findField(fieldName) : -1;
}

const char *ackMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_transactionId
        "int",    // FIELD_receiver
        "int",    // FIELD_htlcIndex
        "int",    // FIELD_pathIndex
        "double",    // FIELD_timeSent
        "bool",    // FIELD_isSuccess
        "int",    // FIELD_failedHopNum
        "string",    // FIELD_secret
        "double",    // FIELD_amount
        "int",    // FIELD_priorityClass
        "bool",    // FIELD_hasTimeOut
        "double",    // FIELD_timeOut
        "double",    // FIELD_largerTxnId
        "bool",    // FIELD_isMarked
        "double",    // FIELD_timeAttempted
        "double",    // FIELD_tx_fee_amount
    };
    return (field >= 0 && field < 16) ? fieldTypeStrings[field] : nullptr;
}

const char **ackMsgDescriptor::getFieldPropertyNames(int field) const
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

const char *ackMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int ackMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ackMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ackMsg'", field);
    }
}

const char *ackMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ackMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        case FIELD_transactionId: return long2string(pp->getTransactionId());
        case FIELD_receiver: return long2string(pp->getReceiver());
        case FIELD_htlcIndex: return long2string(pp->getHtlcIndex());
        case FIELD_pathIndex: return long2string(pp->getPathIndex());
        case FIELD_timeSent: return double2string(pp->getTimeSent());
        case FIELD_isSuccess: return bool2string(pp->isSuccess());
        case FIELD_failedHopNum: return long2string(pp->getFailedHopNum());
        case FIELD_secret: return oppstring2string(pp->getSecret());
        case FIELD_amount: return double2string(pp->getAmount());
        case FIELD_priorityClass: return long2string(pp->getPriorityClass());
        case FIELD_hasTimeOut: return bool2string(pp->getHasTimeOut());
        case FIELD_timeOut: return double2string(pp->getTimeOut());
        case FIELD_largerTxnId: return double2string(pp->getLargerTxnId());
        case FIELD_isMarked: return bool2string(pp->isMarked());
        case FIELD_timeAttempted: return double2string(pp->getTimeAttempted());
        case FIELD_tx_fee_amount: return double2string(pp->getTx_fee_amount());
        default: return "";
    }
}

void ackMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        case FIELD_transactionId: pp->setTransactionId(string2long(value)); break;
        case FIELD_receiver: pp->setReceiver(string2long(value)); break;
        case FIELD_htlcIndex: pp->setHtlcIndex(string2long(value)); break;
        case FIELD_pathIndex: pp->setPathIndex(string2long(value)); break;
        case FIELD_timeSent: pp->setTimeSent(string2double(value)); break;
        case FIELD_isSuccess: pp->setIsSuccess(string2bool(value)); break;
        case FIELD_failedHopNum: pp->setFailedHopNum(string2long(value)); break;
        case FIELD_secret: pp->setSecret((value)); break;
        case FIELD_amount: pp->setAmount(string2double(value)); break;
        case FIELD_priorityClass: pp->setPriorityClass(string2long(value)); break;
        case FIELD_hasTimeOut: pp->setHasTimeOut(string2bool(value)); break;
        case FIELD_timeOut: pp->setTimeOut(string2double(value)); break;
        case FIELD_largerTxnId: pp->setLargerTxnId(string2double(value)); break;
        case FIELD_isMarked: pp->setIsMarked(string2bool(value)); break;
        case FIELD_timeAttempted: pp->setTimeAttempted(string2double(value)); break;
        case FIELD_tx_fee_amount: pp->setTx_fee_amount(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ackMsg'", field);
    }
}

omnetpp::cValue ackMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        case FIELD_transactionId: return pp->getTransactionId();
        case FIELD_receiver: return pp->getReceiver();
        case FIELD_htlcIndex: return pp->getHtlcIndex();
        case FIELD_pathIndex: return pp->getPathIndex();
        case FIELD_timeSent: return pp->getTimeSent();
        case FIELD_isSuccess: return pp->isSuccess();
        case FIELD_failedHopNum: return pp->getFailedHopNum();
        case FIELD_secret: return pp->getSecret();
        case FIELD_amount: return pp->getAmount();
        case FIELD_priorityClass: return pp->getPriorityClass();
        case FIELD_hasTimeOut: return pp->getHasTimeOut();
        case FIELD_timeOut: return pp->getTimeOut();
        case FIELD_largerTxnId: return pp->getLargerTxnId();
        case FIELD_isMarked: return pp->isMarked();
        case FIELD_timeAttempted: return pp->getTimeAttempted();
        case FIELD_tx_fee_amount: return pp->getTx_fee_amount();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ackMsg' as cValue -- field index out of range?", field);
    }
}

void ackMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        case FIELD_transactionId: pp->setTransactionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_receiver: pp->setReceiver(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_htlcIndex: pp->setHtlcIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_pathIndex: pp->setPathIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_timeSent: pp->setTimeSent(value.doubleValue()); break;
        case FIELD_isSuccess: pp->setIsSuccess(value.boolValue()); break;
        case FIELD_failedHopNum: pp->setFailedHopNum(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_secret: pp->setSecret(value.stringValue()); break;
        case FIELD_amount: pp->setAmount(value.doubleValue()); break;
        case FIELD_priorityClass: pp->setPriorityClass(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_hasTimeOut: pp->setHasTimeOut(value.boolValue()); break;
        case FIELD_timeOut: pp->setTimeOut(value.doubleValue()); break;
        case FIELD_largerTxnId: pp->setLargerTxnId(value.doubleValue()); break;
        case FIELD_isMarked: pp->setIsMarked(value.boolValue()); break;
        case FIELD_timeAttempted: pp->setTimeAttempted(value.doubleValue()); break;
        case FIELD_tx_fee_amount: pp->setTx_fee_amount(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ackMsg'", field);
    }
}

const char *ackMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ackMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ackMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ackMsg *pp = omnetpp::fromAnyPtr<ackMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ackMsg'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

