#include "Network.h"
#include "Pair.h"
#include "Utils.h"
#include "Xmalloc.h"

static uint64_t NetworkGetProcessorsAmount(const Network* Self) {
  return (Self->Miners.Len / 100) * NetworkPercentOfProcessorsForTransaction;
}

static void NetworkRandomMinersOrder(Network* Self) {
  VecShuffle(&Self->Miners);
}

typedef Result(Vec(MinerTransactionOutputData)) NetworkProcessTransactionWithMinersResult;
static NetworkProcessTransactionWithMinersResult NetworkProcessTransactionWithMiners(
  const Network* Self,
  MinerTransactionInputData Transaction,
  uint64_t ProcessorsAmount
) {
  NetworkProcessTransactionWithMinersResult Res = {
    .Res = NewVec(MinerTransactionOutputData, ProcessorsAmount)
  };

  for (size_t i = 0; i < ProcessorsAmount; ++i) {
    MinerProcessResult Output = MinerProcess(Self->Miners.Data[i].Instance, &Transaction);
    if (ErrorIs(&Output.Err)) {
      Res.Err = Output.Err;
      FreeVec(&Res.Res);
      return Res;
    }

    VecPush(&Res.Res, Output.Res);
  }

  return Res;
}

typedef Result(None) NetworkAddNewBlockIfNeededResult;
static NetworkAddNewBlockIfNeededResult NetworkAddNewBlockIfNeeded(
  Network* Self
) {
  if (Self->CurrentTransactions.Len == BlockChain_BlockTransactionsSize) {
    BlockChain_Block Block = {
      .Id = Self->PreviousBlock->Id + 1,
      .CreationTime = time(0),
      .PreviousBlockHash = Self->PreviousBlock->Hash,
    };

    strcpy((char*)Block.Payload, (const char*)"i love u :)");
    memcpy(Block.Transactions, Self->CurrentTransactions.Data, sizeof(Block.Transactions));
    
    Vec(uint64_t) TransactionHashes = NewVec(uint64_t, ArrLen(Block.Transactions));

    for (size_t i = 0; i < ArrLen(Block.Transactions); ++i) {
      BlockChain_Transaction Transaction = Block.Transactions[i];
      uint64_t TransactionWorkHash = BlockChain_TransactionGetWorkHash(
        Transaction.SenderId,
        Transaction.RecieverId,
        ArrLen(Transaction.Payload),
        Transaction.Payload
      ); 

      VecPush(&TransactionHashes, TransactionWorkHash);
    }

    Block.TransactionsHash = BlockChain_NewHash(TransactionHashes.Len * sizeof(uint64_t), (const uint8_t*)TransactionHashes.Data);
    FreeVec(&TransactionHashes);

    VecPush(&Self->Blocks, Block);

    return (NetworkAddNewBlockIfNeededResult) {0};
  }

  return (NetworkAddNewBlockIfNeededResult) {0};
}

typedef Result(Pair(bool, NetworkMinerWithInfo)) NetworkVerifyTransactionResult;
static NetworkVerifyTransactionResult NetworkVerifyTransaction(
  const Network* Self,
  const MinerTransactionInputData* Transaction,
  uint64_t Hash
) {
  NetworkVerifyTransactionResult Res = {0};
  NetworkMinerWithInfo RandomMiner = Self->Miners.Data[rand() % Self->Miners.Len];
  
  MinerProcessResult MinerResult = MinerProcess(RandomMiner.Instance, Transaction);
  if (ErrorIs(&MinerResult.Err)) {
    Res.Err = MinerResult.Err;
    return Res;
  }

  Res.Res.First = (Hash == MinerResult.Res.WorkHash);
  Res.Res.Second = RandomMiner;

  return Res;
}

Network NewNetwork(void) {
  Network Net = {
    .Blocks = NewVec(BlockChain_Block, 32),
    .Miners = NewVec(NetworkMinerWithInfo, 10),
    .PreviousBlock = NULL,
    .CurrentTransactions = NewVec(BlockChain_Transaction, 10),
  };

  VecPush(&Net.Blocks, BlockChain_NewGenesisBlock());
  Net.PreviousBlock = &Net.Blocks.Data[0];

  return Net;
}

void FreeNetwork(const Network* Self) {
  FreeVec(&Self->Blocks);
  FreeVec(&Self->Miners);
  FreeVec(&Self->CurrentTransactions);
}

void NetworkConnectMiner(
  Network* Self,
  const NetworkMinerWithInfo* MinerWithInfo
) {
  VecPush(&Self->Miners, *MinerWithInfo);
}

NetworkProcessTransactionResult NetworkProcessTransaction(
  Network* Self,
  uint64_t SenderId,
  uint64_t RecieverId,
  size_t PayloadLen,
  const uint8_t Payload[PayloadLen]
) { 
  NetworkProcessTransactionResult Res = {0};

  uint64_t ProcessorsAmount = NetworkGetProcessorsAmount(Self);

  MinerTransactionInputData CurrentTransaction = {
    .Id = BlockChain_GetRandomId(),
    .BlockHash = Self->PreviousBlock->Hash,
    .SenderId = SenderId,
    .RecieverId = RecieverId,
    .Payload = {0},
    .MagicBytes = {1, 3, 3, 3},
  };

  memcpy(&CurrentTransaction.Payload, Payload, PayloadLen);

  NetworkRandomMinersOrder(Self);
  NetworkProcessTransactionWithMinersResult ProcessedTransactions = NetworkProcessTransactionWithMiners(
    Self, CurrentTransaction, ProcessorsAmount
  );
  if (ErrorIs(&ProcessedTransactions.Err)) {
    Res.Err = ProcessedTransactions.Err;
    return Res;
  }

  typedef Pair(uint64_t, uint64_t) HashOccurance;
  Vec(HashOccurance) Hashes = NewVec(HashOccurance, ProcessedTransactions.Res.Len);

  for (size_t i = 0; i < ProcessedTransactions.Res.Len; ++i) {
    MinerTransactionOutputData Data = ProcessedTransactions.Res.Data[i];
    
    bool Found = false;
    for (size_t y = 0; y < Hashes.Len; ++y) {
      if (Hashes.Data[y].First == Data.WorkHash) {
        Hashes.Data[y].Second += 1;
        Found = true;
        break;
      }
    }
    if (Found == false) {
      HashOccurance Occ = {Data.WorkHash, 1};
      VecPush(&Hashes, Occ);
    }
  }

  HashOccurance LegitimateHash = {0};
  for (size_t i = 0; i < Hashes.Len; ++i) {
    if (Hashes.Data[i].Second > LegitimateHash.Second) {
      LegitimateHash = Hashes.Data[i];
    }
  }

  FreeVec(&ProcessedTransactions.Res);
  FreeVec(&Hashes);

  NetworkVerifyTransactionResult TransactionVerified = NetworkVerifyTransaction(
    Self, &CurrentTransaction, LegitimateHash.First
  );
  if (ErrorIs(&TransactionVerified.Err)) {
    Res.Err = TransactionVerified.Err;
    return Res;
  }

  if (TransactionVerified.Res.First != true) {
    Res.Err = ErrorNew("Legitimate Hash Verification Error.");
    return Res;
  }

  BlockChain_Transaction Transaction = {
    .Id = CurrentTransaction.Id,
    .BlockHash = CurrentTransaction.BlockHash,
    .CreationTime = time(0),
    .SenderId = SenderId,
    .RecieverId = RecieverId,
    .VerificatorId = TransactionVerified.Res.Second.RecieverId,
    .Payload = {0},
    .MagicBytes = {0},
    .WorkHash = LegitimateHash.First
  };

  memcpy(&Transaction.Payload, Payload, PayloadLen);
  memcpy(&Transaction.MagicBytes, CurrentTransaction.MagicBytes, sizeof(CurrentTransaction.MagicBytes));

  VecPush(&Self->CurrentTransactions, Transaction);

  NetworkAddNewBlockIfNeededResult BlockAdded = NetworkAddNewBlockIfNeeded(Self);
  if (ErrorIs(&BlockAdded.Err)) {
    Res.Err = BlockAdded.Err;
    return Res;
  }

  return (NetworkProcessTransactionResult) {0};
}
