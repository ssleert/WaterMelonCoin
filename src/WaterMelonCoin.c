#include <stdint.h>

#include "WaterMelonCoin.h"
#include "Log.h"
#include "BlockChain.h"
#include "Network.h"
#include "Miner.h"
#include "Utils.h"
#include "Xmalloc.h"

int32_t main(int32_t argc, char* argv[]) {
  LogInfo("watermeloncoin");

  Network Net = NewNetwork();

  NetworkMinerWithInfo Miners[100] = {0};
  
  for (size_t i = 0; i < ArrLen(Miners); ++i) {
    NetworkMinerWithInfo MinerWithInfo = {
      .Id = i,
      .RecieverId = i * 2,
      .Instance = Xmalloc(sizeof(Miner)),
    };

    *MinerWithInfo.Instance = NewMiner();

    NetworkConnectMiner(&Net, &MinerWithInfo);

    Miners[i] = MinerWithInfo;
  }


  const uint8_t Payload[] = "i love u :)";

  for (size_t i = 0; i < 1000000; ++i) {
    NetworkProcessTransaction(&Net, 123, 123, sizeof(Payload), Payload);
  }


  FreeNetwork(&Net);

  for (size_t i = 0; i < ArrLen(Miners); ++i) {
    FreeMiner(Miners[i].Instance);
    free(Miners[i].Instance);
  }

  return 0;
}
