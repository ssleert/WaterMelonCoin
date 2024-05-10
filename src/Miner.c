#include "Miner.h"

enum {
  BlocksForFirstAllocation = 32,
};

Miner NewMiner(void) {
  return (Miner) {
    .Blocks = NewVec(BlockChain_Block, BlocksForFirstAllocation),
    .PreviousBlock = NULL,
  };
}

MinerProcessResult MinerProcess(
  Miner* Self,
  const MinerTransactionInputData* InputData
) {
  if ((Self->PreviousBlock != NULL) && (Self->PreviousBlock->Hash == InputData->BlockHash)) {
    return (MinerProcessResult) {
      .Err = ErrorNew("Transaction Connected Block Hash not equal to PreviousBlock hash.")
    };
  }

  MinerProcessResult OutputData = {
    .Res = {
      .Id = InputData->Id,
      .WorkHash = BlockChain_TransactionGetWorkHash(
        InputData->SenderId,
        InputData->RecieverId,
        sizeof(InputData->Payload),
        InputData->Payload
      )
    }
  };

  memcpy(&OutputData.Res.MagicBytes, InputData->MagicBytes, sizeof(OutputData.Res.MagicBytes));

  return OutputData;
}

MinerBroadcastNewBlockResult MinerBroadcastNewBlock(
  Miner* Self,
  const BlockChain_Block* Block
) {
  if ((Self->PreviousBlock != NULL) && !(BlockChain_BlockValid(Block, Self->PreviousBlock))) {
    return (MinerBroadcastNewBlockResult) {
      .Err = ErrorNew("Previous Block Hash Not Equal To Broadcasted Block Connected Hash.")
    };
  }
  
  VecPush(&Self->Blocks, *Block);
  Self->PreviousBlock = &Self->Blocks.Data[Self->Blocks.Len - 1];

  return (MinerBroadcastNewBlockResult) {0};
}

void FreeMiner(const Miner* Self) {
  FreeVec(&Self->Blocks);
}
