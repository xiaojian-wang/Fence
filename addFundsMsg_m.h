//
// Generated file, do not edit! Created by opp_msgtool 6.0 from addFundsMsg.msg.
//

#ifndef __ADDFUNDSMSG_M_H
#define __ADDFUNDSMSG_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif

class addFundsMsg;
// cplusplus {{
#include <map>
typedef std::map<int,double> FundsVector;
// }}

/**
 * Class generated from <tt>addFundsMsg.msg:24</tt> by opp_msgtool.
 * <pre>
 * packet addFundsMsg
 * {
 *     FundsVector pcsNeedingFunds;
 * }
 * </pre>
 */
class addFundsMsg : public ::omnetpp::cPacket
{
  protected:
    FundsVector pcsNeedingFunds;

  private:
    void copy(const addFundsMsg& other);

  protected:
    bool operator==(const addFundsMsg&) = delete;

  public:
    addFundsMsg(const char *name=nullptr, short kind=0);
    addFundsMsg(const addFundsMsg& other);
    virtual ~addFundsMsg();
    addFundsMsg& operator=(const addFundsMsg& other);
    virtual addFundsMsg *dup() const override {return new addFundsMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual const FundsVector& getPcsNeedingFunds() const;
    virtual FundsVector& getPcsNeedingFundsForUpdate() { return const_cast<FundsVector&>(const_cast<addFundsMsg*>(this)->getPcsNeedingFunds());}
    virtual void setPcsNeedingFunds(const FundsVector& pcsNeedingFunds);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const addFundsMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, addFundsMsg& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

inline any_ptr toAnyPtr(const FundsVector *p) {if (auto obj = as_cObject(p)) return any_ptr(obj); else return any_ptr(p);}
template<> inline FundsVector *fromAnyPtr(any_ptr ptr) { return ptr.get<FundsVector>(); }
template<> inline addFundsMsg *fromAnyPtr(any_ptr ptr) { return check_and_cast<addFundsMsg*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __ADDFUNDSMSG_M_H

