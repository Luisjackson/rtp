//
// Created by Liming Shao on 2018/5/11.
//

#include "Network.h"
#include <stdio.h>
#include <string.h>

// funcao de inicializacao do canal udp
int udpInit(UDPContext *udp) {
  if (NULL == udp || NULL == udp->dstIp || 0 == udp->dstPort) {
    printf("udpInit error.\n");
    return -1;
  }

  // definicao do uso da udp
  udp->socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp->socket < 0) {
    printf("udpInit socket error.\n");
    return -1;
  }

  int tos = 0xB8; // 0xB8 = DSCP 46 (Expedited Forwarding) flag de prioridade
  if (setsockopt(udp->socket, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0) {
    perror("Erro ao definir QoS (DSCP)");
    // Não interrompemos, mas registramos que o QoS falhou
  }

  udp->servAddr.sin_family = AF_INET;
  udp->servAddr.sin_port = htons(udp->dstPort);
  inet_aton(udp->dstIp, &udp->servAddr.sin_addr);

  // test de conectividade com um byte vazio
  int num =
      (int)sendto(udp->socket, "", 1, 0, (struct sockaddr *)&udp->servAddr,
                  sizeof(udp->servAddr));
  if (num != 1) {
    printf("udpInit sendto test err. %d", num);
    return -1;
  }

  return 0;
}

// udpSend() recebe o contexto, um ponteiro para o buffer de dados e o tamanho
// do pacote, e chama o sendto()
int udpSend(const UDPContext *udp, const uint8_t *data, uint32_t len) {

  ssize_t num =
      sendto(udp->socket, data, len, 0, (struct sockaddr *)&udp->servAddr,
             sizeof(udp->servAddr));
  if (num != len) {
    printf("%s sendto err. %d %d\n", __FUNCTION__, (uint32_t)num, len);
    return -1;
  }

  return len;
}
