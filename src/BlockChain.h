#ifndef __BLOCKCHAIN_H__
#define __BLOCKCHAIN_H__

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

enum {
  BlockChain_TransactionPayloadSize = 4096,
  BlockChain_TransactionMagicBytesSize = 64,
};

typedef struct {
  uint64_t Id;
  uint64_t BlockHash;
  time_t   CreationTime;
  uint64_t SenderId;
  uint64_t RecieverId;
  uint64_t VerificatorId;
  uint8_t  Payload[BlockChain_TransactionPayloadSize];
  uint8_t  MagicBytes[BlockChain_TransactionMagicBytesSize];
  uint64_t WorkHash;
} BlockChain_Transaction;

uint64_t BlockChain_TransactionGetWorkHash(
  uint64_t SenderId, 
  uint64_t RecieverId, 
  size_t PayloadSize,
  const uint8_t Payload[PayloadSize]
);

enum {
  BlockChain_BlockPayloadSize = 4096,
  BlockChain_BlockTransactionsSize = 128,
};

typedef struct {
  uint64_t               Id;
  time_t                 CreationTime;
  uint64_t               PreviousBlockHash;
  uint64_t               Hash;
  uint8_t                Payload[BlockChain_BlockPayloadSize];
  BlockChain_Transaction Transactions[BlockChain_BlockTransactionsSize];
  uint64_t               TransactionsHash;
} BlockChain_Block;

BlockChain_Block BlockChain_NewGenesisBlock(void);
bool BlockChain_BlockValid(
  const BlockChain_Block* Self,
  const BlockChain_Block* Block
);

uint64_t BlockChain_GetBlockHash(const BlockChain_Block* Block);

uint64_t BlockChain_NewHash(size_t DataLen, const uint8_t Data[DataLen]);
uint64_t BlockChain_GetRandomId(void);

#endif

