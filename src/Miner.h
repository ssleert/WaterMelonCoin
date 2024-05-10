#ifndef __MINER_H__
#define __MINER_H__

#include <stdint.h>
#include "BlockChain.h"
#include "Vec.h"
#include "Result.h"

typedef struct {
  uint64_t Id;
  uint64_t BlockHash;
  uint64_t SenderId;
  uint64_t RecieverId;
  uint8_t  Payload[BlockChain_TransactionPayloadSize];
  uint8_t  MagicBytes[BlockChain_TransactionMagicBytesSize];
} MinerTransactionInputData;

typedef struct {
  uint64_t Id;
  uint64_t WorkHash;
  uint8_t  MagicBytes[BlockChain_TransactionMagicBytesSize];
} MinerTransactionOutputData;

typedef struct {
  Vec(BlockChain_Block)  Blocks;
  BlockChain_Block*      PreviousBlock;
} Miner;

Miner NewMiner(void);

typedef Result(MinerTransactionOutputData) MinerProcessResult;
MinerProcessResult MinerProcess(
  Miner* Self,
  const MinerTransactionInputData* InputData
);

typedef Result(None) MinerBroadcastNewBlockResult;
MinerBroadcastNewBlockResult MinerBroadcastNewBlock(
  Miner* Self,
  const BlockChain_Block* Block
);

void FreeMiner(const Miner* Self);

#endif
