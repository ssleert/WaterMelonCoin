#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "Vec.h"
#include "BlockChain.h"
#include "Miner.h"
#include "Result.h"

enum {
  NetworkPercentOfProcessorsForTransaction = 10
};

typedef struct {
  uint64_t Id;
  uint64_t RecieverId;
  Miner* Instance;
} NetworkMinerWithInfo;

typedef struct {
  Vec(BlockChain_Block)       Blocks;
  Vec(NetworkMinerWithInfo)   Miners;
  BlockChain_Block*           PreviousBlock;
  Vec(BlockChain_Transaction) CurrentTransactions;
} Network;

Network NewNetwork(void);
void FreeNetwork(const Network* Self);

void NetworkConnectMiner(
  Network* Self,
  const NetworkMinerWithInfo* MinerWithInfo
);

typedef Result(None) NetworkProcessTransactionResult;
NetworkProcessTransactionResult NetworkProcessTransaction(
  Network* Self,
  uint64_t SenderId,
  uint64_t RecieverId,
  size_t PayloadLen,
  const uint8_t Payload[PayloadLen]
);

#endif
