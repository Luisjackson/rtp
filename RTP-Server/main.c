#include "Network.h"
#include "RTPEnc.h"
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  int res;
  int len = 0;
  uint8_t *stream = NULL;
  const char *fileName = "../Sample.h264";

  RTPMuxContext rtpMuxContext;
  UDPContext udpContext = {
      .dstIp = "127.0.0.1", // destination ip
      .dstPort = 1234       // destination port
  };

  // 1. Inicializa o Socket UDP (Agora com QoS/DSCP implementado no network.c)
  res = udpInit(&udpContext);
  if (res) {
    printf("udpInit error.\n");
    return -1;
  }

  // 2. Inicializa o Contexto RTP
  initRTPMuxContext(&rtpMuxContext);

  // 3. Lê o arquivo inteiro para a memória para evitar fragmentação de NALUs
  res = readFile(&stream, &len, fileName);
  if (res) {
    printf("readFile error.\n");
    return -1;
  }

  printf("Iniciando transmissão...\n");

  // 4. Envia o stream H.264 completo. O tempo (25 FPS) é controlado internamente por rtpSendH264HEVC.
  rtpSendH264HEVC(&rtpMuxContext, &udpContext, stream, len);

  printf("Transmissão finalizada.\n");

  free(stream);

  return 0;
}