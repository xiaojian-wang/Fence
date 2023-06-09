//
// Generated file, do not edit! Created by opp_msgtool 6.0 from transactionMsg.msg.
//

#ifndef __TRANSACTIONMSG_M_H
#define __TRANSACTIONMSG_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif

class transactionMsg;
// cplusplus {{
#include <vector>
typedef std::vector<double> DoubleVector;
// }}

/**
 * Class generated from <tt>transactionMsg.msg:25</tt> by opp_msgtool.
 * <pre>
 * packet transactionMsg
 * {
 *     double amount;
 *     double timeSent;  //time after start time that job is active
 *     int sender;
 *     int receiver;
 *     int priorityClass;
 *     int transactionId; //is messageID of transactionMsg
 *     bool hasTimeOut;
 *     double timeOut;
 *     int htlcIndex;
 *     int pathIndex;
 *     bool isAttempted;
 *     double largerTxnId;
 *     bool isMarked;
 *     double timeAttempted;
 *     double total_amount; // OnlineScheme:
 *     double tx_fee_amount;
 *     DoubleVector convolution_flow;
 *     double tx_fee_total;
 * }
 * </pre>
 */
class transactionMsg : public ::omnetpp::cPacket
{
  protected:
    double amount = 0;
    double timeSent = 0;
    int sender = 0;
    int receiver = 0;
    int priorityClass = 0;
    int transactionId = 0;
    bool hasTimeOut = false;
    double timeOut = 0;
    int htlcIndex = 0;
    int pathIndex = 0;
    bool isAttempted_ = false;
    double largerTxnId = 0;
    bool isMarked_ = false;
    double timeAttempted = 0;
    double total_amount = 0;
    double tx_fee_amount = 0;
    DoubleVector convolution_flow;
    double tx_fee_total = 0;

  private:
    void copy(const transactionMsg& other);

  protected:
    bool operator==(const transactionMsg&) = delete;

  public:
    transactionMsg(const char *name=nullptr, short kind=0);
    transactionMsg(const transactionMsg& other);
    virtual ~transactionMsg();
    transactionMsg& operator=(const transactionMsg& other);
    virtual transactionMsg *dup() const override {return new transactionMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual double getAmount() const;
    virtual void setAmount(double amount);

    virtual double getTimeSent() const;
    virtual void setTimeSent(double timeSent);

    virtual int getSender() const;
    virtual void setSender(int sender);

    virtual int getReceiver() const;
    virtual void setReceiver(int receiver);

    virtual int getPriorityClass() const;
    virtual void setPriorityClass(int priorityClass);

    virtual int getTransactionId() const;
    virtual void setTransactionId(int transactionId);

    virtual bool getHasTimeOut() const;
    virtual void setHasTimeOut(bool hasTimeOut);

    virtual double getTimeOut() const;
    virtual void setTimeOut(double timeOut);

    virtual int getHtlcIndex() const;
    virtual void setHtlcIndex(int htlcIndex);

    virtual int getPathIndex() const;
    virtual void setPathIndex(int pathIndex);

    virtual bool isAttempted() const;
    virtual void setIsAttempted(bool isAttempted);

    virtual double getLargerTxnId() const;
    virtual void setLargerTxnId(double largerTxnId);

    virtual bool isMarked() const;
    virtual void setIsMarked(bool isMarked);

    virtual double getTimeAttempted() const;
    virtual void setTimeAttempted(double timeAttempted);

    virtual double getTotal_amount() const;
    virtual void setTotal_amount(double total_amount);

    virtual double getTx_fee_amount() const;
    virtual void setTx_fee_amount(double tx_fee_amount);

    virtual const DoubleVector& getConvolution_flow() const;
    virtual DoubleVector& getConvolution_flowForUpdate() { return const_cast<DoubleVector&>(const_cast<transactionMsg*>(this)->getConvolution_flow());}
    virtual void setConvolution_flow(const DoubleVector& convolution_flow);

    virtual double getTx_fee_total() const;
    virtual void setTx_fee_total(double tx_fee_total);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const transactionMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, transactionMsg& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

inline any_ptr toAnyPtr(const DoubleVector *p) {if (auto obj = as_cObject(p)) return any_ptr(obj); else return any_ptr(p);}
template<> inline DoubleVector *fromAnyPtr(any_ptr ptr) { return ptr.get<DoubleVector>(); }
template<> inline transactionMsg *fromAnyPtr(any_ptr ptr) { return check_and_cast<transactionMsg*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __TRANSACTIONMSG_M_H

