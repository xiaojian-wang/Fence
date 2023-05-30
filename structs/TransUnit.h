class TransUnit{
   public:
      double amount;
      double timeSent;  //time delay after start time of simulation that trans-unit is active
      int sender;
      int receiver;
      //vector<int> route; //includes sender and reciever as first and last entries
      int priorityClass;
      bool hasTimeOut;
      double timeOut;
      double largerTxnId;
      // OnlineScheme: need to add two more filed, one for total amount, one for destination amount, one for tx_fee_amount
      double total_amount; // add tx_fee
      double tx_fee_amount;
      // double tx_fee_total;


      TransUnit(double amount1, double timeSent1, int sender1, int receiver1, int priorityClass1, bool hasTimeOut1, double timeOut1 = -1, double largerTxnId1 = -1){
         assert((hasTimeOut1 && timeOut1>0) || (!(hasTimeOut1) && timeOut1==-1));
         amount = amount1;
         timeSent = timeSent1;
         sender = sender1;
         receiver = receiver1;
         priorityClass=  priorityClass1;
         hasTimeOut =  true; // TODO: temp, original: hasTimeOut1;
         timeOut = 5.0; // TODO: temp, original: timeOut1;
         largerTxnId = largerTxnId1;
         total_amount = amount1;
         tx_fee_amount = 0; //Note: this fee is changed
         // tx_fee_total = 0; //Note: this fee is the fee that record originally, not change, for output the txid, tx amount, tx_fee
      }
};
