#include "BlockChain.h"
#include "xxhash.h"
#include "string.h"
#include "Utils.h"
#include <stdlib.h>

uint64_t BlockChain_TransactionGetWorkHash(
  uint64_t SenderId, 
  uint64_t RecieverId, 
  size_t PayloadLen,
  const uint8_t Payload[PayloadLen]
) {
  const size_t BytesLen = (
    sizeof(SenderId) + sizeof(RecieverId) + 
    sizeof(*Payload) * PayloadLen
  );
  uint8_t Bytes[BytesLen];
  memset(Bytes, 0, BytesLen);

  Bytes[0] = SenderId;
  Bytes[sizeof(RecieverId) * 1] = RecieverId;

  memcpy(&Bytes[sizeof(RecieverId) * 2], Payload, PayloadLen);

  return BlockChain_NewHash(BytesLen, Bytes);
}


BlockChain_Block BlockChain_NewGenesisBlock(void) {
  return (BlockChain_Block) {
    .Id = 0,
    .CreationTime = time(NULL),
    .PreviousBlockHash = 1337,
    .Hash = 20070707,
    .Payload = "i love u :)",
    .Transactions = {0},
    .TransactionsHash = 0
  };
}

bool BlockChain_BlockValid(
  const BlockChain_Block* Self,
  const BlockChain_Block* Block
) {
  return Self->PreviousBlockHash == Block->Hash;
}

uint64_t BlockChain_GetBlockHash(const BlockChain_Block* Block) {
  #define MemCpyWithSizeof(Dest, Src) (memcpy((Dest), &(Src), sizeof(Src)), (Dest) += sizeof(Src))

  uint8_t Bytes[
    sizeof(Block->Id) + sizeof(Block->PreviousBlockHash) +
    sizeof(Block->Payload) + sizeof(Block->TransactionsHash)
  ] = {0};
  
  uint8_t* BytesPtr = Bytes;

  MemCpyWithSizeof(BytesPtr, Block->Id);
  MemCpyWithSizeof(BytesPtr, Block->PreviousBlockHash);
  MemCpyWithSizeof(BytesPtr, Block->Payload);
  MemCpyWithSizeof(BytesPtr, Block->TransactionsHash);

  return BlockChain_NewHash(ArrLen(Bytes), Bytes);
}

uint64_t BlockChain_NewHash(size_t DataLen, const uint8_t Data[DataLen]) {
  return XXH3_64bits(Data, DataLen);
}

uint64_t BlockChain_GetRandomId(void) {
  uint8_t Bytes[256] = {0};

  for (size_t i = 0; i < ArrLen(Bytes); ++i) {
    Bytes[i] = (uint8_t)rand();
  }

  return BlockChain_NewHash(ArrLen(Bytes), Bytes);
}
