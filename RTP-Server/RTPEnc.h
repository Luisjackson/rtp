#ifndef RTPSERVER_RTPENC_H
#define RTPSERVER_RTPENC_H

#include "Network.h"

// O tamanho máximo de dados que enviamos num único pacote UDP.
// 1400 Para ter margem
#define RTP_PAYLOAD_MAX 1400

typedef struct {
    // Array final que vai para a rede: 1400 bytes de vídeo + 12 bytes de cabeçalho RTP 
    uint8_t cache[RTP_PAYLOAD_MAX+12]; 
    // Buffer temporário para guardar os pedaços de vídeo (NAL Units)
    uint8_t buf[RTP_PAYLOAD_MAX];
    // Um ponteiro (marcador) para saber onde estamos a escrever no buffer temporário
    uint8_t *buf_ptr;

    // Configurações da transmissão
    int aggregation;   // 0: Envia frame a frame, 1: Junta frames pequenos num só pacote
    int payload_type;  // Tipo de vídeo: 0 para H.264/AVC; 1 para HEVC/H.265

    // Campos cruciais do protocolo RTP:
    uint32_t ssrc; // Identificador único da nossa câmara/servidor (Sync Source)
    uint32_t seq; // Número de Sequência (Aumenta a cada pacote enviado)
    uint32_t timestamp;// Marca de Tempo (Para o cliente saber a que segundo pertence a imagem)
}RTPMuxContext;

// Assinaturas das funções que vamos usar no RTPEnc.c
int initRTPMuxContext(RTPMuxContext *ctx);  // COnfiguração inicial

void rtpSendH264HEVC(RTPMuxContext *ctx, UDPContext *udp, const uint8_t *buf, int size);
//Receber os dados brutos do vídeo, processar esses frames e 
// coordenar o envio contínuo para a rede através do socket UDP

#endif //RTPSERVER_RTPENC_H
